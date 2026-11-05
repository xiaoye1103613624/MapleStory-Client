//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "LoginHandlers.h"

#include "Helpers/LoginParser.h"

#include "../Packets/LoginPackets.h"

#include "../../IO/UI.h"

#include "../../IO/UITypes/UICharSelect.h"
#include "../../IO/UITypes/UIGender.h"
#include "../../IO/UITypes/UILoginNotice.h"
#include "../../IO/UITypes/UILoginWait.h"
#include "../../IO/UITypes/UIRaceSelect.h"
#include "../../IO/UITypes/UITermsOfService.h"
#include "../../IO/UITypes/UIWorldSelect.h"
#include "../../IO/UIStateLogin.h"
#include "../../IO/UITypes/UIViewAllChar.h"

#include "../../Configuration.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/Window.h"
#include "../../Util/Resource/ResourceService.h"

#include <chrono>
#include <thread>

namespace ms
{
	void LoginResultHandler::handle(InPacket& recv) const
	{
		auto loginwait = UI::get().get_element<UILoginWait>();

		// 修复（2026-11-03 实机：创建流程多 UI 态同屏叠加）：这三项清理原只在
		// loginwait 活跃分支内执行；Wait 被门闩看护提前回收（UIStateLogin.cpp:86
		// 的 inactive→remove）或竞态失活时走 else，GENDER/TOS/LOGINNOTICE 残留
		// active，跨到频道/选角/创建页与后续窗口叠加。LOGIN_RESULT 到达即代表
		// 上一轮登录/性别/EULA 对话终结——无论 Wait 是否还在，都必须拆干净。
		UI::get().remove(UIElement::Type::LOGINNOTICE);
		UI::get().remove(UIElement::Type::TOS);
		UI::get().remove(UIElement::Type::GENDER);

		if (loginwait && loginwait->is_active())
		{
			// 失败路径才立刻拆掉 Wait；成功路径要等 ensure_game_roots 结束再拆，
			// 否则装 Character 时无加载框，像“登录卡死半分钟”。

			std::function<void()> okhandler = loginwait->get_handler();

			// The packet should contain a 'reason' integer which can signify various things
			if (int32_t reason = recv.read_int())
			{
				while (recv.available())
					recv.skip_byte();
				UI::get().remove(UIElement::Type::LOGINWAIT);

				// Login unsuccessful
				// The LoginNotice displayed will contain the specific information
				switch (reason)
				{
					case 2:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::BLOCKED_ID, okhandler);
						break;
					}
					case 5:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NOT_REGISTERED, okhandler);
						break;
					}
					case 7:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::ALREADY_LOGGED_IN, okhandler);
						break;
					}
					case 13:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNABLE_TO_LOGIN_WITH_IP, okhandler);
						break;
					}
					case 23:
					{
						UI::get().emplace<UITermsOfService>(okhandler);
						break;
					}
					default:
					{
						// 未穷举的 reason：统一展示通用错误，避免 int8 截断回绕。
						// 原 static_cast<int8_t>(reason - 1) 在 reason > 128 时会回绕成负
						// 值，再隐式转 uint16_t 当 Message id → 错乱文案。v83 登录失败
						// reason 是固定小集合（case 2/5/7/13/23 已覆盖），其余归一到通用错误。
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNKNOWN_ERROR, okhandler);

						break;
					}
					}
					}
					else
					{
					// Login successful
				// The packet contains information on the account, so we initialize the account with it.
				Account account = LoginParser::parse_account(recv);
				while (recv.available())
					recv.skip_byte();

				Configuration::get().set_admin(account.admin);

				// img：选角根已在启动时后台预热（prewarm_game_roots）。
				// 这里等待其就绪；若尚未完成（预热比登录响应慢），期间持续
				// 泵 update/draw，让 Wait 弹窗动画继续而不是冻屏。
				// （旧行为：同步 ensure_game_roots() 冻结主循环半分钟级，
				// 表现为「连接加载条卡住不动，过一会才动」。）
				if (!ms::res::ResourceService::get().game_roots_ready())
				{
					while (!ms::res::ResourceService::get().game_roots_ready())
					{
						Window::get().poll_events();
						UI::get().update();
						Window::get().begin();
						Stage::get().draw(0.0f);
						// UI::draw 的形参在 UI 侧是不透明度（见 MapleStory::draw 注释），
						// 传 0 会把 Wait 弹窗整个画成透明，泵帧等于黑屏。
						UI::get().draw(1.0f);
						Window::get().end();
						std::this_thread::sleep_for(std::chrono::milliseconds(15));
					}
				}

				// 参考端 Loading 进度条走满 0→100% 后才切页；未满就拆 Wait 会
				// 「没到 100% 就跳频道页」。这里把续场逻辑交给 Wait，由它在
				// update() 里满条后执行（Wait 自身 deactivate，门闩看护下一帧回收）。
				std::function<void()> oncomplete = [okhandler, account]()
				{
					if (account.female == 10)
					{
						UI::get().emplace<UIGender>(okhandler);
						return;
					}

					// Prefer a non-empty server account name; never wipe a typed ID
					// that UILogin already persisted when "save account" is on.
					if (Setting<SaveLogin>::get().load() && !account.name.empty())
					{
						Setting<DefaultAccount>::get().save(account.name);
						Configuration::get().save();
					}

					// Request the list of worlds and channels online
					ServerRequestPacket().dispatch();
				};

				if (loginwait && loginwait->is_active())
					loginwait->begin_finish(std::move(oncomplete));
				else
					oncomplete();
			}
		}
	}

	void ServerStatusHandler::handle(InPacket& recv) const
	{
		// Possible values for status:
		// 0 - Normal
		// 1 - Highly populated
		// 2 - Full
		//
		// 服务端依据：ServerStatusRequestHandler → World.getWorldCapacityStatus()，
		// 世界不存在时回 2（PacketCreator.getServerStatus 只写一个 short）。
		// 客户端语义：1 提示人口过多（UILoginNotice::POPULATION_TOO_HIGH）仍可进入，
		// 2 额外阻止进入该世界；交由 UIWorldSelect 统一处理。
		const int16_t status = recv.read_short();
		while (recv.available())
			recv.skip_byte();

		if (auto worldselect = UI::get().get_element<UIWorldSelect>())
			worldselect->set_server_status(status);
	}

	void SelectCharacterHandler::handle(InPacket& recv) const
	{
		std::function<void()> okhandler = []() {};

		// The packet should contain a 'reason' integer which can signify various things
		if (int16_t reason = recv.read_short())
		{
			while (recv.available())
				recv.skip_byte();
			// 选角失败：复位请求门闩，允许玩家重新选择
			if (auto charselect = UI::get().get_element<UICharSelect>())
				charselect->clear_pending();

			// Select character unsuccessful
			// The LoginNotice displayed will contain the specific information
			switch (reason)
			{
				case 2:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::BLOCKED_ID, okhandler);
					break;
				}
				case 5:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NOT_REGISTERED, okhandler);
					break;
				}
				case 7:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::ALREADY_LOGGED_IN, okhandler);
					break;
				}
				case 13:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNABLE_TO_LOGIN_WITH_IP, okhandler);
					break;
				}
				case 23:
				{
					UI::get().emplace<UITermsOfService>(okhandler);
					break;
				}
				default:
				{
					// 与 LoginResultHandler 同构：未穷举 reason 归一至通用错误，
					// 避免 int8 截断回绕导致选角失败文案错乱。
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNKNOWN_ERROR, okhandler);

					break;
				}
			}
		}
		else
		{
			while (recv.available())
				recv.skip_byte();
		}
	}

	void ServerlistHandler::handle(InPacket& recv) const
	{
		auto worldselect = UI::get().get_element<UIWorldSelect>();

		if (!worldselect)
			worldselect = UI::get().emplace<UIWorldSelect>();

		// BeiDou/s9：每个世界单独一个 SERVERLIST 包，最后另发 0xFF 结束包。
		// 旧逻辑在每个包开头 clear_worlds()，结束包会把刚加的世界清光 → 木牌全是空 e。
		// 清列表改由 add_world：若上一轮已 draw_world 定稿，再来新世界时自动清空。
		while (recv.available())
		{
			World world = LoginParser::parse_world(recv);

			if (world.id != -1)
			{
				LOG(LOG_INFO, "[Serverlist] world id=" << static_cast<int>(world.id)
					<< " name=" << world.name
					<< " channels=" << static_cast<int>(world.channel_count)
					<< " loads=" << world.channel_capacities.size());
				// 顺手把「本世界频道数」登记进 Configuration：登录页用完就拆，
				// 游戏内的「频道变更」窗还要靠它决定排几行几格（UIChannel）。
				Configuration::get().set_channel_total(world.id, world.channel_count);
				worldselect->add_world(world);
			}
			else
			{
				UI::get().remove(UIElement::Type::LOGIN);
				worldselect->draw_world();
				return;
			}
		}
	}

	void CharlistHandler::handle(InPacket& recv) const
	{
		// 旧实现要求「存在且活动的 UILoginWait」才处理，否则整包角色列表被静默丢弃。
		// 但 Wait 取消后 UIStateLogin 会在下一帧把它移除（见门闩看护），
		// 重连/补发场景回包到达时 Wait 已不在 → 选角页永远出不来。
		// 这里只保留 Wait 的清理语义，不再把它当必要条件。
		auto loginwait = UI::get().get_element<UILoginWait>();

		{
			uint8_t channel_id = recv.read_byte();

			// Parse all characters
			std::vector<CharEntry> characters;
			int8_t charcount = recv.read_byte();

			for (uint8_t i = 0; i < charcount; ++i)
				characters.emplace_back(LoginParser::parse_charentry(recv));

			int8_t pic = recv.read_byte();
			int32_t slots = recv.read_int();
			while (recv.available())
				recv.skip_byte();

			// 选角 CharLook 构造依赖 BodyDrawInfo；登录成功虽已 ensure，
			// 频道包到达前若 Character 未绑全会得到空锚点 → 头身脚叠成白影。
			// 在拆 Wait / 建 UICharSelect 前再保一次（已绑则几乎无开销）。
			ms::res::ResourceService::get().ensure_game_roots();
			Window::get().poll_events();

			// Remove previous UIs
			UI::get().remove(UIElement::Type::LOGINNOTICE);

			// 与登录成功同 rule：进度条满 0→100% 后再拆 Wait、建选角页。
			// 续场逻辑由 Wait 在 update() 里满条后执行（含 remove LOGINWAIT，
			// 经门闩看护回收），避免「没到 100% 就跳页」。
			std::function<void()> oncomplete = [characters, charcount, slots, pic]() mutable
			{
				UI::get().remove(UIElement::Type::LOGINWAIT);

				// Remove the world selection screen
				if (auto worldselect = UI::get().get_element<UIWorldSelect>())
					worldselect->remove_selected();

				// Add the character selection screen
				UI::get().emplace<UICharSelect>(characters, charcount, slots, pic);
			};

			if (loginwait && loginwait->is_active())
				loginwait->begin_finish(std::move(oncomplete));
			else
				oncomplete();
		}
	}

	void ServerIPHandler::handle(InPacket& recv) const
	{
		recv.skip_byte();

		// 这条路径下面自己会发 PLAYER_LOGIN，所以先把可能残留的「换频道待重入」
		// 标记清掉（上一次换频道若没收到回包就会留着）——否则 parse_login 会拿
		// 上一个角色的 charId 抢发一次入场，把角色换错人。
		LoginParser::clear_channel_reentry();

		LoginParser::parse_login(recv);

		int32_t cid = recv.read_int();
		while (recv.available())
			recv.skip_byte();

		// 存一份：换频道时 CHANGE_CHANNEL 的回包不带 charId，
		// 重连后由 LoginParser::parse_login 用这个值补发 PLAYER_LOGIN。
		LoginParser::note_channel_entry_cid(cid);

		PlayerLoginPacket(cid).dispatch();
	}

	void CharnameResponseHandler::handle(InPacket& recv) const
	{
		// Read the name and if it is already in use
		std::string name = recv.read_string();
		bool used = recv.read_bool();

		while (recv.available())
			recv.skip_byte();
		// Notify the character creation screen
		if (auto raceselect = UI::get().get_element<UIRaceSelect>())
			raceselect->send_naming_result(used);
		LOG(LOG_DEBUG, "CharnameResponse name=" << name << " used=" << used);
		(void)name;
	}

	void AddNewCharEntryHandler::handle(InPacket& recv) const
	{
		// 首字节为结果码：服务端 PacketCreator.addNewCharEntry 恒写 0 后接角色条目；
		// 创建失败走的是 DELETE_CHAR_RESPONSE(cid=0, state=9)（CreateCharHandler 的
		// 非法名/非法外观分支），不会带条目。旧实现无条件解析，失败包会把空条目
		// 插进选角页（表现为凭空多出一个空白角色）。
		const uint8_t result = recv.read_byte();

		if (result != 0 || !recv.available())
		{
			while (recv.available())
				recv.skip_byte();
			return;
		}

		// Parse info on the new character
		CharEntry character = LoginParser::parse_charentry(recv);
		while (recv.available())
			recv.skip_byte();

		// Read the updated character selection
		if (auto charselect = UI::get().get_element<UICharSelect>())
			charselect->add_character(std::move(character));
	}

	void DeleteCharResponseHandler::handle(InPacket& recv) const
	{
		// Read the character id and if deletion was successful (PIC was correct)
		int32_t cid = recv.read_int();
		uint8_t state = recv.read_byte();
		while (recv.available())
			recv.skip_byte();

		// 删除回包到达：无论成功/失败都复位请求门闩
		if (auto charselect = UI::get().get_element<UICharSelect>())
			charselect->clear_pending();

		// Extract information from the state byte
		if (state)
		{
			UILoginNotice::Message message;

			switch (state)
			{
				// 状态码以 s9 DeleteCharHandler 实际下发为准：
				//   0x09 未知错误 / 0x14 PIC 错误 / 0x16 公会会长 /
				//   0x1A 待世界转移 / 0x1D 有家族（0 为成功，走 else 分支）。
				// 注意：s9 删除流程从不发 0x0A/0x12，旧实现把 case 10
				// 误映射为 BIRTHDAY_INCORRECT 是错的（那是登录/BIRTHDAY 流程）。
				case 0x14: // 20: PIC 错误
					message = UILoginNotice::Message::INCORRECT_PIC;
					break;
				case 0x16: // 22: 公会会长
					message = UILoginNotice::Message::CANNOT_DELETE_GUILD_LEADER;
					break;
				case 0x1A: // 26: 待世界转移（无专门枚举，复用通用错误）
					message = UILoginNotice::Message::UNKNOWN_ERROR;
					break;
				case 0x1D: // 29: 有家族
					message = UILoginNotice::Message::CANNOT_DELETE_FAMILY_LEADER;
					break;
				case 0x09: // 9: 未知错误
				default:
					message = UILoginNotice::Message::UNKNOWN_ERROR;
			}

			UI::get().emplace<UILoginNotice>(message);
		}
		else
		{
			if (auto charselect = UI::get().get_element<UICharSelect>())
				charselect->remove_character(cid);
		}
	}

	void GenderDoneHandler::handle(InPacket& recv) const
	{
		// 标准 v83：设置性别成功后服务端回 GENDER_DONE，客户端读 gender 后请求世界列表。
		// 注意：s9 的 SetGenderHandler 成功后回的是 LOGIN_RESULT（auth success），
		// 性别流程已在该 handler 闭环（LoginResultHandler 读 account.female）；
		// 此 opcode 在 s9 下不会下发，注册仅作协议完整性保留，连参考端/未来 s9 启用时生效。
		// 修复（2026-11-03）：参考端下发 GENDER_DONE 时走的是本 handler——旧代码
		// 只发 ServerRequestPacket，从不拆 UIGender 欢迎框与 UILoginWait，导致
		// 性别欢迎框跨到频道/创建页残留叠加（用户实机问题 4 的第一条泄漏路径）。
		recv.read_byte(); // gender
		while (recv.available())
			recv.skip_byte();

		UI::get().remove(UIElement::Type::GENDER);
		UI::get().remove(UIElement::Type::LOGINWAIT);

		ServerRequestPacket().dispatch();
	}

	void ConfirmEulaResultHandler::handle(InPacket& recv) const
	{
		// 标准 v83：EULA 确认结果（1=成功）。s9 的 AcceptToSHandler 同样回 LOGIN_RESULT，
		// 此 opcode 在 s9 下不会下发；注册仅作协议完整性保留。
		uint8_t ok = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		while (recv.available())
			recv.skip_byte();
		if (ok == 1)
		{
			UI::get().remove(UIElement::Type::TOS);

			ServerRequestPacket().dispatch();
		}
	}

	void CheckPincodeHandler::handle(InPacket& recv) const
	{
		// 服务端 PIN 操作（PacketCreator.pinOperation）：
		// mode 0=接受 / 1=注册新 PIN / 2=无效重输 / 3=系统错误 / 4=请输入 PIN。
		// s9 默认 enable_pin=false 时不下发；enable_pin=true 时登录后要求输入 PIN。
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0xFF;
		while (recv.available())
			recv.skip_byte();
		switch (mode)
		{
			case 0: // accepted：流程继续，无需提示
				break;
			case 1: // register a new PIN
				UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NEW_PIC_REQ);
				break;
			case 2: // invalid pin / reenter
				UI::get().emplace<UILoginNotice>(UILoginNotice::Message::INCORRECT_PIC);
				break;
			case 3: // connection failed due to system error
				UI::get().emplace<UILoginNotice>(UILoginNotice::Message::AN_ERROR_OCCURED);
				break;
			case 4: // enter the pin
				UI::get().emplace<UILoginNotice>(UILoginNotice::Message::PIC_REQ);
				break;
			default:
				UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNKNOWN_ERROR);
				break;
		}
	}

	void UpdatePincodeHandler::handle(InPacket& recv) const
	{
		// 服务端 PIN 注册结果（PacketCreator.pinRegistered）：writeByte(0) = 注册成功。
		uint8_t code = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0xFF;
		while (recv.available())
			recv.skip_byte();
		if (code == 0)
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::PIC_ACTIVATED);
		else
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNKNOWN_ERROR);
	}

	void ViewAllCharHandler::handle(InPacket& recv) const
	{
		// s9 PacketCreator（与标准 v83 一致）：
		//  showAllCharacter      : writeByte(totalChrs>0?1:5) + writeInt(totalWorlds) + writeInt(totalChrs)
		//  showAllCharacterInfo  : writeByte(0) + writeByte(worldid) + writeByte(count)
		//                          + 逐项 addCharEntry(p, chr, true) + writeByte(usePic?1:2)
		// 首字节区分两类包：0 = 某世界详情，其余(1/5) = 概览。
		const uint8_t header = recv.read_byte();

		if (header == 0)
		{
			const uint8_t worldid = recv.read_byte();
			const uint8_t count = recv.read_byte();

			UIStateLogin::ViewAllCharWorld world;
			world.worldid = worldid;

			for (uint8_t i = 0; i < count && recv.available(); ++i)
				world.chars.emplace_back(LoginParser::parse_charentry(recv));

			if (recv.available())
				world.use_pic = recv.read_byte() == 1;

			UIStateLogin::viewall_worlds.push_back(std::move(world));

			// 无结束标记：以概览包的 totalWorlds 为准，详情收齐后展示 UIViewAllChar。
			if (UIStateLogin::viewall_expected_worlds > 0 &&
				static_cast<int32_t>(UIStateLogin::viewall_worlds.size()) >= UIStateLogin::viewall_expected_worlds)
			{
				UIStateLogin::viewall_ready = true;
				UI::get().emplace<UIViewAllChar>();
			}
		}
		else
		{
			// 概览包：totalWorlds(int) + totalChrs(int)。header 1/5 仅表示是否有角色，
			// 详情由后续 info 包给出。新一次查看先重置累积缓冲，待 UIViewAllChar 消费。
			UIStateLogin::viewall_expected_worlds = recv.read_int();
			recv.read_int(); // totalChrs

			UIStateLogin::viewall_worlds.clear();
			UIStateLogin::viewall_ready = false;
		}
		while (recv.available())
			recv.skip_byte();
	}
}
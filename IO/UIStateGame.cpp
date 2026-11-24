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
#include "UIStateGame.h"

#include <fstream>


#include "UI.h"
#include "Window.h"

#include "../Data/EquipData.h"

#include "UITypes/UIBuffList.h"
#include "UITypes/UICooldownList.h"
#include "UITypes/UITemporaryStatView.h"
#include "UITypes/UIEnergyBar.h"
#include "UITypes/UIComboCounter.h"
#include "UITypes/UIBossHP.h"
#include "UITypes/UIClock.h"
#include "UITypes/UIChannel.h"
#include "UITypes/UICharInfo.h"
#include "UITypes/UIChat.h"
#include "UITypes/UIChatBar.h"
#include "UITypes/UIMessenger.h"
#include "UITypes/UIEquipInventory.h"
#include "UITypes/UIEvent.h"
#include "UITypes/UISpecial.h"
#include "UITypes/UIItemInventory.h"
#include "UITypes/UIKeyConfig.h"
#include "UITypes/UIMiniMap.h"
#include "UITypes/UISideIconBar.h"
#include "UITypes/UIQuestLog.h"
#include "UITypes/UIQuit.h"
#include "UITypes/UIShop.h"
#include "UITypes/UISkillBook.h"
#include "UITypes/UIStatsInfo.h"
#include "UITypes/UIStatusBar.h"
#include "UITypes/UIStatusMessenger.h"
#include "UITypes/UIUserList.h"
#include "UITypes/UIWorldMap.h"
#include "UITypes/UIMonsterBook.h"
#include "UITypes/UIMaker.h"
#include "UITypes/UIMedal.h"
#include "UITypes/UIMiscShells.h"
#include "UITypes/UIGuild.h"
#include "UITypes/UIFamily.h"
#include "UITypes/UIPartySearch.h"
#include "UITypes/UILoginNotice.h"
#include "../Net/Packets/FamilyPackets.h"

#include "../Net/Packets/GameplayPackets.h"
#include "../Net/Packets/PlayerInteractionPackets.h"
#include "../Gameplay/Stage.h"
#include "../Template/Rectangle.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include "../Util/HudLayout.h"
#include "../Util/Misc.h"
#include "../Util/ResolutionTiers.h"
#include "../Util/UiAssetResolve.h"
#include "../Util/UiTheme.h"

namespace ms
{
	UIStateGame::UIStateGame() : stats(Stage::get().get_player().get_stats()), dragged(nullptr)
	{

		focused = UIElement::Type::NONE;
		tooltipparent = Tooltip::Parent::NONE;

		const CharLook& look = Stage::get().get_player().get_look();
		const Inventory& inventory = Stage::get().get_player().get_inventory();

		emplace<UIStatusMessenger>();

		// Prefer StatusBar3; otherwise classic StatusBar.img (BeiDou / v83).
		if (UiAssetResolve::has_game_hud())
		{
			emplace<UIStatusBar>(stats);
			emplace<UIChatBar>();
		}
		else
		{
			LOG(LOG_WARN, "UIStateGame: no StatusBar3.img or StatusBar.img/base — skipping status/chat HUD");
		}

		emplace<UIMiniMap>(stats);
		emplace<UISideIconBar>();
		emplace<UIBuffList>();
		emplace<UICooldownList>();
		emplace<UITemporaryStatView>();
		if (auto tsv = UI::get().get_element<UITemporaryStatView>())
			tsv->deactivate();
		emplace<UIEnergyBar>();
		emplace<UIComboCounter>();
		emplace<UIBossHP>();
		emplace<UIClock>();

		// Shop window is inactive until an NPC opens it; still construct only
		// when shop UI assets exist so missing Shop2 cannot affect enter-game.
		if (UiAssetResolve::usable(UiAssetResolve::shop_root()))
		{
			emplace<UIShop>(look, inventory);
		}

		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();
	}

	void UIStateGame::draw(float inter, Point<int16_t> cursor) const
	{
		bool chat_drawn = false;

		for (auto& type : elementorder)
		{
			auto& element = elements[type];

			// W4-chatbar / 2026-11-15 真机还原：状态栏底条的 base/backgrnd2 是
			// 570 宽半透明白带（局部 y4..28、α204..255），一旦它后画就会把聊天栏
			// base/chatTarget 的深蓝框 (51,85,119) 稀释成浅灰白 (241,244,246) ——
			// 正是用户「图2」里「对所有人」白字压白底看不见的真因（真机帧全帧
			// 深蓝 0 命中）。原生 Angel 里底栏与 chatTarget 同属 CUIStatusBar 一次
			// Draw，天然 chatTarget 最后画；本端拆成 STATUSBAR / CHATBAR 两个 element
			// 后引入了 elementorder 依赖，视口重建 / bring_to_front 任一路径把
			// STATUSBAR 甩到队尾都会翻层级。这里把 CHATBAR 从正序遍历摘出，紧贴
			// STATUSBAR 之后补画，锁死「聊天栏压状态栏」；弹窗（后 emplace）仍在
			// CHATBAR 之后画，层级不变。
			if (type == UIElement::Type::CHATBAR)
				continue;

			if (element && element->is_active())
				element->draw(inter);

			if (type == UIElement::Type::STATUSBAR)
			{
				if (auto& chat = elements[UIElement::Type::CHATBAR])
				{
					if (chat->is_active())
						chat->draw(inter);
				}
				chat_drawn = true;
			}
		}

		// elementorder 里没有 STATUSBAR（异常/未建）时兜底，避免聊天栏整块不画。
		if (!chat_drawn)
		{
			if (auto& chat = elements[UIElement::Type::CHATBAR])
			{
				if (chat->is_active())
					chat->draw(inter);
			}
		}

		if (tooltip)
			tooltip->draw(cursor + Point<int16_t>(0, 22));

		if (draggedicon)
			draggedicon->dragdraw(cursor);
	}

	void UIStateGame::update()
	{
		// SysOpt 仍 active 时绝不能重建 StatusBar（模态下切档会挂死）。
		// 视口 Constants 已由 Window::check_events 在本帧更早落盘（queue → apply）。
		const bool sysopt_live = [&]() {
			const auto& e = elements[UIElement::Type::SYSOPT];
			return e && e->is_active();
		}();

		// 模态已关且视口排队已消费：拆掉残留，避免 TOGGLED SysOpt 占槽。
		// OK 当帧仍 deferred_pending=true，留给下一帧 check_events 先 apply。
		if (!sysopt_live && elements[UIElement::Type::SYSOPT]
			&& !ResolutionTiers::deferred_pending)
		{
			remove(UIElement::Type::SYSOPT);
		}

		bool update_screen = false;
		int16_t new_width = Constants::Constants::get().get_viewwidth();
		int16_t new_height = Constants::Constants::get().get_viewheight();

		if (!sysopt_live && (VWIDTH != new_width || VHEIGHT != new_height))
		{
			const int16_t old_width = VWIDTH;
			update_screen = true;
			VWIDTH = new_width;
			VHEIGHT = new_height;

			if (UiAssetResolve::has_game_hud())
			{
				// classic：仅当 QS 宽/窄档翻转才整窗重建（换 quickSlot08↔26 纹理）；
				// 同档切分辨率只走 update_screen 重锚，避开 initwindow 同帧的重型 ctor。
				// 与 UIStatusBar::CLASSIC_QS_SLOTS=26 同源（slots32=false）。
				const bool qs_flip = HudLayout::is_qs_wide(old_width, false)
					!= HudLayout::is_qs_wide(new_width, false);
				auto* sb = elements[UIElement::Type::STATUSBAR].get();
				const bool classic_reanchor = sb
					&& static_cast<UIStatusBar*>(sb)->is_classic()
					&& !qs_flip;

				if (!classic_reanchor)
				{
					UI::get().remove(UIElement::Type::STATUSBAR);

					const CharStats& stats = Stage::get().get_player().get_stats();
					emplace<UIStatusBar>(stats);

					// activate() 里先 STATUSBAR 后 CHATBAR，get_front 从 elementorder 尾部
					// 反向扫，所以聊天栏本应压住状态栏。视图尺寸一变就把 STATUSBAR 重新
					// emplace（push_back 到队尾），它反过来吞掉整条底栏 y>=649 的点击 ——
					// 真机实测「+」展开按钮点了没反应。重排回原生优先级。
					elementorder.remove(UIElement::Type::CHATBAR);
					elementorder.push_back(UIElement::Type::CHATBAR);
				}
			}
		}

		// 视口差分：广播给已构造元素（含 inactive）。跳过仍 active 的 SYSOPT。
		// update / locale 仍只跑 active。
		if (update_screen)
		{
			for (auto iter : elements)
			{
				if (!iter.second)
					continue;
				if (iter.first == UIElement::Type::SYSOPT)
					continue;
				iter.second->update_screen(new_width, new_height);
			}

			LOG(LOG_DEBUG, "[MR] UIStateGame broadcast update_screen "
				<< new_width << "x" << new_height);
		}

		for (auto& type : elementorder)
		{
			auto& element = elements[type];

			if (element && element->is_active())
			{
				// 切语言时已开着的窗要立刻换文案：文案代次只在 reload 时变，
				// 无绑定的窗体在这里是一次空 vector 判断，成本可忽略。
				element->refresh_localized();
				element->update();
			}
		}
	}

	bool UIStateGame::drop_icon(const Icon& icon, Point<int16_t> cursor_position)
	{
		if (UIElement* front = get_front(cursor_position))
			return front->send_icon(icon, cursor_position);
		else
			icon.drop_on_stage();

		return true;
	}

	void UIStateGame::remove_icon()
	{
		draggedicon->reset();
		draggedicon = {};
	}

	void UIStateGame::remove_cursors()
	{
		for (auto type : elementorder)
		{
			auto element = elements[type].get();

			if (element && element->is_active())
				element->remove_cursor();
		}
	}

	void UIStateGame::remove_cursor(UIElement::Type t)
	{
		for (auto type : elementorder)
		{
			auto element = elements[type].get();

			if (element && element->is_active() && element->get_type() != t)
				element->remove_cursor();
		}
	}

	void UIStateGame::doubleclick(Point<int16_t> pos)
	{
		// 对齐 MinMS UIStateGame::doubleclick：命中 UI 必转发后 return，
		// 绝不能落到「地图角色 → 个人情报」。
		// 本端 send_cursor 在 mousedown 即 drag_icon；双击序列的第二次按下
		// 会先置 draggedicon。若此处再 `if (!draggedicon)` 跳过，双击穿/卸
		// 永远不触发（2026-11-22 回归）。MinMS 无此门闩——取消未松手的
		// 拖拽（不 drop）再转发 doubleclick。
		if (UIElement* front = get_front(pos))
		{
			if (draggedicon)
				remove_icon();

			front->doubleclick(pos);
			return;
		}

		// 双击地图角色 → 个人情报（Angel / 正确截图：双击开 UserInfo）。
		Stage& stage = Stage::get();
		const Point<int16_t> view = stage.get_camera().position();
		int32_t cid = 0;
		std::string name;

		if (stage.get_chars().find_char_at(pos, view, cid, name))
		{
			CharInfoRequestPacket(cid).dispatch();
			return;
		}

		// 双击自己也开个人情报。
		Player& self = stage.get_player();
		Point<int16_t> absp = self.get_position() + view;
		Rectangle<int16_t> bounds(absp.x() - 36, absp.x() + 36, absp.y() - 110, absp.y());

		if (bounds.contains(pos))
			UI::get().emplace<UICharInfo>(self.get_oid());
	}

	void UIStateGame::rightclick(Point<int16_t> pos)
	{
		if (UIElement* front = get_front(pos))
		{
			front->rightclick(pos);
			return;
		}

		// Angel CUserLocal::HandleRButtonClk: 仅当光标命中玩家角色身体矩形才弹 ContextMenu
		// （sub_9726EA 遍历 CAvatar::GetBodyRect 做点-in-rect，未命中返回 0 → 不弹窗）。
		// 空地 / 怪物 / NPC 一律不弹；其他玩家走 MapChars::find_char_at，自己也按同尺寸身体框判定。
		Stage& stage = Stage::get();
		const Point<int16_t> view = stage.get_camera().position();
		int32_t cid = 0;
		std::string name;

		if (!stage.get_chars().find_char_at(pos, view, cid, name))
		{
			Player& self = stage.get_player();
			Point<int16_t> absp = self.get_position() + view;
			Rectangle<int16_t> bounds(absp.x() - 36, absp.x() + 36, absp.y() - 110, absp.y());
			if (!bounds.contains(pos))
				return;
			cid = self.get_oid();
			name = self.get_name();
		}

		auto cm = UI::get().get_element<UIContextMenu>();
		if (!cm)
			cm = UI::get().emplace<UIContextMenu>();
		if (cm)
			cm->open_at(pos, cid, name);
	}

	bool UIStateGame::has_active_focused()
	{
		heal_balloon_focus();

		if (auto element = get(focused))
			return element->is_active();

		return false;
	}

	void UIStateGame::heal_balloon_focus()
	{
		// 焦点自愈：目录/界面气球关闭后再次打开走 `makeactive()` 复用实例，
		// 不经过 pre_add，focused 标志丢失 —— 方向键随 KeyType::Id::ACTION
		// 绑定（Keyboard.cpp 把 DOWN 绑成玩家动作）穿透到 Stage::send_key，
		// 表现成「按上下键角色趴下」（用户实测复现）。发现焦点已空但有 active
		// 的气球时恢复指向，让 focused 分支接住按键。
		if (focused != UIElement::Type::NONE)
			return;

		if (auto gm = get(UIElement::Type::GAMEMENU); gm && gm->is_active())
			focused = UIElement::Type::GAMEMENU;
		else if (auto sc = get(UIElement::Type::SHORTCUTUI); sc && sc->is_active())
			focused = UIElement::Type::SHORTCUTUI;
	}

	void UIStateGame::send_key(KeyType::Id type, int32_t action, bool pressed, bool escape)
	{
		heal_balloon_focus();

		if (UIElement* focusedelement = get(focused))
		{
			if (focusedelement->is_active())
			{
				return focusedelement->send_key(action, pressed, escape);
			}
			// 焦点指向的窗体已经 inactive（最常见：鼠标停在窗体上点了 ✕，窗口关闭但
			// `focused` 还留着那个 Type）。旧实现在这里 `focused = NONE; return;`
			// —— 这一次按键既没交给窗体也没交给全局开关，被**静默吞掉**：
			// 关掉背包后按第一下 `i` 打不开、要按第二下，就是这条。
			// 正确做法是清掉脏焦点后**继续**往下走全局 dispatch，而不是丢弃按键。
			focused = UIElement::NONE;
		}

		// 脏焦点清掉之后，这一键**仍然要**交给下面的全局 dispatch。
		// 早先这里是 if/else：走进「焦点指向已关闭窗体」这一支就跳出，按键被吞 ——
		// 表现就是 ESC 要按第二下才开目录（第一下只把脏焦点清掉，相位差一整拍）。
		{
			// ESC 兜底：s9 的 KEYMAP 是从库里装载的，该账号没存 ESC 那一格（KeyConfig::Key::ESCAPE = 84）
			// 时下发的是 type=0/action=0 ⇒ mapping 为 NONE，下面那条 MAINMENU 分支永远进不来，
			// 这就是用户报的「esc 按键最后不能打开目录」。能走到这里说明没有任何活动窗体接住这一下，
			// 而原生在这一刻正是开「目录」。只在 classic 底栏下兜，避免改到 StatusBar3 的 ESC 语义。
			if (escape && pressed && type == KeyType::Id::NONE && UiAssetResolve::is_classic_hud())
			{
				type = KeyType::Id::MENU;
				action = KeyAction::Id::MAINMENU;
			}

			switch (type)
			{
				case KeyType::Id::MENU:
				{
					if (pressed)
					{
						switch (action)
						{
							// ===== §f-keymap 遗留五死键接线（2026-11-06）=====
							// 本枚举 21+ 段沿用新版编号，v83 的 11/17/25/26 透传后落在
							// WHISPER/GUILD/MANAGELEGION(MEDALS槽)/MEDALS(FAMILY槽)，
							// 原先无 case ⇒ 键发了、没反应。窗口本体全部已存在。
							case KeyAction::Id::WHISPER:
							{
								// v83 H(11) = WHISPER：原生「请输入悄悄话的对象」弹层；
								// 其确认回调 = ChatBar::set_whisper_target（切密语频道+预置 /w）。
								// 与底栏密语钮、下拉选「悄悄话」同一条路（pick_classic_target）。
								if (auto dlg = UI::get().get_element<UIWhisperTargetDlg>())
								{
									dlg->makeactive();
									dlg->open();
								}
								else if (auto new_dlg = UI::get().emplace<UIWhisperTargetDlg>())
								{
									new_dlg->open();
								}
								break;
							}
							case KeyAction::Id::MANAGELEGION:
							{
								// v83 O(25) = PARTYSEARCH 组队搜索窗（枚举名沿用新版军团管理）。
								if (UiAssetResolve::usable(UiAssetResolve::window("PartySearch")))
									emplace<UIPartySearch>();
								break;
							}
							case KeyAction::Id::MEDALS:
							{
								// v83 F(33 键) action 26 = FAMILY 学院（家族）窗。
								// KeyAction.h:55 `MEDALS // FAMILY (v83)` 为编号错位实证。
								// 开窗前照刷新钮语义请求一次（s9 回 95 FAMILY_INFO_RESULT
								// 回灌活动窗；登录数据层已有，双保险）。
								OpenFamilyPacket().dispatch();
								auto fam = UI::get().get_element<UIFamily>();
								if (!fam)
									fam = UI::get().emplace<UIFamily>();
								else if (!fam->is_active())
									fam->makeactive();
								break;
							}
							case KeyAction::Id::NOTIFIER:
							{
								// v83 L(20) = NOTIFIER 通知窗。查证：s9 SendOpcode 无任何 NOTIFIER/
								// SERVERNOTICE 窗口化下发（18 号在 s9 未使用），原生通知窗内容源
								// = notifier 列表封包 —— 服务端不存在 ⇒ 按门禁 7 不做假窗。
								// 通知文案实际全走 serverNotice/chat（已接）。保留灭活并留证。
								LOG(LOG_DEBUG, "UIStateGame: NOTIFIER key (v83 L) — no s9 notifier backend, intentionally inert");
								break;
							}
							case KeyAction::Id::EQUIPMENT:
							{
								// 只开主装备窗；辅助装备由 UIEquipInventory::sync_side_panels
								// 贴左上并同开同关（StatusBar 装备钮同路径）。
								if (UiAssetResolve::usable(UiAssetResolve::equip_panel_root()))
									emplace<UIEquipInventory>(
										Stage::get().get_player().get_inventory()
										);

								break;
							}
							case KeyAction::Id::ITEMS:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("Item")))
									emplace<UIItemInventory>(
										Stage::get().get_player().get_inventory()
										);

								break;
							}
							case KeyAction::Id::STATS:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window4("Stat")))
									emplace<UIStatsInfo>(
										Stage::get().get_player().get_stats()
										);

								break;
							}
							case KeyAction::Id::SKILLS:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("Skill")))
									emplace<UISkillBook>(
										Stage::get().get_player().get_stats(),
										Stage::get().get_player().get_skills()
										);

								break;
							}
							case KeyAction::Id::FRIENDS:
							case KeyAction::Id::PARTY:
							case KeyAction::Id::BOSSPARTY:
							case KeyAction::Id::GUILD:
							{
								// v83 G(17) = GUILD「家族」：用户 2026-11-07 指正，原生是
								// 「好友/组员/家族/家族联盟/拒绝列表」共用窗的第 3 页签，
								// 不是独立 UIGuild 浮窗（浮窗仍由底栏社区钮保留入口）。
								UIUserList::Tab tab;

								switch (action)
								{
									case KeyAction::Id::FRIENDS:
										tab = UIUserList::Tab::FRIEND;
										break;
									case KeyAction::Id::PARTY:
										tab = UIUserList::Tab::PARTY;
										break;
									case KeyAction::Id::BOSSPARTY:
										// P0：KeyConfig 已不再提供 BOSSPARTY 槽；残留绑定兜底进家族页
										//（v83 action27 经 from_server 已映射为 MEDAL，不走此分支）。
									case KeyAction::Id::GUILD:
										tab = UIUserList::Tab::GUILD;
										break;
								}

								auto userlist = UI::get().get_element<UIUserList>();

								if (userlist && userlist->get_tab() != tab && userlist->is_active())
								{
									userlist->change_tab(tab);
								}
								else
								{
									if (UiAssetResolve::usable(UiAssetResolve::window("UserList")))
										emplace<UIUserList>(tab);

									if (userlist && userlist->get_tab() != tab && userlist->is_active())
										userlist->change_tab(tab);
								}

								break;
							}
							case KeyAction::Id::WORLDMAP:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("WorldMap")))
									emplace<UIWorldMap>();
								break;
							}
							case KeyAction::Id::MAPLECHAT:
							{
								// Wave A：优先 Messenger；无资源时回退 socialChatEnter/UIChat
								if (UiAssetResolve::usable(UiAssetResolve::window("Messenger")))
								{
									emplace<UIMessenger>();

									if (auto m = UI::get().get_element<UIMessenger>())
										m->try_create_room();

									break;
								}

								if (!UiAssetResolve::usable(UiAssetResolve::window("socialChatEnter")))
									break;

								auto chat = UI::get().get_element<UIChat>();

								if (!chat || !chat->is_active())
									emplace<UIChat>();

								break;
							}
							case KeyAction::Id::MINIMAP:
							{
								if (auto minimap = UI::get().get_element<UIMiniMap>())
									minimap->send_key(action, pressed, escape);
								else
									// M 关过窗体后对象可能随换图销毁：重建即回到 MIN 态。
									emplace<UIMiniMap>(Stage::get().get_player().get_stats());

								break;
							}
							case KeyAction::Id::QUESTLOG:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("Quest")))
									emplace<UIQuestLog>(
										Stage::get().get_player().get_quests()
										);

								break;
							}
							case KeyAction::Id::KEYBINDINGS:
							{
								nl::node keycfg = nl::nx::UI["StatusBar3.img"]["KeyConfig"];

								if (!UiAssetResolve::usable(keycfg))
									keycfg = UiAssetResolve::window("KeyConfig");

								if (!UiAssetResolve::usable(keycfg))
									break;

								auto keyconfig = UI::get().get_element<UIKeyConfig>();

								if (!keyconfig || !keyconfig->is_active())
								{
									emplace<UIKeyConfig>(
										Stage::get().get_player().get_inventory(),
										Stage::get().get_player().get_skills()
										);
								}
								else if (keyconfig && keyconfig->is_active())
								{
									keyconfig->close();
								}

								break;
							}
							case KeyAction::Id::SAY:
							case KeyAction::Id::PARTYCHAT:
							case KeyAction::Id::FRIENDSCHAT:
							case KeyAction::Id::GUILDCHAT:
							case KeyAction::Id::ALLIANCECHAT:
							case KeyAction::Id::TOSPOUSE:
							{
								if (auto chatbar = UI::get().get_element<UIChatBar>())
									chatbar->change_target(action);

								break;
							}
							case KeyAction::Id::MENU:
							{
								if (auto statusbar = UI::get().get_element<UIStatusBar>())
									statusbar->toggle_menu();

								break;
							}
							case KeyAction::Id::QUICKSLOTS:
							{
								if (auto statusbar = UI::get().get_element<UIStatusBar>())
									statusbar->toggle_qs();

								break;
							}
							case KeyAction::Id::TOGGLECHAT:
							{
								if (auto chatbar = UI::get().get_element<UIChatBar>())
								{
									if (chatbar->has_input())
										chatbar->send_key(action, pressed, escape);
									else if (chatbar->is_classic())
										// classic HUD 没有 BtMax/BtMin，Enter 就是"开输入行"；
										// 原来这里调 toggle_view() 只翻历史框，收框后再也开不回来。
										chatbar->toggle_input(true);
									else
										chatbar->toggle_view();
								}

								break;
							}
							case KeyAction::Id::CASHSHOP:
							{
								EnterCashShopPacket().dispatch();
								break;
							}
							case KeyAction::Id::MONSTERCOLLECTION:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("MonsterBook")))
									emplace<UIMonsterBook>();
								break;
							}
							case KeyAction::Id::PROFESSION:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("Maker")))
									emplace<UIMaker>();
								break;
							}
							case KeyAction::Id::EVENT:
							{
								// 本端扩展（E 桶）：福利中心（签到/兑换/排行/转职/传送）非 083 原生窗族 —— 083 活 IDB 全库 `*UISpecial*` / `*Fairy*` / `*Welfare*` / `*CheckIn*` / `*Attendance*` 符号命中均为 0（`tools/_ida_r93/special_family.json`），故按 m09729「扩展功能可以不对齐」保留且不改行为；暂时复用 EVENT 键打开（后续若补独立键位须同步服务端 keymap）。旧注释写「095 自定义中文 UI」属依据链污染，round93 按 083 零命中改判本端扩展身份。
								emplace<UISpecial>();
								break;
							}
							case KeyAction::Id::CHARINFO:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("UserInfo")))
									emplace<UICharInfo>(
										Stage::get().get_player().get_oid()
										);

								break;
							}
							case KeyAction::Id::CHANGECHANNEL:
							{
								if (UiAssetResolve::usable(UiAssetResolve::window("Channel")))
									emplace<UIChannel>();
								break;
							}
							case KeyAction::Id::MAINMENU:
							{
								// ESC → 「目录」。旧实现只把 MAINMENU 转给 UIStatusBar::send_key，
								// 而那个 send_key 里没有 MAINMENU 分支（它只认 escape/RETURN/上下），
								// 所以 classic 底栏下 ESC 是条死线：键表绑了、动作发了、什么都没发生。
								// 底栏「目录」钮走的是 UIGameMenu 开/关，这里复用同一条路。
								if (UiAssetResolve::is_classic_hud())
								{
									// 互斥：开主菜单时先收起界面列表（UIShortCut）。
									if (auto sc = UI::get().get_element<UIShortCut>(); sc && sc->is_active())
										sc->deactivate();

									auto gm = UI::get().get_element<UIGameMenu>();

									if (!gm)
										gm = UI::get().emplace<UIGameMenu>();
									else if (!gm->is_active())
									{
										gm->reset_selection();
										gm->makeactive();
									}
									else
										gm->deactivate();

									break;
								}

								if (auto statusbar = UI::get().get_element<UIStatusBar>())
									statusbar->send_key(action, pressed, escape);

								break;
							}
							case KeyAction::Id::SCREENSHOT:
							{
								// 「截图保存位置」的触发键（B4-7）。真正的回读/落盘在
								// Window::end() 里做，这里只挂请求 —— 本帧还没提交。
								if (pressed)
									Window::get().request_screenshot();

								break;
							}
							case KeyAction::Id::MEDAL:
							{
								// D1 / 分号键「勋章」窗：v83 action 27 -> from_server -> MEDAL(28)。
								// D1 / 分号键「勋章」窗（v83 action 27 -> from_server -> MEDAL(28)）。
								// 原生 = CUIMedalQuestInfo（ctor @0x840170，260x374）：
								// 数据源 QuestInfo.img viewMedalItem × QuestLog 完成态。
								// 窗体框架走门禁 18 复用件（Title 件 SAD 探针否定，见 UIMedal.h）。
								if (pressed)
									emplace<UIMedal>();
								break;
							}

							default:
							{
								LOG(LOG_DEBUG, "Unknown KeyAction::Id action: [" << action << "]");
								break;
							}
						}
					}

					break;
				}
				case KeyType::Id::ACTION:
				case KeyType::Id::FACE:
				case KeyType::Id::ITEM:
				case KeyType::Id::SKILL:
				case KeyType::Id::MACRO:
				{
					Stage::get().send_key(type, action, pressed);
					break;
				}
			}
		}
	}

	Cursor::State UIStateGame::send_cursor(Point<int16_t> cursor_position, Cursor::State cursor_state)
	{
		if (draggedicon)
		{
			// 2026-11-22 按方案 A 彻底对齐原生（Angel CDraggableSkill::OnDropped
			// @0x4faa22 / CUIStatusBar::GetShortCutIndexByPos @0x8de8d5 的 DragDrop
			// 模型 + MinMS UIStateGame::send_cursor:505-523）：
			// 单一拖拽模型——按住拖动（CLICKING/其它帧一律 GRABBING，不结算），
			// 松手（IDLE）帧落点并清除。**删除本端自研的 macro_sticky 点击黏住态**
			// （三家原生都没有，是本端历史补丁，反复导致技能宏/快捷栏拖拽回归）。
			if (cursor_state == Cursor::State::IDLE)
			{
				drop_icon(*draggedicon, cursor_position);
				remove_icon();

				return Cursor::State::IDLE;
			}

			return Cursor::State::GRABBING;
		}
		else
		{
			bool clicked = cursor_state == Cursor::State::CLICKING || cursor_state == Cursor::State::VSCROLLIDLE || (dragged && (cursor_state == Cursor::State::CHATBARMOVE || cursor_state == Cursor::State::CHATBARVDRAG || cursor_state == Cursor::State::CHATBARHDRAG || cursor_state == Cursor::State::CHATBARBLTRDRAG || cursor_state == Cursor::State::CHATBARBRTLDRAG));

			// 焦点窗独占点击的例外：轻量气球（目录/界面列表，closes_on_outside_click）
			// 在光标落到自身范围外时收起自己，并把这次点击交还下面的普通派发 ——
			// 否则状态栏「目录/界面」钮在气球打开期间永远收不到点击，
			// 两个气球也就无法互相切换（用户实测「点目录没反应」的根因）。
			UIElement* focusedelement = get(focused);

			if (focusedelement && focusedelement->is_active()
				&& !focusedelement->is_in_range(cursor_position)
				&& focusedelement->closes_on_outside_click())
			{
				focused = UIElement::Type::NONE;
				focusedelement->deactivate();
				focusedelement = nullptr;
			}

			if (focusedelement)
			{
				if (focusedelement->is_active())
				{
					// 与下方普通派发同口径：tooltip 每帧派发入口先清，on-entry 同帧重挂。
					if (tooltipparent != Tooltip::Parent::NONE)
						clear_tooltip(tooltipparent);

					remove_cursor(focusedelement->get_type());

					return focusedelement->send_cursor(clicked, cursor_position);
				}
				else
				{
					focused = UIElement::Type::NONE;

					return cursor_state;
				}
			}
			else
			{
				if (!clicked)
				{
					dragged = nullptr;

					if (auto front = get_front(cursor_position))
					{
						UIElement::Type front_type = front->get_type();

						// 2026-11-16 修复：tooltip 移出即消失。旧逻辑只在悬停到「另一个元素」时清
						// tooltip——光标落回空白处（get_front 为空）或仍在本窗但离开条目区时，
						// tooltip 永远挂着跟鼠标跑。现在每帧派发入口先清，on-slot 的
						// send_cursor 同帧内重新 show_item/show_equip，无闪烁；equip/item/
						// skill/map/text 全部 tooltip 父类型统一生效（共用修复）。
						if (tooltipparent != Tooltip::Parent::NONE)
							clear_tooltip(tooltipparent);

						remove_cursor(front_type);

						return front->send_cursor(clicked, cursor_position);
					}
					else
					{
						remove_cursors();

						return Stage::get().send_cursor(clicked, cursor_position);
					}
				}
				else
				{
					if (!dragged)
					{
						UIElement::Type drag_element_type = UIElement::Type::NONE;

						for (auto iter = elementorder.rbegin(); iter != elementorder.rend(); ++iter)
						{
							auto& element = elements[*iter];

							if (element && element->is_active() && element->is_in_range(cursor_position))
							{
								dragged = element.get();
								drag_element_type = *iter;
								break;
							}
						}

						if (drag_element_type != UIElement::Type::NONE)
						{
							elementorder.remove(drag_element_type);
							elementorder.push_back(drag_element_type);
						}
					}

					if (dragged)
						return dragged->send_cursor(clicked, cursor_position);
					else
						return Stage::get().send_cursor(clicked, cursor_position);
				}
			}
		}
	}

	void UIStateGame::send_scroll(double yoffset)
	{
		for (auto& type : elementorder)
		{
			auto& element = elements[type];

			if (element && element->is_active())
				element->send_scroll(yoffset);
		}
	}

	void UIStateGame::send_close()
	{
		// UIQuit 依赖 UIWindow6.img/askReward（BeiDou 素材缺失）：
		// 缺失时只画黑色遮罩、没有退出提示（用户实测反馈）。退回退出确认弹窗。
		// 用统一 gate 直接查 UIWindow6.img —— 旧的 window("UIWindow6") 只查
		// UIWindow2/img，永远取不到，导致这条判断恒为 false。
		if (UiAssetResolve::has_quit_reward_dialog())
		{
			UI::get().emplace<UIQuit>(stats);
		}
		else
		{
			UI::get().emplace<UIQuitConfirm>();
		}
	}

	void UIStateGame::drag_icon(Icon* drgic)
	{
		draggedicon = drgic;
		// 方案 A：不再区分宏源/普通源，统一 DragDrop 按住拖松手落。
		(void)drgic;
	}

	void UIStateGame::clear_tooltip(Tooltip::Parent parent)
	{
		if (parent == tooltipparent)
		{
			eqtooltip.set_equip(Tooltip::Parent::NONE, 0);
			ittooltip.set_item(0);
			tetooltip.set_text("");
			matooltip.reset();
			tooltip = {};
			tooltipparent = Tooltip::Parent::NONE;
		}
	}

	void UIStateGame::show_equip(Tooltip::Parent parent, int16_t slot)
	{
		eqtooltip.set_equip(parent, slot);

		if (slot)
		{
			tooltip = eqtooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_item(Tooltip::Parent parent, int32_t itemid)
	{
		// 2026-11-20 用户实拍「卷轴显示成装备 tooltip」根因：
		// 旧判定 `EquipData::get(itemid)` 走 Cache::get → 任何有效道具都会构造出
		// EquipData，且 is_valid()==itemdata.is_valid() 对卷轴/消耗品同样为 true，
		// 于是 2041043（披风魔法防御卷轴）被误当装备画成需求/属性/对比。
		// 原生只有装备号段（100..194 + 254 cash 特殊）才有 EquipTooltip；
		// 消耗(2xx)/安装(3xx)/etc(4xx)/cash 普通(5xx)一律走 ItemTooltip
		// （名称居中 + 图标 + 说明），这才是原生卷轴 tip 的样子。
		if (itemid && itemid / 1000000 == 1)
		{
			eqtooltip.set_equip_itemid(itemid);
			tooltip = eqtooltip;
			tooltipparent = parent;
			return;
		}

		ittooltip.set_item(itemid);

		if (itemid)
		{
			tooltip = ittooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_skill(Tooltip::Parent parent, int32_t skill_id, int32_t level, int32_t masterlevel, int64_t expiration)
	{
		sktooltip.set_skill(skill_id, level, masterlevel, expiration);

		if (skill_id)
		{
			tooltip = sktooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_text(Tooltip::Parent parent, std::string text)
	{
		tetooltip.set_text(text);

		if (!text.empty())
		{
			tooltip = tetooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_map(Tooltip::Parent parent, std::string title, std::string description, int32_t mapid, bool bolded, bool portal,
		const std::vector<MapTooltipPlayer>& players)
	{
		matooltip.set_title(parent, title, bolded);
		matooltip.set_desc(description);
		matooltip.set_mapid(mapid, portal);
		matooltip.set_players(players);

		if (!title.empty())
		{
			tooltip = matooltip;
			tooltipparent = parent;
		}
	}

	template <class T, typename...Args>
	void UIStateGame::emplace(Args&& ...args)
	{
		if (auto iter = pre_add(T::TYPE, T::TOGGLED, T::FOCUSED))
		{
			(*iter).second = std::make_unique<T>(
				std::forward<Args>(args)...
				);

			auto silent_types = {
				UIElement::Type::STATUSMESSENGER,
				UIElement::Type::STATUSBAR,
				UIElement::Type::CHATBAR,
				UIElement::Type::MINIMAP,
				UIElement::Type::BUFFLIST,
				UIElement::Type::ENERGYBAR,
				UIElement::Type::COMBOCOUNTER,
				UIElement::Type::BOSSHP,
				UIElement::Type::CLOCK,
				UIElement::Type::NPCTALK,
				UIElement::Type::SHOP
			};

			if (std::find(silent_types.begin(), silent_types.end(), T::TYPE) == silent_types.end())
			{
				if (T::TYPE == UIElement::Type::WORLDMAP)
					Sound(Sound::Name::WORLDMAPOPEN).play();
				else
					Sound(Sound::Name::MENUUP).play();

				UI::get().send_cursor(false);
			}
		}
	}

	UIState::Iterator UIStateGame::pre_add(UIElement::Type type, bool is_toggled, bool is_focused)
	{
		auto& element = elements[type];

		if (element && is_toggled)
		{
			elementorder.remove(type);
			elementorder.push_back(type);

			bool active = element->is_active();

			element->toggle_active();

			if (active != element->is_active())
			{
				if (element->is_active())
				{
					if (type == UIElement::Type::WORLDMAP)
						Sound(Sound::Name::WORLDMAPOPEN).play();
					else
						Sound(Sound::Name::MENUUP).play();

					UI::get().send_cursor(false);
				}
				else
				{
					if (type == UIElement::Type::WORLDMAP)
						Sound(Sound::Name::WORLDMAPCLOSE).play();
					else
						Sound(Sound::Name::MENUDOWN).play();

					element->remove_cursor();

					if (draggedicon)
					{
						if (element->get_type() == icon_map[draggedicon.get()->get_type()]
							|| element->get_type() == UIElement::Type::SKILLMACROEX)
						{
							// 方案 A：关闭来源窗（含宏面板）时收掉光标上的拖拽副本，
							// 防止残留幽灵图标；宏图标源格写回由 remove_cursor() 负责。
							remove_icon();
						}
					}

					UI::get().send_cursor(false);
				}
			}

			return elements.end();
		}
		else
		{
			remove(type);
			elementorder.push_back(type);

			if (is_focused)
				focused = type;

			return elements.find(type);
		}
	}

	void UIStateGame::apply_hud_theme()
	{
		if (!UiAssetResolve::has_game_hud())
		{
			LOG(LOG_WARN, "UIStateGame::apply_hud_theme: no StatusBar pack available");
			return;
		}

		// Close classic/modern menus that would dangle after StatusBar rebuild.
		if (auto* sb = dynamic_cast<UIStatusBar*>(elements[UIElement::Type::STATUSBAR].get()))
		{
			if (sb->is_menu_active())
				sb->remove_menus();
		}

		UI::get().remove(UIElement::Type::STATUSBAR);
		UI::get().remove(UIElement::Type::CHATBAR);

		emplace<UIStatusBar>(stats);
		emplace<UIChatBar>();

		// Keep chat above statusbar for hit-testing (same as resolution rebuild path).
		elementorder.remove(UIElement::Type::CHATBAR);
		elementorder.push_back(UIElement::Type::CHATBAR);

		LOG(LOG_INFO, "UIStateGame::apply_hud_theme applied="
			<< UiTheme::to_config(UiTheme::resolve())
			<< " classic=" << (UiTheme::is_classic_layout() ? 1 : 0));
	}

	void UIStateGame::remove(UIElement::Type type)
	{
		if (type == focused)
			focused = UIElement::Type::NONE;

		if (type == tooltipparent)
			clear_tooltip(tooltipparent);

		elementorder.remove(type);

		if (auto& element = elements[type])
		{
			element->deactivate();
			// 必须 reset 析构：release() 会泄漏旧窗实例；NpcTalk 等 FOCUSED 窗
			// 每次 ScriptMessage emplace 时若只 release，叠层残影/幽灵按钮会挂在屏上。
			element.reset();
		}
	}

	void UIStateGame::bring_to_front(UIElement::Type type)
	{
		// elementorder 尾 = draw() 最后画 = 最上层。已在队尾时直接返回：既省一次重排，
		// 也避免在按引用遍历 elementorder 的回调里把“当前节点”摘掉（list::remove 会失效该迭代器）。
		if (elementorder.empty() || elementorder.back() == type)
			return;

		// 不在层级表里（尚未 emplace / 已 remove）时不能 push_back，否则会留下指向空元素的幽灵条目。
		if (std::find(elementorder.begin(), elementorder.end(), type) == elementorder.end())
			return;

		elementorder.remove(type);
		elementorder.push_back(type);
	}

	UIElement* UIStateGame::get(UIElement::Type type)
	{
		return elements[type].get();
	}

	UIElement* UIStateGame::get_front(std::list<UIElement::Type> types)
	{
		auto begin = elementorder.rbegin();
		auto end = elementorder.rend();

		for (auto& iter = begin; iter != end; ++iter)
		{
			if (std::find(types.begin(), types.end(), *iter) != types.end())
			{
				auto& element = elements[*iter];

				if (element && element->is_active())
					return element.get();
			}
		}

		return nullptr;
	}

	UIElement* UIStateGame::get_front(Point<int16_t> cursor_position)
	{
		// modern 底栏六钮 / quickSlot：CHATBAR 宽命中区常盖住右侧菜单，须先让 STATUSBAR 吃点。
		if (auto* sb = dynamic_cast<UIStatusBar*>(elements[UIElement::Type::STATUSBAR].get()))
		{
			if (sb->is_active() && sb->cursor_on_interactive(cursor_position))
				return sb;
		}

		// 聊天栏输入行锁定（2026-11-15 用户真机反馈）：点快捷键展开/收缩钮等任意
		// 路径把 STATUSBAR 提升到 elementorder 尾部后，get_front 反向扫描先命中
		// 状态栏（其命中区 = 整条底栏 dimension (VWIDTH,80)），聊天栏的「+」/
		// chatTarget/输入行就再也收不到点击 —— 「聊天区加号点不开」。
		// 绘制侧早已锁死 CHATBAR 紧贴 STATUSBAR 之后（draw() 同批），命中侧在此
		// 对齐同一优先级：光标落在聊天栏命中区内且聊天栏 active 时，聊天栏必胜。
		if (auto& chat = elements[UIElement::Type::CHATBAR])
		{
			if (chat->is_active() && chat->is_in_range(cursor_position))
				return chat.get();
		}

		auto begin = elementorder.rbegin();
		auto end = elementorder.rend();

		for (auto& iter = begin; iter != end; ++iter)
		{
			auto& element = elements[*iter];

			if (element && element->is_active() && element->is_in_range(cursor_position))
				return element.get();
		}

		return nullptr;
	}
}

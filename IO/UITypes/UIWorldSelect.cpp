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
////////////////////////////////////////////////////////////////////////////////
#include "UIWorldSelect.h"

#include "UILogin.h"
#include "UILoginNotice.h"
#include "UINotice.h"
#include "UISoftKey.h"

#include "../../Util/Locale.h"
#include "UILoginWait.h"
#include "UIRegion.h"

#include "../UI.h"

#include "../LoginBackground.h"

#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"

#include "../../Graphics/Texture.h"
#include "../../Audio/Audio.h"
#include "../../Util/Randomizer.h"
#include "../../Constants.h"

#include "../../Net/Packets/LoginPackets.h"
#include "../../Net/Packets/SelectCharPackets.h"
#include "../../Net/Session.h"

#include "../../Graphics/LocalPngTexture.h"
#include "../../Graphics/Text.h"

// 卷轴展开动画用 Range<int16_t> 做底板源裁剪。
#include "../../Template/Range.h"

#include <algorithm>
#include <iostream>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 版式依据：gms083 IDA ?DrawWorldItems@CUIWorldSelect@@QAEXXZ (0x624e13)。
	// 世界列表为横向一行条幅：最多 30 槽，x 步进 27px；按钮节点为
	// UI/Login.img/WorldSelect/BtWorld/[worldId]，未开放的大区用 BtWorld/e 占位，
	// 世界名图为 WorldSelect/world/[worldId]，大区提示图标为 Login.img/WorldNotice/[worldId]。
	static constexpr int16_t WS_SLOT_STEP_X = 27;
	static constexpr int16_t WS_SLOT_COUNT = 30;

	// 世界按钮槽上限直接取自 UIWorldSelect::WORLD_BUTTON_COUNT
	// （Buttons 枚举的 BtChannel0 / BtGoWorld 均由它推导），不再单独写死数字，避免两处脱钩。
	static constexpr uint16_t WS_MAX_WORLDS = UIWorldSelect::WORLD_BUTTON_COUNT;

	// 越界告警限频标记：每类、每 id 在进程内只告警一次。
	// 世界数与 id 均由服务端下发，异常或恶意服务端可能持续触发，故不可无条件打印。
	static bool WS_BOUNDS_WARNED_WORLD[256] = { false };
	static bool WS_BOUNDS_WARNED_CHANNEL[256] = { false };

	/// <summary>
	/// 记录一次「按钮索引越界」告警（限频：同类同 id 只报一次）。
	/// 定级 WARN：属「服务端下发数据超出客户端容量」的可审计异常事件，
	/// 需在正式环境保留，便于定位服务端配置与客户端版本不匹配的问题。
	/// </summary>
	static void warn_button_index_out_of_range(bool* warned, uint16_t index, const char* kind, uint16_t limit)
	{
		if (index < 256)
		{
			if (warned[index])
				return;

			warned[index] = true;
		}

		// Log text stays ASCII for console readability; Chinese notes live in comments.
		std::cerr << "[UIWorldSelect][WARN] " << kind << " index=" << index
			<< " exceeds client button capacity " << limit << "; operation ignored. "
			<< "(server data does not match client capacity)" << std::endl;
	}

	/// <summary>
	/// 记录一次「服务端下发的频道数超出客户端容量」告警（进程内只报一次）。
	/// 与「索引越界」区分开，语义更明确：整段多余频道都会被忽略。
	/// </summary>
	static void warn_channel_count_out_of_range(uint16_t count)
	{
		static bool warned = false;

		if (warned)
			return;

		warned = true;

		std::cerr << "[UIWorldSelect][WARN] server channel count=" << count
			<< " exceeds client capacity " << UIWorldSelect::CHANNEL_BUTTON_COUNT
			<< "; extra channels ignored (only the first "
			<< UIWorldSelect::CHANNEL_BUTTON_COUNT << " take effect)." << std::endl;
	}

	// 频道面板 chBackgrn 尺寸 449x233（原点在中心），据此算左上角。
	static constexpr int16_t CH_PANEL_W = 449;
	static constexpr int16_t CH_PANEL_H = 233;

	// MapLogin WorldSelect/signboard/0 横杆；木牌挂在其底边绳扣下。
	static constexpr Point<int16_t> WS_POLE_MAP_POS(51, -888);
	static constexpr int16_t WS_BOARD_HANG_PAD = 4;

	// MapLogin WorldSelect/signboard/2 羊皮卷；频道面板顶边在其底边之下。
	static constexpr Point<int16_t> WS_SCROLL_MAP_POS(42, -753);
	static constexpr int16_t CH_BELOW_GAP = 8;

	// LoginBackground 不可用时的兜底（退化路径，按 1280x720 标定值）。
	static constexpr int16_t WS_BANNER_FALLBACK_Y = 111;
	static constexpr int16_t CH_PANEL_TOP_GAP = 34;

	// 选中后频道面板滑入。BeiDou 地图已有 signboard 卷轴，滑距过大易错位；缩短。
	static constexpr int16_t CH_SLIDE_DIST = 24;
	static constexpr float CH_SLIDE_MS = 180.0f;

	// 参考端选中世界后频道板为「卷轴展开」：自上而下纵向揭示，约 220ms。
	static constexpr float CH_EXPAND_MS = 220.0f;

	// 选中世界后 world/[id]（图标+名）相对 chBackgrn 中心：再往左上贴表头。
	static constexpr Point<int16_t> WS_WORLD_NAME_ON_PANEL(-222, -114);

	// BtGoWorld 140x41：相对面板中心，落在羊皮纸内右下（图2）。
	static constexpr int16_t WS_GOWORLD_W = 140;
	static constexpr int16_t WS_GOWORLD_H = 41;
	static constexpr Point<int16_t> WS_GOWORLD_OFF(
		CH_PANEL_W / 2 - WS_GOWORLD_W - 16,
		CH_PANEL_H / 2 - WS_GOWORLD_H - 12
	);

	// 左侧侧栏对照原生截图（1280×720 实测，正确端截图
	// docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/选择频道页.png）：
	//   step/1（150×57，暗红带在精灵内 y=3..56）绘于 (0,33) → 可见带 y=36..85、x=0..145；
	//   BtViewChoice「查看世界地图」（99×35）(0,127)；BtViewAll「查看所有角色」（99×35）(0,377)；
	//   BtStart「回到首页」（98×34）(0,427)。第一钮与第二钮之间是大段留白（参考端如此），
	//   结束游戏独立左上角 (8,8)。侧栏贴屏幕左缘（x=0）。
	static constexpr int16_t WS_SIDEBAR_X = 0;
	static constexpr int16_t WS_SIDEBAR_STEP_Y = 33;	// step「选择频道」
	static constexpr int16_t WS_SIDEBAR_Y0 = 127;		// BtViewChoice
	static constexpr int16_t WS_SIDEBAR_Y1 = 377;		// BtViewAll
	static constexpr int16_t WS_SIDEBAR_Y2 = 427;		// BtStart
	static constexpr Point<int16_t> WS_EXIT_POS(8, 8);

	// 公告底板相对 chBackgrn 中心：放在面板左侧。
	static constexpr int16_t WS_NOTICE_GAP = 12;

	// 频道按钮/负载条在频道面板内的网格偏移。
	static Point<int16_t> channel_cell_offset(uint16_t i)
	{
		constexpr int16_t COLS = 4;
		constexpr int16_t CELL_W = 100;
		constexpr int16_t CELL_H = 34;

		div_t d = std::div(static_cast<int>(i), static_cast<int>(COLS));

		return Point<int16_t>(22 + CELL_W * d.rem, 56 + CELL_H * d.quot);
	}

	UIWorldSelect::UIWorldSelect() : UIElement(
			Point<int16_t>(0, 0),
			Point<int16_t>(
				Constants::Constants::get().get_viewwidth(),
				Constants::Constants::get().get_viewheight()
			)
		), worldcount(0), world_selected(false), channel_slide(1.0f), notice_slide(1.0f),
		scroll_playing(false), scroll_has_anim(false), world_status(0), status_notified_world(-1)
	{
		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS, "Ver. " + version_text);
		version_pos = nl::nx::UI["Login.img"]["Common"]["version"]["pos"];

		int16_t vieww = Constants::Constants::get().get_viewwidth();
		int16_t viewh = Constants::Constants::get().get_viewheight();

		// 背景由 LoginBackground 统一绘制（地图 + 外框），这里走 step=1 的大区选择视角。
		bool bg_managed = LoginBackground::get().available();

		if (bg_managed)
			LoginBackground::get().set_step(1);

		Point<int16_t> viewpos = bg_managed
			? LoginBackground::get().get_target_viewpos()
			: Point<int16_t>(vieww / 2, viewh / 2);

		int16_t slotline_w = static_cast<int16_t>(WS_SLOT_STEP_X * WS_MAX_WORLDS);

		if (bg_managed)
		{
			auto sane_origin = [](Point<int16_t> o) -> Point<int16_t>
			{
				if (o.x() < -2048 || o.x() > 2048 || o.y() < -2048 || o.y() > 2048)
					return Point<int16_t>(0, 0);
				return o;
			};

			// 木牌对准横杆 signboard/0：水平以横杆中心居中，竖直挂在底边绳扣下。
			Texture pole_tex(nl::nx::Map["Obj"]["login.img"]["WorldSelect"]["signboard"]["0"]["0"]);
			Point<int16_t> pole_origin = sane_origin(pole_tex.get_origin());
			Point<int16_t> pole_dim = pole_tex.get_dimensions();
			Point<int16_t> pole_topleft = WS_POLE_MAP_POS + viewpos - pole_origin;
			int16_t pole_center_x = static_cast<int16_t>(pole_topleft.x() + pole_dim.x() / 2);
			int16_t pole_bottom_y = static_cast<int16_t>(pole_topleft.y() + pole_dim.y());
			worldsrc_pos = Point<int16_t>(
				static_cast<int16_t>(pole_center_x - slotline_w / 2),
				static_cast<int16_t>(pole_bottom_y + WS_BOARD_HANG_PAD)
			);

			// 频道面板顶边在羊皮卷 signboard/2 底边之下（不再以卷心为面板中心）。
			Texture scroll_tex(nl::nx::Map["Obj"]["login.img"]["WorldSelect"]["signboard"]["2"]["0"]);
			Point<int16_t> scroll_origin = sane_origin(scroll_tex.get_origin());
			Point<int16_t> scroll_dim = scroll_tex.get_dimensions();
			Point<int16_t> scroll_topleft = WS_SCROLL_MAP_POS + viewpos - scroll_origin;
			int16_t scroll_center_x = static_cast<int16_t>(scroll_topleft.x() + scroll_dim.x() / 2);
			int16_t scroll_bottom_y = static_cast<int16_t>(scroll_topleft.y() + scroll_dim.y());
			int16_t panel_top = static_cast<int16_t>(scroll_bottom_y + CH_BELOW_GAP);
			channelsrc_pos = Point<int16_t>(
				scroll_center_x,
				static_cast<int16_t>(panel_top + CH_PANEL_H / 2)
			);
		}
		else
		{
			worldsrc_pos = Point<int16_t>(
				static_cast<int16_t>((vieww - slotline_w) / 2),
				WS_BANNER_FALLBACK_Y
			);
			channelsrc_pos = Point<int16_t>(
				static_cast<int16_t>((vieww - CH_PANEL_W) / 2),
				static_cast<int16_t>(WS_BANNER_FALLBACK_Y + CH_PANEL_TOP_GAP + CH_PANEL_H / 2)
			);
		}

		worldid = Setting<DefaultWorld>::get().load();
		channelid = Setting<DefaultChannel>::get().load();

		nl::node Login = nl::nx::UI["Login.img"];
		nl::node Common = Login["Common"];
		nl::node WorldSelect = Login["WorldSelect"];

		worldsrc = WorldSelect["BtWorld"];
		worldnamesrc = WorldSelect["world"];
		channelsrc = WorldSelect["channel"];

		if (!bg_managed)
		{
			Point<int16_t> background_pos = Point<int16_t>(vieww / 2, viewh / 2);
			sprites.emplace_back(WorldSelect["scroll"]["0"]["0"], background_pos);
		}

		buttons[Buttons::BtExit] = std::make_unique<MapleButton>(Common["BtExit"], WS_EXIT_POS);
		buttons[Buttons::BtGoWorld] = std::make_unique<MapleButton>(
			WorldSelect["BtGoworld"],
			channelsrc_pos + WS_GOWORLD_OFF
		);
		buttons[Buttons::BtGoWorld]->set_active(false);

		// 左侧侧栏：step 装饰 + 地图/全角色 stub + 回到首页（坐标见上方实测注释）。
		step_channel = Sprite(Common["step"]["1"], Point<int16_t>(WS_SIDEBAR_X, WS_SIDEBAR_STEP_Y));

		buttons[Buttons::BtViewChoice] = std::make_unique<MapleButton>(
			WorldSelect["BtViewChoice"],
			Point<int16_t>(WS_SIDEBAR_X, WS_SIDEBAR_Y0)
		);

		buttons[Buttons::BtViewAll] = std::make_unique<MapleButton>(
			WorldSelect["BtViewAll"],
			Point<int16_t>(WS_SIDEBAR_X, WS_SIDEBAR_Y1)
		);
		// BtViewAll 直接用真资产：与正确端截图（选择频道页.png）NCC=0.61 吻合，
		// 文案就是「查看所有角色」。此前 r20 误判 WZ 文案错误、生成了本地假 PNG
		// 覆盖（NCC=0.33、边框样式不对），已删除。

		buttons[Buttons::BtStart] = std::make_unique<MapleButton>(
			Common["BtStart"],
			Point<int16_t>(WS_SIDEBAR_X, WS_SIDEBAR_Y2)
		);

		channels_background = WorldSelect["chBackgrn"];

		for (uint16_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0; i++)
		{
			std::string channel = std::to_string(i);

			buttons[Buttons::BtChannel0 + i] = std::make_unique<TwoSpriteButton>(
				channelsrc[channel]["normal"],
				channelsrc[channel]["normal"],
				channelsrc_pos - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2) + channel_cell_offset(i)
			);
			buttons[Buttons::BtChannel0 + i]->set_active(false);

			// maximum = 条宽；update 传入 0..1 负载比例（见 change_world）。
			Texture gauge_tex(channelsrc["chgauge"]);
			channel_gauge[i] = Gauge(
				Gauge::Type::WORLDSELECT,
				gauge_tex,
				std::max<int16_t>(1, gauge_tex.width()),
				0.0f
			);
		}

		// 选中频道红圈动画（参考端在选中频道 CH.N 标签上画红圈圈选）。
		// chSelect 是循环动画，手动帧播放并定格末帧（完全展开的红圈）。
		if (nl::node chsel = channelsrc["chSelect"])
		{
			for (const nl::node& f : chsel)
			{
				Frame frame(f);

				if (frame.get_dimensions().x() > 0)
					ch_select_frames.push_back(frame);
			}
		}

		// 不预创建 BtWorld/e 空木牌：Map Obj signboard 已有空挂板，
		// 15 个 active 的 e 会盖住真实世界牌，观感就是「木牌全空」。
		// 世界列表到达后由 draw_world() 按实际条目创建。
		for (uint8_t i = 0; i < WS_MAX_WORLDS; i++)
		{
			buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(
				worldsrc["e"]["normal"]["0"],
				worldsrc["e"]["mouseOver"]["0"],
				worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i), 0)
			);
			buttons[Buttons::BtWorld0 + i]->set_active(false);
		}

		chatballoon.change_text(Locale::text("worldselect.hint_select_world"));

		// BeiDou：左侧已有「公告」气球，勿再初始化提示气泡（避免双提示框）。
		if (LoginBackground::get().available())
			chatballoon.change_text("");

		for (size_t i = 1; i <= FLAG_SIZE; i++)
			flag_sprites.emplace_back(Login["WorldNotice"][i]);

		if (Configuration::get().get_auto_login())
		{
			uint8_t world = Configuration::get().get_auto_world();
			uint8_t channel = Configuration::get().get_auto_channel();

			Configuration::get().set_worldid(world);
			Configuration::get().set_channelid(channel);

			UI::get().emplace<UILoginWait>();

			auto loginwait = UI::get().get_element<UILoginWait>();

			if (loginwait && loginwait->is_active())
			{
				// 与手工路径（点世界→进频道）一致：下发的是「槽位 → 真实世界 id」
				// 映射后的 id，而不是配置里的原始槽位值。
				const uint16_t worldid = get_worldbyid(world);

				CharlistRequestPacket(worldid, channel).dispatch();
			}
		}
	}

	Point<int16_t> UIWorldSelect::channel_panel_center() const
	{
		return channelsrc_pos;
	}

	Point<int16_t> UIWorldSelect::channel_panel_visual_center() const
	{
		// t=0: panel sits CH_SLIDE_DIST above rest; t=1: at rest.
		int16_t dy = static_cast<int16_t>((1.0f - channel_slide) * (-CH_SLIDE_DIST));
		return channelsrc_pos + Point<int16_t>(0, dy);
	}

	void UIWorldSelect::layout_channel_controls(Point<int16_t> panel_center)
	{
		Point<int16_t> panel_topleft = panel_center - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);

		if (Button* go = buttons[Buttons::BtGoWorld].get())
			go->set_position(panel_center + WS_GOWORLD_OFF);

		for (uint16_t i = 0; i < CHANNEL_BUTTON_COUNT; i++)
		{
			if (Button* ch = get_channel_button(i))
				ch->set_position(panel_topleft + channel_cell_offset(i));
		}
	}

	void UIWorldSelect::update()
	{
		UIElement::update();

		// 卷轴展开：0→1，完成后激活频道/进入按钮。
		if (world_selected && panel_expand < 1.0f)
		{
			panel_expand += static_cast<float>(Constants::TIMESTEP) / CH_EXPAND_MS;

			if (panel_expand >= 1.0f)
			{
				panel_expand = 1.0f;
				activate_panel_controls();
			}
		}

		if (world_selected && channel_slide < 1.0f)
		{
			channel_slide += static_cast<float>(Constants::TIMESTEP) / CH_SLIDE_MS;

			if (channel_slide > 1.0f)
				channel_slide = 1.0f;

			layout_channel_controls(channel_panel_visual_center());
		}

		// 公告展开与频道滑入同拍。
		if (world_selected && notice_slide < 1.0f)
		{
			notice_slide += static_cast<float>(Constants::TIMESTEP) / CH_SLIDE_MS;

			if (notice_slide > 1.0f)
				notice_slide = 1.0f;
		}

		if (scroll_playing)
		{
			if (scroll_open.update())
			{
				// 播完一轮后定格为静态频道底板，避免 rewind 闪回首帧。
				scroll_playing = false;
			}
		}

		// 选中频道红圈动画推进；播完定格末帧（圈选静态停留在选中频道上）。
		if (ch_select_playing)
		{
			ch_select_elapsed = static_cast<uint16_t>(ch_select_elapsed + Constants::TIMESTEP);

			while (ch_select_playing)
			{
				uint16_t delay = ch_select_frames[static_cast<size_t>(ch_select_frame)].get_delay();

				if (ch_select_elapsed < delay)
					break;

				ch_select_elapsed = static_cast<uint16_t>(ch_select_elapsed - delay);
				ch_select_frame++;

				if (ch_select_frame >= static_cast<int16_t>(ch_select_frames.size()))
				{
					ch_select_frame = static_cast<int16_t>(ch_select_frames.size() - 1);
					ch_select_playing = false;
					break;
				}
			}
		}
	}

	void UIWorldSelect::activate_panel_controls()
	{
		// 展开完成后才让频道按钮可点：展开过程中板还没铺开，按钮悬空可点会很违和。
		for (uint16_t i = 0; i < pending_channel_limit; ++i)
		{
			if (Button* channel_btn = get_channel_button(i))
			{
				channel_btn->set_active(true);

				if (i == channelid)
					channel_btn->set_state(Button::State::PRESSED);
			}
		}

		if (auto it = buttons.find(Buttons::BtGoWorld); it != buttons.end() && it->second)
			it->second->set_active(true);
	}

	void UIWorldSelect::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		step_channel.draw(position, alpha);

		for (size_t i = 0; i < worlds.size(); i++)
		{
			World world = worlds[i];

			if (world.flag > 0 && world.flag <= FLAG_SIZE)
				flag_sprites[world.flag - 1].draw(
					position + worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i) + 5, 0),
					alpha
				);
		}

		Point<int16_t> panel_c = channel_panel_visual_center();

		if (world_selected)
		{
			if (scroll_playing && scroll_has_anim)
			{
				scroll_open.draw(position + panel_c, alpha);
			}
			else
			{
				// 卷轴展开：按进度自上而下纵向揭示底板。源裁剪（露出顶部 reveal 行）
				// 与目标高度同步收缩，保持 1:1 不拉伸；未用贴图拉伸是因为木框会被压扁。
				const int16_t full_h = channels_background.get_dimensions().y();
				int16_t reveal = static_cast<int16_t>(full_h * panel_expand);

				if (reveal < 1)
					reveal = 1;

				if (reveal < full_h)
				{
					channels_background.draw(
						DrawArgument(position + panel_c, Point<int16_t>(0, reveal)),
						Range<int16_t>(0, static_cast<int16_t>(full_h - reveal))
					);
				}
				else
				{
					channels_background.draw(position + panel_c);
				}
			}
		}

		UIElement::draw_buttons(alpha);

		if (world_selected && worldid < worlds.size())
		{
			World selectedWorld = worlds[worldid];
			uint8_t channel_total = selectedWorld.channel_count;

			// 内容随展开进度淡入：板没铺开前不画世界名/负载条/公告。
			// 注意：draw(alpha) 的 alpha 是帧间插值系数（每帧 0→1 往复），
			// 绝不能当不透明度乘进来——否则世界名条每帧透明度都不同，
			// 表现为「蓝蜗牛一直闪烁」（用户实测 GIF 反馈）。
			const float panel_a = panel_expand < 0.0f ? 0.0f : (panel_expand > 1.0f ? 1.0f : panel_expand);
			const float content_alpha = panel_a;

			if (content_alpha > 0.0f)
			{
				Texture world_name(worldnamesrc[std::to_string(selectedWorld.id)]);
				world_name.draw(DrawArgument(position + panel_c + WS_WORLD_NAME_ON_PANEL, content_alpha));

				Point<int16_t> panel_topleft = panel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);

				for (size_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0 && i < channel_total; i++)
				{
					channel_gauge[i].draw(
						DrawArgument(position + panel_topleft + channel_cell_offset(static_cast<uint16_t>(i)), content_alpha)
					);
				}

				// 选中频道红圈圈选（channel/chSelect，播完定格末帧）。
				// 帧为「中心锚点」且逐帧变大（23x11→59x33）：anchor 取末帧 origin，
				// 使末帧视觉框恰好以频道单元原点为左上、圈心始终对准 CH.N 标签；
				// 之前硬编码 (2,-9) 没补偿 origin，圈被上左平移了半个帧
				// （用户实测「圈选位置不在选中的频道上」）。
				if (channelid != 0xFF && !ch_select_frames.empty())
				{
					const Frame& circle = ch_select_frames[static_cast<size_t>(
						ch_select_playing ? ch_select_frame : ch_select_frames.size() - 1
					)];

					circle.draw(DrawArgument(
						position + panel_topleft + channel_cell_offset(channelid) + circle.get_origin(),
						content_alpha
					));
				}

				if (!world_notice.empty())
				{
					// 公告随 channel_slide 自上方滑入（与卷轴展开同步）。
					// LoginProcessNotice::draw 只接受坐标，无 alpha 通道，故不参与淡入。
					const int16_t notice_dy = static_cast<int16_t>((1.0f - notice_slide) * (-40));
					const int16_t notice_x = static_cast<int16_t>(
						panel_c.x() - CH_PANEL_W / 2 - world_notice.width() - WS_NOTICE_GAP
					);
					const int16_t notice_y = static_cast<int16_t>(
						panel_c.y() - CH_PANEL_H / 2 + 8 + notice_dy
					);
					world_notice.draw(position + Point<int16_t>(notice_x, notice_y));
				}
			}
		}

		version.draw(position + version_pos - Point<int16_t>(0, 5));
		// BeiDou 用左侧「公告」气球；旧版英文 chatballoon 再画会变成两个提示框。
		if (!LoginBackground::get().available())
			chatballoon.draw(position + Point<int16_t>(747, 105));
	}

	Cursor::State UIWorldSelect::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> panel_c = channel_panel_visual_center();
		Point<int16_t> panel_topleft = panel_c - Point<int16_t>(CH_PANEL_W / 2, CH_PANEL_H / 2);

		Rectangle<int16_t> channels_bounds = Rectangle<int16_t>(
			position + panel_topleft,
			position + panel_topleft + channels_background.get_dimensions()
			);

		// 「到选择的世界去」在卷内右下，仍单独并入命中，避免点按钮边缘被当成面板外取消选中。
		bool inside_goworld = false;

		auto goworld = buttons.find(Buttons::BtGoWorld);

		if (goworld != buttons.end() && goworld->second->is_active())
			inside_goworld = goworld->second->bounds(position).contains(cursorpos);

		if (world_selected && !channels_bounds.contains(cursorpos) && !inside_goworld)
		{
			if (clicked)
			{
				world_selected = false;
				channel_slide = 1.0f;
				panel_expand = 1.0f;
				clear_selected_world();
			}
		}

		Cursor::State ret = clicked ? Cursor::State::CLICKING : Cursor::State::IDLE;

		for (auto& btit : buttons)
		{
			if (btit.second->is_active() && btit.second->bounds(position).contains(cursorpos))
			{
				if (btit.second->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();

					btit.second->set_state(Button::State::MOUSEOVER);
					ret = Cursor::State::CANCLICK;
				}
				else if (btit.second->get_state() == Button::State::PRESSED)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));

						ret = Cursor::State::IDLE;
					}
					else
					{
						ret = Cursor::State::CANCLICK;
					}
				}
				else if (btit.second->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));

						ret = Cursor::State::IDLE;
					}
					else
					{
						ret = Cursor::State::CANCLICK;
					}
				}
			}
			else if (btit.second->get_state() == Button::State::MOUSEOVER)
			{
				btit.second->set_state(Button::State::NORMAL);
			}
		}

		return ret;
	}

	void UIWorldSelect::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (world_selected)
			{
				World selectedWorld = worlds[worldid];

				uint8_t selected_channel = (channelid == 0xFF) ? 0 : channelid;
				uint8_t channel_total = selectedWorld.channel_count;

				uint8_t columns = std::min(channel_total, COLUMNS);

				uint8_t rows = std::floor((channel_total - 1) / COLUMNS) + 1;

				div_t div = std::div(selected_channel, columns);
				int32_t current_col = div.rem;

				if (keycode == KeyAction::Id::UP)
				{
					auto next_channel = (selected_channel - COLUMNS < 0 ? (selected_channel - COLUMNS) + rows * COLUMNS : selected_channel - COLUMNS);

					if (next_channel == channelid)
						return;

					if (next_channel > channel_total)
						button_pressed(next_channel - COLUMNS + Buttons::BtChannel0);
					else
						button_pressed(next_channel + Buttons::BtChannel0);
				}
				else if (keycode == KeyAction::Id::DOWN)
				{
					auto next_channel = (selected_channel + COLUMNS >= channel_total ? current_col : selected_channel + COLUMNS);

					if (next_channel == channelid)
						return;

					if (next_channel > channel_total)
						button_pressed(next_channel + COLUMNS + Buttons::BtChannel0);
					else
						button_pressed(next_channel + Buttons::BtChannel0);
				}
				else if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::TAB)
				{
					if (selected_channel != 0)
						selected_channel--;
					else
						selected_channel = channel_total - 1;

					button_pressed(selected_channel + Buttons::BtChannel0);
				}
				else if (keycode == KeyAction::Id::RIGHT)
				{
					if (selected_channel != channel_total - 1)
						selected_channel++;
					else
						selected_channel = 0;

					button_pressed(selected_channel + Buttons::BtChannel0);
				}
				else if (escape)
				{
					world_selected = false;
					channel_slide = 1.0f;
					panel_expand = 1.0f;
					clear_selected_world();
				}
				else if (keycode == KeyAction::Id::RETURN)
				{
					button_pressed(Buttons::BtGoWorld);
				}
				else if (keycode == 'S' || keycode == 's' || keycode == 'C' || keycode == 'c')
				{
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							int ch = std::stoi(s);
							if (ch < 0) return;
							World selectedWorld = worlds[worldid];
							if (ch >= selectedWorld.channel_count) return;
							button_pressed(static_cast<uint16_t>(ch + Buttons::BtChannel0));
							LOG(LOG_DEBUG, "UIWorldSelect: SoftKey channel=" << ch);
						} catch (...) {}
					}, []() {}, "Channel #");
				}
				else if (keycode == 'G' || keycode == 'g')
				{
					button_pressed(Buttons::BtGoWorld);
				}
			}
			else
			{
				auto selected_world = worldid;

				if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::RIGHT || keycode == KeyAction::Id::UP || keycode == KeyAction::Id::DOWN || keycode == KeyAction::Id::TAB)
				{
					// 导航范围取「服务端下发世界数 ∩ 客户端按钮容量」：
					// worldcount 可能大于容量，若直接用它做取模/边界，选中值会落到
					// 不存在的槽位，索引随即越界到频道按钮段。
					const uint16_t navcount = navigable_world_count();

					if (navcount > 0)
					{
						if (worldid >= navcount)
							worldid = 0;

						if (keycode == KeyAction::Id::LEFT || keycode == KeyAction::Id::UP)
							selected_world = (worldid == 0) ? navcount - 1 : worldid - 1;
						else
							selected_world = (worldid + 1 >= navcount) ? 0 : worldid + 1;

						if (Button* previous = get_world_button(worldid))
							previous->set_state(Button::State::NORMAL);

						worldid = static_cast<uint8_t>(selected_world);

						if (Button* current = get_world_button(worldid))
							current->set_state(Button::State::PRESSED);
					}
				}
				else if (escape)
				{
					auto quitconfirm = UI::get().get_element<UIQuitConfirm>();

					if (quitconfirm && quitconfirm->is_active())
						return UI::get().send_key(keycode, pressed);
					else
						button_pressed(Buttons::BtExit);
				}
				else if (keycode == KeyAction::Id::RETURN)
				{
					auto quitconfirm = UI::get().get_element<UIQuitConfirm>();

					if (quitconfirm && quitconfirm->is_active())
					{
						return UI::get().send_key(keycode, pressed);
					}
					else
					{
						// 回车进入当前选中世界：仅在选中槽位确实存在时才派发，
						// 否则按下的 id 会落到频道/进入按钮段。
						if (worldid < navigable_world_count())
							button_pressed(worldid + Buttons::BtWorld0);
					}
				}
				else if (keycode == 'W' || keycode == 'w' || keycode == 'S' || keycode == 's')
				{
					UI::get().emplace<UISoftKey>([this](const std::string& s) {
						try {
							int wid = std::stoi(s);
							const uint16_t nav = navigable_world_count();
							if (wid < 0 || static_cast<uint16_t>(wid) >= nav) return;
							button_pressed(static_cast<uint16_t>(wid) + Buttons::BtWorld0);
							LOG(LOG_DEBUG, "UIWorldSelect: SoftKey world=" << wid);
						} catch (...) {}
					}, []() {}, "World #");
				}
			}
		}
	}

	UIElement::Type UIWorldSelect::get_type() const
	{
		return TYPE;
	}

	void UIWorldSelect::draw_world()
	{
		world_textures.clear();
		world_map.clear();

		if (worldcount <= 0)
		{
			LOG(LOG_INFO, "[WorldSelect] draw_world: empty world list (count=0)");
			// 正确截图：无世界时仍显示一排空木牌（BtWorld/e）。
			for (uint8_t i = 0; i < WS_MAX_WORLDS; i++)
			{
				buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(
					worldsrc["e"]["normal"]["0"],
					worldsrc["e"]["mouseOver"]["0"],
					worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i), 0)
				);
				buttons[Buttons::BtWorld0 + i]->set_active(true);
			}
			return;
		}

		for (size_t i = 0; i < worlds.size() && i < WS_MAX_WORLDS; i++)
		{
			World& world = worlds[i];

			// 若包解析把 count 读小，用 capacities 长度兜底。
			if (world.channel_count < world.channel_capacities.size())
			{
				world.channel_count = static_cast<uint8_t>(
					std::min<size_t>(255, world.channel_capacities.size())
				);
				LOG(LOG_INFO, "[WorldSelect] fixup channel_count id="
					<< static_cast<int>(world.id) << " -> "
					<< static_cast<int>(world.channel_count));
			}

			std::string world_str = std::to_string(world.id);
			nl::node worldbtn = worldsrc[world_str];

			if (!worldbtn || !worldbtn["normal"]["0"])
				worldbtn = worldsrc["0"];
			if (!worldbtn || !worldbtn["normal"]["0"])
				worldbtn = worldsrc["e"];

			nl::node normal = worldbtn["normal"]["0"] ? worldbtn["normal"]["0"] : worldbtn["normal"];
			nl::node over = worldbtn["mouseOver"]["0"] ? worldbtn["mouseOver"]["0"] : normal;

			world_map[static_cast<uint16_t>(i)] = static_cast<uint16_t>(world.id);
			world_textures.emplace_back(worldnamesrc[world_str] ? worldnamesrc[world_str] : worldnamesrc["0"]);

			// 木牌按槽位挂横杆：首牌为世界名（如「1 蓝蜗牛」），其后空 e。
			buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(
				normal,
				over,
				worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * static_cast<int>(i)), 0)
			);
			buttons[Buttons::BtWorld0 + i]->set_active(true);
		}

		const uint8_t filled = static_cast<uint8_t>(
			std::min<size_t>(worlds.size(), WS_MAX_WORLDS)
		);

		// 正确截图：世界牌右侧填满空木牌 e。
		for (uint8_t i = filled; i < WS_MAX_WORLDS; i++)
		{
			buttons[Buttons::BtWorld0 + i] = std::make_unique<TwoSpriteButton>(
				worldsrc["e"]["normal"]["0"],
				worldsrc["e"]["mouseOver"]["0"],
				worldsrc_pos + Point<int16_t>(static_cast<int16_t>(WS_SLOT_STEP_X * i), 0)
			);
			buttons[Buttons::BtWorld0 + i]->set_active(true);
		}

		world_list_finalized = true;

		LOG(LOG_INFO, "[WorldSelect] draw_world: worlds=" << worlds.size()
			<< " filled=" << static_cast<int>(filled)
			<< " pos=(" << worldsrc_pos.x() << "," << worldsrc_pos.y() << ")");
	}

	void UIWorldSelect::add_world(World world)
	{
		// 上一轮列表已 draw_world 定稿后，又收到新世界 → 新一轮 SERVERLIST，先清空。
		if (world_list_finalized)
		{
			clear_worlds();
			world_list_finalized = false;
		}

		// 超出按钮容量时不再静默丢弃：记录限频告警，便于发现「服务端世界列表
		// 与客户端按钮容量不匹配」。同时不再把该世界放进 worlds，
		// 以保证 worldcount == worlds.size() 这一不变量，键盘导航因此不会越过实际槽位。
		if (worldcount >= WS_MAX_WORLDS)
		{
			warn_button_index_out_of_range(WS_BOUNDS_WARNED_WORLD, WS_MAX_WORLDS, "world", WS_MAX_WORLDS);

			return;
		}

		worlds.emplace_back(std::move(world));
		worldcount++;
	}

	void UIWorldSelect::clear_worlds()
	{
		worlds.clear();
		worldcount = 0;
		world_list_finalized = false;
	}

	void UIWorldSelect::change_world(World selectedWorld)
	{
		if (Button* world_btn = get_world_button(worldid))
			world_btn->set_state(Button::State::PRESSED);

		// 频道相关数据全部来自服务端，且三者长度未必一致：
		//   - channel_count：包里的频道数（uint8_t）；
		//   - channel_capacities：变长 vector，越界读属未定义行为；
		//   - channel_gauge：**定长数组**（CHANNEL_BUTTON_COUNT），越界写会直接破坏内存。
		// 故按三者最小值收敛，多余频道忽略并限频告警。
		uint16_t limit = static_cast<uint16_t>(selectedWorld.channel_count);

		if (limit > CHANNEL_BUTTON_COUNT)
		{
			warn_channel_count_out_of_range(limit);

			limit = CHANNEL_BUTTON_COUNT;
		}

		if (static_cast<size_t>(limit) > selectedWorld.channel_capacities.size())
			limit = static_cast<uint16_t>(selectedWorld.channel_capacities.size());

		// 频道按钮 / BtGoWorld 的激活推迟到卷轴展开完成（update 中 panel_expand>=1
		// 时由 activate_panel_controls 执行），避免展开过程中按钮悬在未展开的板上。
		pending_channel_limit = limit;

		for (uint16_t i = 0; i < limit; ++i)
		{
			// Gauge::draw 用 percentage*maximum；必须传 0..1，不能传绝对人数。
			float load = static_cast<float>(selectedWorld.channel_capacities[i]) / static_cast<float>(CHANNEL_LOAD);
			if (load < 0.0f)
				load = 0.0f;
			if (load > 1.0f)
				load = 1.0f;
			channel_gauge[i].update(load);
		}

		world_notice.change_text(selectedWorld.event_message);
		notice_slide = 0.0f;
	}

	void UIWorldSelect::remove_selected()
	{
		deactivate();

		Sound(Sound::Name::SCROLLUP).play();

		world_selected = false;
		channel_slide = 1.0f;
		notice_slide = 1.0f;
		panel_expand = 1.0f;
		scroll_playing = false;

		clear_selected_world();
	}

	void UIWorldSelect::set_region(uint8_t regionid)
	{
		// BeiDou 版式下没有 index/[region] 分层，大区列表直接由服务端下发的
		// 世界 id 决定；仍持久化 login.region 供下次默认选区。
		Setting<DefaultRegion>::get().save(regionid);
	}

	uint16_t UIWorldSelect::get_worldbyid(uint16_t worldid)
	{
		auto it = world_map.find(worldid);

		// 槽位 -> 真实世界 id；查不到时退回槽位本身（避免解引用 end()）。
		return (it != world_map.end()) ? it->second : worldid;
	}

	Button* UIWorldSelect::get_world_button(uint16_t index) const
	{
		if (index >= WS_MAX_WORLDS)
		{
			warn_button_index_out_of_range(WS_BOUNDS_WARNED_WORLD, index, "world", WS_MAX_WORLDS);

			return nullptr;
		}

		auto it = buttons.find(static_cast<uint16_t>(Buttons::BtWorld0 + index));

		// 未创建（例如该槽世界因 channel_count==0 被跳过）时同样返回空，由调用方跳过。
		return (it != buttons.end()) ? it->second.get() : nullptr;
	}

	Button* UIWorldSelect::get_channel_button(uint16_t index) const
	{
		if (index >= CHANNEL_BUTTON_COUNT)
		{
			warn_button_index_out_of_range(WS_BOUNDS_WARNED_CHANNEL, index, "channel", CHANNEL_BUTTON_COUNT);

			return nullptr;
		}

		auto it = buttons.find(static_cast<uint16_t>(Buttons::BtChannel0 + index));

		return (it != buttons.end()) ? it->second.get() : nullptr;
	}

	uint16_t UIWorldSelect::navigable_world_count() const
	{
		// 键盘导航不得越过实际容量：worldcount 只反映服务端下发的世界数，
		// 可能大于客户端按钮容量（WS_MAX_WORLDS），越过就会命中通道按钮段。
		const uint16_t by_server = static_cast<uint16_t>(worldcount);

		return (by_server < WS_MAX_WORLDS) ? by_server : WS_MAX_WORLDS;
	}

	Button::State UIWorldSelect::button_pressed(uint16_t id)
	{
		// 参考端 CUIWorldSelect::IsRequestValid：step 1 且 CLogin+368 门闩为空。
		// 当前端以「活动中的 UILoginWait」作为“请求进行中”的等价标记；
		// 取消后的 Wait 只 deactivate（不能在按钮回调里 remove(this)），
		// 若按“存在”判定会把世界选择永久锁死。
		if (auto loginwait = UI::get().get_element<UILoginWait>())
		{
			if (loginwait->is_active())
				return Button::State::NORMAL;
		}

		if (id == Buttons::BtGoWorld)
		{
			enter_world();

			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtExit)
		{
			UI::get().emplace<UIQuitConfirm>();

			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtStart)
		{
			// 回到首页：重置会话后再进登录，否则服仍停在选世界态，
			// 再点连接会一直卡在 Loading（收不到 LoginResult）。
			Sound(Sound::Name::SCROLLUP).play();
			Session::get().reconnect();
			LoginBackground::get().set_step(0);
			UI::get().remove(UIElement::Type::WORLDSELECT);
			UI::get().remove(UIElement::Type::LOGINWAIT);
			UI::get().remove(UIElement::Type::LOGINNOTICE);
			UI::get().emplace<UILogin>();

			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtViewAll)
		{
			ViewAllCharPacket().dispatch();
			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtViewChoice)
		{
			// 登录态无世界地图：SoftKey 选区 / 回退提示
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) {
					UI::get().emplace<UIRegion>();
					deactivate();
					LOG(LOG_DEBUG, "UIWorldSelect: BtViewChoice empty → UIRegion");
					return;
				}
				auto reg = UI::get().get_element<UIRegion>();
				if (!reg) reg = UI::get().emplace<UIRegion>();
				else if (!reg->is_active()) reg->makeactive();
				LOG(LOG_DEBUG, "UIWorldSelect: BtViewChoice SoftKey → UIRegion tip=" << tip);
			}, []() {}, "Region tip|empty=Region");
			return Button::State::NORMAL;
		}
		else if (id == Buttons::BtRegion)
		{
			UI::get().emplace<UIRegion>();

			deactivate();

			return Button::State::NORMAL;
		}
		else if (id >= Buttons::BtWorld0 && id < Buttons::BtChannel0)
		{
			const uint16_t next_world = static_cast<uint16_t>(id - Buttons::BtWorld0);

			// 槽位须对应已创建的世界按钮；draw_world 仅跳过 channel_count==0。
			if (next_world >= worlds.size() || !get_world_button(static_cast<uint8_t>(next_world)))
			{
				warn_button_index_out_of_range(
					WS_BOUNDS_WARNED_WORLD, next_world, "world", static_cast<uint16_t>(worlds.size()));

				return Button::State::NORMAL;
			}

			if (Button* previous = get_world_button(worldid))
				previous->set_state(Button::State::NORMAL);

			worldid = static_cast<uint8_t>(next_world);

			ServerStatusRequestPacket(get_worldbyid(worldid)).dispatch();

			world_selected = true;
			// 卷轴展开动画接管面板入场（不再用整板下滑，避免与展开双重运动）。
			channel_slide = 1.0f;
			notice_slide = 0.0f;
			panel_expand = 0.0f;
			pending_channel_limit = 0;
			// BeiDou 地图已有 signboard 卷轴；Login.img/WorldSelect/scroll 是旧版全屏卷（513×401），
			// 锚在频道面板中心会错位。仅用 CH_SLIDE，不再播该动画。
			if (!LoginBackground::get().available())
			{
				if (nl::node open = nl::nx::UI["Login.img"]["WorldSelect"]["scroll"]["0"])
				{
					scroll_open = Animation(open);
					scroll_open.reset();
					scroll_has_anim = true;
					scroll_playing = true;
				}
				else
				{
					scroll_has_anim = false;
					scroll_playing = false;
				}
			}
			else
			{
				scroll_has_anim = false;
				scroll_playing = false;
			}
			layout_channel_controls(channel_panel_visual_center());
			clear_selected_world();
			change_world(worlds[worldid]);

			return Button::State::PRESSED;
		}
		else if (id >= Buttons::BtChannel0 && id < Buttons::BtGoWorld)
		{
			const uint8_t selectedch = static_cast<uint8_t>(id - Buttons::BtChannel0);

			// 尚未选中任何频道（clear 后 channelid=0xFF）或点了其它频道 → 仅选中。
			// 再次点击同一频道 → 进入世界。
			if (channelid == 0xFF || selectedch != channelid)
			{
				if (channelid != 0xFF)
				{
					if (Button* previous = get_channel_button(channelid))
						previous->set_state(Button::State::NORMAL);
				}

				channelid = selectedch;

				if (Button* current = get_channel_button(channelid))
					current->set_state(Button::State::PRESSED);

				// 选中频道：播红圈圈选动画（播完定格在选中频道上）。
				if (!ch_select_frames.empty())
				{
					ch_select_frame = 0;
					ch_select_elapsed = 0;
					ch_select_playing = true;
				}

				Sound(Sound::Name::WORLDSELECT).play();
			}
			else
			{
				enter_world();
			}

			return Button::State::PRESSED;
		}
		else
		{
			return Button::State::NORMAL;
		}
	}

	void UIWorldSelect::set_server_status(int16_t status)
	{
		world_status = status;

		// 0 正常：清掉提示记录，允许下次拥挤再提示一次。
		if (status == 0)
		{
			status_notified_world = -1;
			return;
		}

		// 1 拥挤 / 2 满员：同一世界只提示一次，避免反复点世界刷屏。
		if (status_notified_world == static_cast<int16_t>(worldid))
			return;

		status_notified_world = static_cast<int16_t>(worldid);

		UI::get().emplace<UILoginNotice>(UILoginNotice::Message::POPULATION_TOO_HIGH);
	}

	void UIWorldSelect::reset_request_state()
	{
		// 请求中止（Wait 取消/会话重连）后复位：清掉满员拦截与提示记录，
		// 允许玩家重新点世界触发新的 SERVERSTATUS_REQUEST。
		world_status = 0;
		status_notified_world = -1;
	}

	void UIWorldSelect::enter_world()
	{
		if (auto loginwait = UI::get().get_element<UILoginWait>())
		{
			if (loginwait->is_active())
				return;
		}

		if (channelid == 0xFF)
			return;

		// 满员世界：SERVERSTATUS=2 时拒绝进入（参考端 CUIWorldSelect 行为），
		// 玩家可重新点世界触发一次新的 SERVERSTATUS_REQUEST 再试。
		if (world_status == 2)
		{
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::POPULATION_TOO_HIGH);
			return;
		}

		Configuration::get().set_worldid(worldid);
		Configuration::get().set_channelid(channelid);

		UI::get().emplace<UILoginWait>();
		auto loginwait = UI::get().get_element<UILoginWait>();

		if (loginwait && loginwait->is_active())
			CharlistRequestPacket(get_worldbyid(worldid), channelid).dispatch();
	}

	void UIWorldSelect::clear_selected_world()
	{
		// 不预选 CH.0：否则首点 CH.1（index 0）会命中 selectedch==channelid 直接 enter_world，
		// 频道滑入动画与「再点确认」流程被跳过。
		channelid = 0xFF;
		ch_select_playing = false;

		// 频道按钮已在构造函数中按 CHANNEL_BUTTON_COUNT 全部创建；
		// 这里仍统一走带范围校验的访问器，避免日后调整容量时漏改本处。
		for (uint16_t i = 0; i < CHANNEL_BUTTON_COUNT; i++)
		{
			if (Button* channel_btn = get_channel_button(i))
				channel_btn->set_state(Button::State::NORMAL);
		}

		for (size_t i = 0; i < Buttons::BtGoWorld - Buttons::BtChannel0; i++)
		{
			buttons[Buttons::BtChannel0 + i]->set_active(false);

			channel_gauge[i].update(0);
		}

		buttons[Buttons::BtGoWorld]->set_active(false);

		world_notice.change_text("");
	}

	uint16_t UIWorldSelect::get_next_world(uint16_t id, bool upward)
	{
		// 与键盘导航同源：范围取「服务端下发世界数 ∩ 客户端按钮容量」，
		// 保证返回值始终指向实际存在的槽位。
		const uint16_t navcount = navigable_world_count();

		if (navcount == 0)
			return 0;

		if (upward)
			return (id == 0) ? navcount - 1 : id - 1;

		return (id + 1 >= navcount) ? 0 : id + 1;
	}
}

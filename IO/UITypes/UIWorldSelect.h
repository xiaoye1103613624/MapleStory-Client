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
#pragma once

#include "../UIElement.h"

#include "../Components/ChatBalloon.h"
#include "../Components/Gauge.h"

#include "../../Graphics/Animation.h"
#include "../../Net/Login.h"

namespace ms
{
	class UIWorldSelect : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WORLDSELECT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		// 按钮容量：世界按钮槽数量与频道按钮数量。
		// 集中定义并由本类 Buttons 枚举**表达式推导**出各 id 段起点，
		// 避免历史上「BtChannel0 隐式=5、BtGoWorld 硬编码=35」这种与枚举顺序
		// 彼此独立写死的耦合——一旦增删世界按钮就会静默错位到频道按钮段。
		// 取值对齐参考版（gms083）容量：15 个世界、30 个频道。
		static constexpr uint16_t WORLD_BUTTON_COUNT = 15U;
		static constexpr uint16_t CHANNEL_BUTTON_COUNT = 30U;

		UIWorldSelect();

		void draw(float alpha) const override;
		void update() override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void draw_world();
		void add_world(World world);
		/// 清空世界列表（重连 / 新一轮 SERVERLIST 开始时由 add_world 自动调用）。
		void clear_worlds();
		void change_world(World selectedWorld);
		void remove_selected();
		void set_region(uint8_t value);
		uint16_t get_worldbyid(uint16_t worldid);

		/// 服务端 SERVERSTATUS 回包：0 正常 / 1 拥挤 / 2 满员。
		/// 拥挤仅提示，满员额外阻止进入该世界（参考端 CUIWorldSelect 语义）。
		void set_server_status(int16_t status);
		/// 请求中止（Wait 取消 / 会话重连）后复位满员拦截与提示记录。
		void reset_request_state();

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		static constexpr uint8_t COLUMNS = 4U; // 频道面板 chBackgrn 宽 449，4 列(100px)才不溢出；与 channel_cell_offset 的 COLS=4 对齐
		static constexpr uint8_t FLAG_SIZE = 3U;
		// This is from the server in ServerConstants
		static constexpr int32_t CHANNEL_LOAD = 100;

		void enter_world();
		void clear_selected_world();
		uint16_t get_next_world(uint16_t id, bool upward);

		/// Final rest position of channel panel center (after slide-in).
		Point<int16_t> channel_panel_center() const;
		/// Center used for draw/hit while slide animates (starts above rest).
		Point<int16_t> channel_panel_visual_center() const;
		void layout_channel_controls(Point<int16_t> panel_center);
		/// 卷轴展开完成后激活频道/进入按钮（展开过程中按钮会悬在未展开的板上）。
		void activate_panel_controls();

		/// <summary>
		/// 取第 index 个世界按钮（0 起）；索引越界返回 nullptr，并做**限频告警**。
		///
		/// 为什么必须统一走本函数：基类的 buttons 是
		/// `std::map&lt;uint16_t, std::unique_ptr&lt;Button&gt;&gt;`，不是数组。
		/// 直接用 `buttons[BtWorld0 + index]` 访问越界 key 时：
		///   1) 若该 key 恰好落在频道/进入/退出按钮的 id 段内 → 误改那些按钮的状态；
		///   2) 若该 key 完全不存在 → operator[] 会静默插入一个空 unique_ptr，
		///      紧接着的 `-&gt;set_state()` 即空指针解引用崩溃。
		/// 服务端下发的世界 id/世界数不受客户端控制，故必须在此收敛。
		/// </summary>
		Button* get_world_button(uint16_t index) const;

		/// 取第 index 个频道按钮（0 起）；语义同 get_world_button。
		Button* get_channel_button(uint16_t index) const;

		/// 可参与键盘导航的世界槽数量 = min(服务端下发世界数, 世界按钮容量)。
		uint16_t navigable_world_count() const;

		enum Buttons : uint16_t
		{
			BtWorld0,
			BtWorld1,
			BtWorld2,
			BtWorld3,
			BtWorld4,
			BtWorld5,
			BtWorld6,
			BtWorld7,
			BtWorld8,
			BtWorld9,
			BtWorld10,
			BtWorld11,
			BtWorld12,
			BtWorld13,
			BtWorld14,
			BtChannel0 = BtWorld0 + WORLD_BUTTON_COUNT,
			BtGoWorld = BtChannel0 + CHANNEL_BUTTON_COUNT,
			BtRegion,
			BtExit,
			BtViewChoice,
			BtViewAll,
			BtStart
		};

		/// If ever changing order, check the WZ file.
		enum Worlds : uint16_t
		{
			SCANIA,
			BERA,
			BROA,
			WINDIA,
			KHAINI,
			BELLOCAN,
			MARDIA,
			KRADIA,
			YELLONDE,
			DEMETHOS,
			GALICIA,
			ELNIDO,
			ZENITH,
			ARCANIA,
			CHAOS,
			NOVA,
			RENEGADES,
			AURORA,
			ELYSIUM,
			KOREAN_ENOSIS = 29,
			LUNA,
			ELYSIUM_LINK,
			LAB = 40,
			KOREAN_RED = 43,
			KOREAN_AURORA,
			REBOOT0,
			REBOOT1,
			PINKBEAN = 48,
			BURNING,
			KOREAN_ARCANE,
			KOREAN_NOVA,
			TESPIA = 100
		};

		Text version;
		Point<int16_t> version_pos;
		// BtViewAll：直接用 Login.img/WorldSelect/BtViewAll 真资产
		// （NCC 0.61 vs 参考端截图；旧的本地假 PNG 覆盖 NCC 仅 0.33，已删）。
		Texture worlds_background;
		Texture channels_background;
		Point<int16_t> worldsrc_pos;
		Point<int16_t> channelsrc_pos;
		ChatBalloonHorizontal chatballoon;

		// worldid/channelid 必须默认初始化：此前两者均无初始化器，UIWorldSelect 构造后
		// 为不确定值；draw_world() 又会把"越界值"归零成 channelid=0（预选 CH.0），
		// 导致用户首点 CH.1(index 0) 即命中 selectedch==channelid → 直接 enter_world
		// 进游戏（表现为"选中频道就进游戏"，跳过选角色界面）。
		// 0xFF = 未预选任何频道，与 clear_selected_world() 的语义一致。
		uint8_t worldid = 0;
		uint8_t channelid = 0xFF;
		uint8_t worldcount;

		/// 0 = just selected (panel above); 1 = fully slid to rest. 250ms lerp.
		float channel_slide;
		/// 公告气球滑入进度（与 channel_slide 同拍）。
		float notice_slide;
		/// 频道板卷轴展开进度 0..1：0 = 仅顶部一条，1 = 完全展开（参考端展开动画）。
		float panel_expand = 1.0f;
		/// change_world 时记录的频道数，展开完成后由 activate_panel_controls 激活按钮。
		uint16_t pending_channel_limit = 0;

		/// Login.img/WorldSelect/scroll/0 卷轴打开动画。
		Animation scroll_open;
		bool scroll_playing;
		bool scroll_has_anim;

		/// 选中频道后的红圈圈选动画（channel/chSelect 0..3，100/50/50ms，
		/// 播完定格末帧）；circle 只在 channelid 有效时显示。
		std::vector<Frame> ch_select_frames;
		int16_t ch_select_frame = 0;
		uint16_t ch_select_elapsed = 0;
		bool ch_select_playing = false;

		std::vector<World> worlds;
		std::vector<Texture> world_textures;
		std::map<uint16_t, uint16_t> world_map;

		std::vector<Sprite> flag_sprites;
		LoginProcessNotice world_notice;
		Sprite step_channel;
		Gauge channel_gauge[Buttons::BtGoWorld - Buttons::BtChannel0];

		bool world_selected;

		/// 最近一次 SERVERSTATUS：0 正常 / 1 拥挤 / 2 满员（默认 0，未收到前不拦截）。
		int16_t world_status;
		/// 拥挤提示已弹过的世界槽位，-1 表示尚未提示；避免反复点同一世界刷屏。
		int16_t status_notified_world;

		/// draw_world 已根据当前列表定稿；下一轮 SERVERLIST 的首个世界到达时需先 clear。
		bool world_list_finalized = false;

		nl::node worldsrc;
		nl::node worldnamesrc;
		nl::node channelsrc;
	};
}
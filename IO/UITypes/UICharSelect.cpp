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
#include "UICharSelect.h"

#include <fstream>

#include "UILoginNotice.h"
#include "UIRaceSelect.h"
#include "UISoftKey.h"
#include "UIWorldSelect.h"

#include "../UI.h"
#include "../../Net/Packets/SelectCharPackets.h"

#include "../LoginBackground.h"
#include "../../Util/Locale.h"

#include "../Components/AreaButton.h"
#include "../Components/MapleButton.h"

#include "../../Configuration.h"

#include "../../Audio/Audio.h"
#include "../../Character/Job.h"
#include "../../Graphics/Geometry.h"

#include "../../Net/Packets/SelectCharPackets.h"

#include <SDL3/SDL.h>

#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// MapLogin CharSelect/signboard/0：右侧三层木牌，开始/创建/删除应贴在三块横板上。
	static const Point<int16_t> CS_SIGN_MAP_POS(253, -1258);
	// 木牌 133x238；按钮约 101x30；横板顶边相对木牌贴图的 Y（顶/中/底）。
	// 贴图内三板带实测：y 13..47 / 55..90 / 106..144（板3 微倾）。
	// 按钮为 30/35/43 高，按各板视觉中心取 Y：板1 中心 30 → 15；板2 中心 72 → 55；
	// 板3 顶边 106（旧值 142 = 板3 底边，导致「删除角色」整块沉到挂牌下方悬空，
	// 用户实测反馈位置不对；参考端三板顶边间距实测 ≈ 48/98px ⇒ 15/63/113，
	// 取板3 顶边 106 与板带起点一致）。
	static constexpr int16_t CS_SIGN_BTN_OFF_X = 16;
	// 木牌三横板：相对贴图顶边的按钮 Y（略下移使文字落在木条中带）。
	static constexpr int16_t CS_SIGN_PLANK_Y[3] = { 15, 55, 106 };

	// BeiDou CharSelect 无 pos/detailPos 节点；详情标签相对 charInfo1 面板（183×151）。
	// 对齐 charInfo1 位图内「职业/等级/人气/四维」空槽（非 Journey 旧版单列）。
	//
	// 值行坐标 = charInfo1 纹理相对坐标（纹理左上角经 get_origin() 补偿后
	// 精确落在 detail_pos）。2026-10-29 对本端渲染逐像素实测：
	// 烘焙标签行顶 texture-y = 3/21/39/57（pitch 18），值行须与标签同行；
	// 分隔行「服务器排行榜」texture-y=82，底部烘焙「排行榜」texture-y=112，
	// 排行文本取 93（居于两者之间、与参考端相对布局一致）。
	//   职业 (46,3)   等级 (46,21)  人气 (136,21)
	//   力量 (46,39)  智力 (136,39)
	//   敏捷 (46,57)  运气 (136,57)
	static constexpr Point<int16_t> CS_DETAIL_JOB(46, 3);
	static constexpr Point<int16_t> CS_DETAIL_LEVEL(46, 21);
	static constexpr Point<int16_t> CS_DETAIL_FAME(136, 21);
	static constexpr Point<int16_t> CS_DETAIL_STR(46, 39);
	static constexpr Point<int16_t> CS_DETAIL_INT(136, 39);
	static constexpr Point<int16_t> CS_DETAIL_DEX(46, 57);
	static constexpr Point<int16_t> CS_DETAIL_LUK(136, 57);
	// 参考端详情面板不绘制角色名（名字由角色脚下 nametag 显示）；保留默认点仅用于 NX 缺省判定。
	static constexpr Point<int16_t> CS_DETAIL_NAME(153, 1);
	static constexpr Point<int16_t> CS_DETAIL_RANK_NA(36, 93);
	static constexpr Point<int16_t> CS_DETAIL_RANK(115, 93);
	// charInfo1（183×151）相对脚点。2026-10-24 按正确端截图（选择角色页.png）
	// 重新实测：内容板（职业行）顶边在屏幕 (391,197)，槽 0 脚点 (485,428)
	// ⇒ 偏移 (-94,-231)。旧值 (-93,-208) 把整板上移 23px（此前误将卷轴顶边
	// 当作内容板顶边测量），表现为属性板偏高、与角色头部分离。
	static constexpr Point<int16_t> CS_CHARINFO_OFF(-94, -231);
	// 内容板在卷轴（CharSelect/scroll/2 末帧 217×193）内的偏移：卷轴顶边在屏幕
	// ≈158，内容板顶边 197 ⇒ 纵向 39；横向居中 (217-183)/2=17。
	static constexpr Point<int16_t> CS_SCROLL_CONTENT_OFF(17, 39);

	// 角色槽：占用与空槽共用同一桥面 Y。
	// 站位校准（依据桥面贴片像素实测，2026-09-25）：
	//   桥面视觉来自 MapLogin.img back/25（no=11，1280x2540 巨型底图），
	//   屏幕偏移 TL=(-98,-652)；桥板不透明带位于贴图 1020~1047 行
	//   （全覆盖率、木色带），即屏幕 y 368~396（板厚 28px）。
	//   脚点必须落在板**顶边** 368：position.y = viewpos.y(1508) + CS_FRAME_ORIGIN.y(-54)
	//   = 1454，故 slot_y = 368 - 1454 = -1086，
	//   DY = -1086 - (CS_LIST_T + CS_SLOT_INSET_BRIDGE) = -8。
	//   （旧值 DY=20 时脚点 396 = 桥板底边，角色整体陷进板里、视觉偏下。）
	static constexpr int16_t CS_SLOT_INSET_BRIDGE = 1348 - 1208;
	static constexpr int16_t CS_SLOT_INSET_EMPTY = 1348 - 1208;
	static constexpr int16_t CS_SLOT_INSET_OCCUPIED = 1384 - 1208;
	static constexpr int16_t CS_SLOT_BRIDGE_DY = -8;
	static constexpr int16_t CS_SLOT_W = 183;
	static constexpr int16_t CS_SLOT_H_EMPTY = 115;
	static constexpr int16_t CS_SLOT_H_OCCUPIED = 151;

	// 帧内坐标 -> 屏幕原点：position = LoginBackground::viewpos + 本常量。
	static const Point<int16_t> CS_FRAME_ORIGIN(-237, -54);

	// 角色列表容器（IDA sub_603FF0: CreateWnd(-290,-1218,600,80,z:10,0)）
	static constexpr int16_t CS_LIST_T = -1218;

	// 选中光（相对角色脚点 charpos；锚点依据 CharSelect/effect 贴图逐帧实测）：
	//   effect/1 光柱：各帧内容顶边均贴贴图第 0~1 行（origin.y=0），内容底边随帧
	//   下移（179→256→280→299→310）——即"光柱自上方垂落、逐帧下探"的动画。
	//   最终帧内容高 310，锚点取 charpos.y-310 使光柱底边恰落在脚点，
	//   顶边高于头顶（310px），符合"从角色上方往一定距离下"的正确观感。
	//   （旧值 y=0 时整根光柱从脚下向下挂到桥下，方向颠倒。）
	//   effect/0 身光：49x132，origin.y -205→-132 随帧上移。锚 (-5,-298)（上游标定）
	//   时首帧内容罩住身体（脚上 72 ~ 脚下 37），随后光环沿身体向上飘升。
	static constexpr Point<int16_t> CS_FX_FOOT(-5, -310);
	static constexpr Point<int16_t> CS_FX_BODY(-5, -298);

		UICharSelect::UICharSelect(std::vector<CharEntry> characters, int8_t characters_count, int32_t slots, int8_t require_pic) : UIElement(Point<int16_t>(0, 0), Point<int16_t>(800, 600)),
		characters(characters), characters_count(characters_count), slots(slots), require_pic(require_pic), tab_index(0), tab_active(false), tab_move(false), charslot_y(0), use_timestamp(false), burning_character(false), show_pic_btns(false)
	{
		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS);
		bind_text(version, "version.prefix", version_text);

		nl::node Login = nl::nx::UI["Login.img"];
		nl::node Common = Login["Common"];
		version_pos = Common["version"]["pos"];

		selected_character = Setting<DefaultCharacter>::get().load();
		page_count = static_cast<uint8_t>((slots + PAGESIZE - 1) / PAGESIZE);

		// 默认角色可能已被删除或超出当前角色数：先夹到实际存在的槽位，
		// 否则 BtSelect/BtDelete 会以 selected_character 直接索引 characters 越界。
		if (characters_count > 0)
		{
			if (selected_character >= static_cast<uint8_t>(characters_count))
				selected_character = static_cast<uint8_t>(characters_count - 1);
		}
		else
		{
			selected_character = 0;
		}

		selected_page = selected_character / PAGESIZE;

		if (page_count > 0 && selected_page >= page_count)
			selected_page = page_count - 1;

		tab_move_pos = 0;

		tab_map[0] = Buttons::BtSelect;
		tab_map[1] = Buttons::BtNew;
		tab_map[2] = Buttons::BtDelete;

		nl::node CharSelect = Login["CharSelect"];
		// v83 无 Cursor/18；勿绑数字字库帧。有独立 tab 图再用，否则不画指示。
		nl::node tab_node = CharSelect["tab"]["0"];
		if (!tab_node)
			tab_node = CharSelect["Tab"]["0"];
		if (tab_node)
			tab = tab_node;
		nl::node selectedWorld = CharSelect["selectedWorld"];
		nl::node pageNew = CharSelect["pageNew"];

		uint16_t world = 0;
		uint8_t world_id = Configuration::get().get_worldid();
		uint8_t channel_id = Configuration::get().get_channelid();

		if (auto worldselect = UI::get().get_element<UIWorldSelect>())
			world = worldselect->get_worldbyid(world_id);

		// 直接进入选角（自动登录/世界选择已销毁）时没有 worldselect 可映射；
		// 仍未初始化的 world 会以未定义索引访问 selectedWorld["icon"]。
		if (!selectedWorld["icon"][world])
			world = 0;

		world_sprites.emplace_back(Common["selectWorld"]);
		world_sprites.emplace_back(selectedWorld["icon"][world]);
		world_sprites.emplace_back(selectedWorld["name"][world]);
		world_sprites.emplace_back(selectedWorld["ch"][channel_id]);

		// BeiDou 下由 LoginBackground 统一绘制原生登录背景（地图 + 外框），
		// 这里使用 step=2 的角色选择视角，跳过 GMS 板式背景。
		bool bg_managed = LoginBackground::get().available();

		if (bg_managed)
		{
			LoginBackground::get().set_step(2);

			// 帧内坐标 -> 屏幕原点。由**已验证的选世界条幅**反推，不是猜：
			//   选世界槽位帧内 = CUIWorldSelect 容器 (−140, −804) 的 (0,0)；
			//   实测其屏幕位置 = (235, 110)（wordsrc_pos，见 verified_worldselect.png）
			//   ⇒ step=1 偏移 = (375, 914)；而 viewpos(step1) = (612, 968)
			//   ⇒ position = viewpos + (−237, −54)（对任意 step/分辨率自动跟随相机）。
			position = LoginBackground::get().get_target_viewpos() + CS_FRAME_ORIGIN;
		}
		else
		{
			// 兼容数据命名：BeiDou 的登录背景图名为 Back/login.img 而非 UI_login.img
			nl::node map = nl::nx::Map001["Back"]["UI_login.img"];
			if (!map)
				map = nl::nx::Map001["Back"]["login.img"];
			nl::node back = map["back"];

			sprites.emplace_back(back["1"], Point<int16_t>(512, 384));

			for (nl::node node : map["ani"])
				sprites.emplace_back(node, Point<int16_t>(0, -2));

			sprites.emplace_back(back["2"], Point<int16_t>(512, 384));

			// 无登录地图时退化为把 800x600 版心居中。
			position = Point<int16_t>(
				static_cast<int16_t>((Constants::Constants::get().get_viewwidth() - 800) / 2),
				static_cast<int16_t>((Constants::Constants::get().get_viewheight() - 600) / 2)
			);
		}

		nl::node BurningNotice = Common["Burning"]["BurningNotice"];
		burning_notice = BurningNotice;
		burning_numPos = BurningNotice["numPos"];
		burning_count = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::CREAM, "1");

		charinfo = CharSelect["charInfo1"];

		// 详情卷轴（展开动画）：Login.img/CharSelect/scroll/2 = 30→196→195→193 高。
		// scroll/2 为循环动画，Animation 无法定格 → 逐帧取 Frame 手动播放。
		if (nl::node scroll2 = CharSelect["scroll"]["2"])
		{
			for (const nl::node& f : scroll2)
			{
				Frame frame(f);

				if (frame.get_dimensions().x() > 0)
					charinfo_scroll_frames.push_back(frame);
			}

			charinfo_has_scroll = !charinfo_scroll_frames.empty();
		}

		// BeiDou：正确端使用含排行区的 charInfo1；无则退回 charInfo。
		if (LoginBackground::get().available() && !charinfo && CharSelect["charInfo"])
			charinfo = CharSelect["charInfo"];

		// nxcache 脏 origin：不再 shift 节点（实测 shift 无效，纹理仍偏移 ~7px），
		// 改为绘制时按 get_origin() 补偿（见 draw()：detail_pos + get_origin()），
		// 使底板左上角精确落在 detail_pos，值行常量得以用纹理相对坐标。

		nl::node charSlot = CharSelect["charSlot"];
		charslot = charSlot["0"];
		charslot_pos = charSlot["pos"];
		charslot_bounds = Rectangle<int16_t>(charSlot["worldInfoLT"], charSlot["worldInfoRB"]);

		pagebase = pageNew["base"]["0"];
		pagenumber = Charset(pageNew["number"], Charset::Alignment::LEFT);
		pagenumberpos = pageNew["numberpos"];
		pagepos = pageNew["pos"];

		signpost[0] = CharSelect["adventure"]["0"];
		signpost[1] = CharSelect["knight"]["0"];
		signpost[2] = CharSelect["aran"]["0"];

		nametag = CharSelect["nameTag"];

		nl::node BtNew = CharSelect["BtNew"];
		Texture BtNewTexture = Texture(BtNew["normal"]["0"]);
		Point<int16_t> BtNewDim = BtNewTexture.get_dimensions();
		BtNewPos = BtNewTexture.get_origin().abs();

		nl::node BtSelect = CharSelect["BtSelect"];
		nl::node BtDelete = CharSelect["BtDelete"];
		Point<int16_t> BtSelectPos = Texture(BtSelect["normal"]["0"]).get_origin().abs();

		Point<int16_t> tab_adj = Point<int16_t>(107, 8);

		tab_pos[0] = tab_adj + BtSelectPos + Point<int16_t>(2, 0);
		tab_pos[1] = tab_adj + BtNewPos;
		tab_pos[2] = tab_adj + BtNewPos + Point<int16_t>(BtNewDim.x() - 1, 0);

		// BeiDou：三按钮锚到 MapLogin CharSelect/signboard/0 三块横板（相对 position）。
		// 非 BeiDou：退回 IDA 按钮条 CreateWnd(200,-1359)+OnCreate(4,-2/36/85)。
		Point<int16_t> btn_select_local(200 + 4, -1359 - 2);
		Point<int16_t> btn_new_local(200 + 4, -1359 + 36);
		Point<int16_t> btn_delete_local(200 + 4, -1359 + 85);

		if (bg_managed)
		{
			Point<int16_t> viewpos = LoginBackground::get().get_target_viewpos();
			Texture sign_tex(nl::nx::Map["Obj"]["login.img"]["CharSelect"]["signboard"]["0"]["0"]);
			Point<int16_t> sign_tl = CS_SIGN_MAP_POS + viewpos - sign_tex.get_origin();

			auto plank_btn = [&](nl::node btn_node, int plank) -> Point<int16_t>
			{
				Texture tex(btn_node["normal"]["0"]);
				Point<int16_t> screen = sign_tl
					+ Point<int16_t>(CS_SIGN_BTN_OFF_X, CS_SIGN_PLANK_Y[plank])
					+ tex.get_origin();
				return screen - position;
			};

			btn_select_local = plank_btn(BtSelect, 0);
			btn_new_local = plank_btn(BtNew, 1);
			btn_delete_local = plank_btn(BtDelete, 2);
			BtNewPos = btn_new_local;
		}

		buttons[Buttons::BtSelect] = std::make_unique<MapleButton>(BtSelect, btn_select_local);
		buttons[Buttons::BtNew] = std::make_unique<MapleButton>(BtNew, btn_new_local);
		buttons[Buttons::BtDelete] = std::make_unique<MapleButton>(BtDelete, btn_delete_local);
		buttons[Buttons::BtPageL] = std::make_unique<MapleButton>(CharSelect["pageL"]);
		buttons[Buttons::BtPageR] = std::make_unique<MapleButton>(CharSelect["pageR"]);
		buttons[Buttons::BtChangePIC] = std::make_unique<MapleButton>(Common["BtChangePIC"]);
		buttons[Buttons::BtResetPIC] = std::make_unique<MapleButton>(Login["WorldSelect"]["BtResetPIC"]);
		buttons[Buttons::BtCharacter] = std::make_unique<MapleButton>(CharSelect["EditCharList"]["BtCharacter"]);
		buttons[Buttons::BtPreview] = std::make_unique<MapleButton>(Common["BtPreview"]);

		nl::node pos = CharSelect["pos"];
		nl::node avatarPos = pos["avatarPos"];
		avatarLT = avatarPos["avatarLT"];
		avatarRB = avatarPos["avatarRB"];
		avatarSpace = avatarPos["avatarSpace"];

		// BeiDou 的 CharSelect/pos/detailPos 节点坐标不可靠（实测会错位：职业/等级
		// 值位置跑偏、人气值串到等级位），一律改用参考端 CUICharDetail::Draw
		// (sub_602ADC) 实测的标签坐标表，确保与正确端截图一致。
		levelPos = CS_DETAIL_LEVEL;
		famePos = CS_DETAIL_FAME;
		namePos = CS_DETAIL_NAME;
		jobPos = CS_DETAIL_JOB;
		statSTRPos = CS_DETAIL_STR;
		statDEXPos = CS_DETAIL_DEX;
		statINTPos = CS_DETAIL_INT;
		statLUKPos = CS_DETAIL_LUK;

		// 命中区覆盖整页 3 槽（桥面统一 Y）；高度用占用槽外框。
		for (size_t i = 0; i < PAGESIZE; i++)
			buttons[Buttons::CHARACTER_SLOT0 + i] = std::make_unique<AreaButton>(
				get_character_slot_pos(i, true),
				Point<int16_t>(CS_SLOT_W, CS_SLOT_H_OCCUPIED));

		if (require_pic == 0 || !show_pic_btns)
		{
			buttons[Buttons::BtChangePIC]->set_active(false);
			buttons[Buttons::BtResetPIC]->set_active(false);
		}

		// BeiDou 的 CharSelect 无 lv/pageNew 节点（实测 NX 树：BtDelete/BtNew/
		// BtSelect/charInfo*/effect/nameTag/pageL/pageR/scroll/adventure/aran/
		// knight），逐级回退：lv → pageNew/number → StatusBar.img/number。
		// 无效的 Charset 会导致等级数字完全不绘制。
		nl::node lvset_node = CharSelect["lv"];

		if (!lvset_node)
			lvset_node = pageNew["number"];

		if (!lvset_node)
			lvset_node = nl::nx::UI["StatusBar.img"]["number"];

		levelset = Charset(lvset_node, Charset::Alignment::LEFT);
		namelabel = OutlinedText(Text::Font::A14B, Text::Alignment::CENTER, Color::Name::WHITE, Color::Name::IRISHCOFFEE);

		// 参考端 sub_602ADC 在 (46/136, 1/19/37/55) 以左对齐绘制文字；
		// charInfo1 值槽 x38-90、127-180，左对齐后数字落在槽内左侧。
		// 参考端属性值为纯深棕无描边；原 WHITE 描边在米色面板把字"洗白/发蓝"，
		// 改为与填充同色 TOBACCOBROWN 即等效无描边（item 3 颜色修复）。
		for (size_t i = 0; i < InfoLabel::NUM_LABELS; i++)
			infolabels[i] = OutlinedText(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOBACCOBROWN, Color::Name::TOBACCOBROWN);

		ranklabel = OutlinedText(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOBACCOBROWN, Color::Name::TOBACCOBROWN);
		// 开局先亮「取不到排行」，等有排行数据再由 update 路径改写（那条路是活文案）。
		bind_text(ranklabel, "charselect.rank_unavailable");

		for (CharEntry& entry : characters)
		{
			charlooks.emplace_back(entry.look);
			nametags.emplace_back(nametag, Text::Font::A12M, entry.stats.name);
		}

		emptyslot_effect = CharSelect["character"]["0"];
		emptyslot = CharSelect["character"]["1"]["0"];

		selectedslot_effect[0] = CharSelect["effect"][0];
		selectedslot_effect[1] = CharSelect["effect"][1];

		charslotlabel = OutlinedText(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::JAMBALAYA);
		charslotlabel.change_text(get_slot_text());

		update_buttons();

		if (characters_count > 0)
		{
			if (selected_character < characters_count)
				update_selected_character();
			else
				select_last_slot();
		}

		if (Configuration::get().get_auto_login())
		{
			// opt-in 自动进游戏：挂起，首帧调 button_pressed(BtSelect)（与手点「开始游戏」同一路径），
			// 避免构造期 UI 未就绪；require_pic==2 时直接 SelectCharPacket 进图。
			auto_enter_pending = true;
		}

	}

	void UICharSelect::draw(float inter) const
	{
		UIElement::draw_sprites(inter);

		version.draw(position + version_pos - Point<int16_t>(0, 5));

		// BeiDou 无 charSlot/pageNew 布局节点时贴图为空，跳过避免左上角堆叠。
		if (charslot.is_valid())
		{
			charslot.draw(position + charslot_pos - Point<int16_t>(0, charslot_y));
			charslotlabel.draw(position + charslot_pos + Point<int16_t>(113, 5 - charslot_y));
		}

		for (Sprite sprite : world_sprites)
			sprite.draw(position, inter);

		std::string total = pad_number_with_leading_zero(page_count);
		std::string current = pad_number_with_leading_zero(selected_page + 1);

		std::list<uint8_t> fliplist = { 3, 4, 5, 9, 10, 11 };

		for (uint8_t i = 0; i < PAGESIZE; i++)
		{
			uint8_t index = i + selected_page * PAGESIZE;
			// 按**绝对**角色索引判断镜像：原本该表是为“每页 12 槽、4 行”版式定的
			// （第 2/4 行镜像）。改为按绝对索引判断后，这些角色无论落在哪一页都保持既有朝向，
			// 不因每页槽位数变化而改变既有表现。
			bool flip_character = std::find(fliplist.begin(), fliplist.end(), index) != fliplist.end();
			bool selectedslot = index == selected_character;

			if (index < characters_count)
			{
				// 统一以 position 为唯一参照原点：槽位局部坐标 + position。
				Point<int16_t> charpos = position + get_character_slot_pos(i, true);
				DrawArgument chararg = DrawArgument(charpos, flip_character);

				nametags[index].draw(charpos + Point<int16_t>(2, 25));

				const StatsEntry& character_stats = characters[index].stats;

				// 选中光：先脚底光柱，再角色，后身光（避免光柱盖住全身 / 从页底升起）。
				if (selectedslot)
					selectedslot_effect[1].draw(charpos + CS_FX_FOOT, inter);

				uint8_t j = 0;
				uint16_t job = character_stats.stats[MapleStat::Id::JOB];

				if (job >= 0 && job < 1000)
					j = 0;
				else if (job >= 1000 && job < 2000)
					j = 1;
				else if (job >= 2000 && job < 2200)
					j = 2;
				else
					j = 0;

				signpost[j].draw(chararg);
				charlooks[index].draw(chararg, inter);

				if (selectedslot)
				{
					selectedslot_effect[0].draw(charpos + CS_FX_BODY, inter);

					// BeiDou：charInfo1 底板 + 数值；先垫不透明底再画贴图，避免透明缝透出森林。
					if (charinfo.is_valid())
					{
						Point<int16_t> detail_pos = charpos + CS_CHARINFO_OFF;

						// 卷轴：仅展开动画期间绘制（2026-10-29 参考图逐像素取证：
						// 静止状态原生只画 charInfo1 内容板，四周无卷轴外露——
						// 面板上方是书页底+气泡，左右是蘑菇屋/天空）。旧实现把
						// scroll_frame 常驻画在内容板后方且上露 39px，即用户所见
						// 「属性框相对卷轴偏下」。动画播完定格末帧后不再绘制。
						if (charinfo_has_scroll)
						{
							const Frame& scroll_frame = charinfo_scroll_frames[
								static_cast<size_t>(charinfo_scroll_playing ? charinfo_scroll_frame : charinfo_scroll_frames.size() - 1)
							];

							scroll_frame.draw(DrawArgument(detail_pos - CS_SCROLL_CONTENT_OFF));
						}

						if (!charinfo_has_scroll || !charinfo_scroll_playing)
						{
							Point<int16_t> ci_dim = charinfo.get_dimensions();
							// nxcache 脏 origin 补偿：纹理实际渲染位 = 参数 - origin，
							// 加回 origin 使底板左上角精确落在 detail_pos（值行常量
							// 由此可用纯纹理相对坐标，消除 ~7-11px 系统偏差）。
							Point<int16_t> ci_pos = detail_pos + charinfo.get_origin();
							ColorBox(ci_dim.x(), ci_dim.y(), Color::Name::CREAM, 1.0f).draw(ci_pos);
							charinfo.draw(ci_pos);

							std::string levelstr = std::to_string(character_stats.stats[MapleStat::Id::LEVEL]);
							// 职业/等级原本用 StatusBar.img/number 的 Charset 数字精灵，
							// 该节点在 BeiDou 部署数据里缺失/无效 → 白字画在米色面板不可见。
							// 改与同面板其他数值一致用 OutlinedText 文字绘制（参考端深棕文字）。
							OutlinedText lvltext(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOBACCOBROWN, Color::Name::TOBACCOBROWN);
							lvltext.change_text(levelstr);
							lvltext.draw(detail_pos + levelPos);

							// 参考端 CUICharDetail::Draw 只绘制职业/等级/人气/四维；
							// 角色名由角色脚下 nametag 显示，详情面板内不再重复绘制。
							for (size_t li = 0; li < InfoLabel::NUM_LABELS; li++)
							{
								Point<int16_t> labelpos = detail_pos + get_infolabel_pos(li);
								infolabels[li].draw(labelpos);
							}

							// 排行带：有 rank → 名次；否则「排行不可用」（对齐正确端截图 / sub_602ADC）。
							ranklabel.draw(detail_pos + (character_stats.has_rank ? CS_DETAIL_RANK : CS_DETAIL_RANK_NA));
						}
					}
					else if (LoginBackground::get().available())
					{
						std::string levelstr = std::to_string(character_stats.stats[MapleStat::Id::LEVEL]);
						OutlinedText lvltext(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::TOBACCOBROWN, Color::Name::TOBACCOBROWN);
						lvltext.change_text(levelstr);
						lvltext.draw(charpos + Point<int16_t>(0, -72));
					}
				}
			}
			else if (i < PAGESIZE)
			{
				// 一页固定画满 3 个影/台（即使服务端 slots < 3，也保证桥上三影观感）。
				Point<int16_t> emptyslotpos = position + get_character_slot_pos(i);

				emptyslot_effect.draw(emptyslotpos, inter);
				emptyslot.draw(DrawArgument(emptyslotpos, flip_character));
			}
		}

		UIElement::draw_buttons(inter);

		if (tab_active && tab.is_valid())
			tab.draw(position + tab_pos[tab_index] + Point<int16_t>(0, tab_move_pos));

		if (burning_character)
		{
			burning_notice.draw(position + BtNewPos, inter);
			burning_count.draw(position + BtNewPos + burning_numPos + Point<int16_t>(8, -4));
		}

		if (pagebase.is_valid())
		{
			pagebase.draw(position + pagepos);
			pagenumber.draw(current.substr(0, 1), position + pagepos + Point<int16_t>(pagenumberpos[0]));
			pagenumber.draw(current.substr(1, 1), position + pagepos + Point<int16_t>(pagenumberpos[1]));
			pagenumber.draw(total.substr(0, 1), position + pagepos + Point<int16_t>(pagenumberpos[2]));
			pagenumber.draw(total.substr(1, 1), position + pagepos + Point<int16_t>(pagenumberpos[3]));
		}
	}

	void UICharSelect::update()
	{
		UIElement::update();

		// 首帧触发挂起的自动进游戏（与手点「开始游戏」一致）。
		if (auto_enter_pending)
		{
			auto_enter_pending = false;
			button_pressed(Buttons::BtSelect);
		}

		int16_t timestep_max = CHARSLOT_Y_MAX * Constants::TIMESTEP;

		if (use_timestamp)
		{
			if (show_timestamp)
			{
				if (timestamp > 0)
				{
					timestamp -= Constants::TIMESTEP;

					if (timestamp <= timestep_max)
						charslot_y += 1;
				}
			}
			else
			{
				if (timestamp <= timestep_max)
				{
					timestamp += Constants::TIMESTEP;

					if (charslot_y >= 0)
						charslot_y -= 1;
				}
			}
		}

		if (tab_move && tab_move_pos < 4)
			tab_move_pos += 1;

		if (tab_move && tab_move_pos == 4)
			tab_move = false;

		if (!tab_move && tab_move_pos > 0)
			tab_move_pos -= 1;

		for (CharLook& charlook : charlooks)
			charlook.update(Constants::TIMESTEP);

		for (Animation& effect : selectedslot_effect)
			effect.update();

		emptyslot_effect.update();

		// 详情卷轴展开推进（手动帧计时）：播完定格末帧，不再循环
		// （scroll/2 是循环动画，Animation::update 永不 true → 旧实现闪烁）。
		if (charinfo_scroll_playing)
		{
			charinfo_scroll_elapsed = static_cast<uint16_t>(charinfo_scroll_elapsed + Constants::TIMESTEP);

			while (charinfo_scroll_playing)
			{
				uint16_t delay = charinfo_scroll_frames[static_cast<size_t>(charinfo_scroll_frame)].get_delay();

				if (charinfo_scroll_elapsed < delay)
					break;

				charinfo_scroll_elapsed = static_cast<uint16_t>(charinfo_scroll_elapsed - delay);
				charinfo_scroll_frame++;

				if (charinfo_scroll_frame >= static_cast<int16_t>(charinfo_scroll_frames.size()))
				{
					charinfo_scroll_frame = static_cast<int16_t>(charinfo_scroll_frames.size() - 1);
					charinfo_scroll_playing = false;
					break;
				}
			}
		}

		if (burning_character)
			burning_notice.update();
	}

	void UICharSelect::doubleclick(Point<int16_t> cursorpos)
	{
		// 双击任意角色槽位：先选中该槽位（若尚未选中），再走「开始游戏」路径。
		// 旧实现只认「当前已选中槽位」的按钮状态，双击另一个角色时无效果
		// （用户实测反馈「双击角色不能直接进入游戏」）。
		for (uint16_t slot = 0; slot < PAGESIZE; slot++)
		{
			uint16_t button_index = Buttons::CHARACTER_SLOT0 + slot;
			auto& btit = buttons[button_index];

			// 命中判定：优先 AreaButton（与单击一致），兜底用覆盖整身的命中框。
			// 角色以脚点 charpos 绘制、身体在脚点上方，而 AreaButton 框在脚点下方
			// 空地 → 点角色身体会漏掉 AreaButton，故用大框覆盖头到脚，确保双击
			// 「点角色」必命中（无论角色是脚锚点还是顶锚点）。
			bool hit = btit && btit->is_active()
				&& btit->bounds(position).contains(cursorpos);

			if (!hit)
			{
				Point<int16_t> charpos = position + get_character_slot_pos(slot, true);
				Rectangle<int16_t> body_rect(
					charpos.x() - 25, charpos.y() - 220, charpos.x() + 158, charpos.y() + 30);

				hit = body_rect.contains(cursorpos);
			}

			if (hit)
			{
				int16_t abs_slot = static_cast<int16_t>(selected_page * PAGESIZE + slot);

				if (abs_slot < characters_count)
				{
					if (selected_character != abs_slot)
					{
						selected_character = abs_slot;
						update_selected_character();
						update_buttons();
					}

					button_pressed(Buttons::BtSelect);
				}

				return;
			}
		}
	}

	Cursor::State UICharSelect::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (charslot_bounds.contains(cursorpos))
		{
			if (clicked)
			{
				if (use_timestamp)
					show_timestamp = !show_timestamp;

				return Cursor::State::CLICKING;
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
				else if (btit.second->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));

						if (tab_active && btit.first == tab_map[tab_index])
							btit.second->set_state(Button::State::MOUSEOVER);

						ret = Cursor::State::IDLE;
					}
					else
					{
						if (!tab_active || btit.first != tab_map[tab_index])
							ret = Cursor::State::CANCLICK;
					}
				}
			}
			else if (btit.second->get_state() == Button::State::MOUSEOVER)
			{
				if (!tab_active || btit.first != tab_map[tab_index])
					btit.second->set_state(Button::State::NORMAL);
			}
		}

		return ret;
	}

	void UICharSelect::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				button_pressed(Buttons::BtPreview);
			}
			else if (keycode == KeyAction::Id::RETURN)
			{
				if (tab_active)
				{
					uint16_t btn_index = tab_map[tab_index];

					auto& btn = buttons[btn_index];
					Button::State state = btn->get_state();

					if (state != Button::State::DISABLED)
						button_pressed(btn_index);
				}
				else
				{
					button_pressed(Buttons::BtSelect);
				}
			}
			else
			{
				if (keycode == KeyAction::Id::TAB)
				{
					uint8_t prev_tab = tab_index;

					if (!tab_active)
					{
						tab_active = true;

						if (!buttons[Buttons::BtSelect]->is_active())
							tab_index++;
					}
					else
					{
						tab_index++;

						if (tab_index > 2)
						{
							tab_active = false;
							tab_index = 0;
						}
					}

					tab_move = true;
					tab_move_pos = 0;

					auto& prev_btn = buttons[tab_map[prev_tab]];
					Button::State prev_state = prev_btn->get_state();

					if (prev_state != Button::State::DISABLED)
						prev_btn->set_state(Button::State::NORMAL);

					if (tab_active)
					{
						auto& btn = buttons[tab_map[tab_index]];
						Button::State state = btn->get_state();

						if (state != Button::State::DISABLED)
							btn->set_state(Button::State::MOUSEOVER);
					}
				}
				else
				{
					uint8_t selected_index = selected_character;
					uint8_t index_total = std::min(characters_count, static_cast<int8_t>((selected_page + 1) * PAGESIZE));

					// 键盘导航的“列数”= 每行槽位数：参考版为一行 3 槽（x = 130*(i%3) - 220），
					// 与 PAGESIZE / SLOTS_PER_ROW 保持一致。
					uint8_t COLUMNS = 3;
					uint8_t columns = std::min(index_total, COLUMNS);

					uint8_t rows = std::floor((index_total - 1) / COLUMNS) + 1;

					int32_t current_col = 0;

					if (columns > 0)
					{
						div_t div = std::div(selected_index, columns);
						current_col = div.rem;
					}

					if (keycode == KeyAction::Id::UP)
					{
						uint8_t next_index = (selected_index - COLUMNS < 0 ? (selected_index - COLUMNS) + rows * COLUMNS : selected_index - COLUMNS);

						if (next_index == selected_character)
							return;

						if (next_index >= index_total)
							button_pressed(next_index - COLUMNS + Buttons::CHARACTER_SLOT0);
						else
							button_pressed(next_index + Buttons::CHARACTER_SLOT0);
					}
					else if (keycode == KeyAction::Id::DOWN)
					{
						uint8_t next_index = (selected_index + COLUMNS >= index_total ? current_col : selected_index + COLUMNS);

						if (next_index == selected_character)
							return;

						if (next_index > index_total)
							button_pressed(next_index + COLUMNS + Buttons::CHARACTER_SLOT0);
						else
							button_pressed(next_index + Buttons::CHARACTER_SLOT0);
					}
					else if (keycode == KeyAction::Id::LEFT)
					{
						if (selected_index != 0)
						{
							selected_index--;

							if (selected_index >= (selected_page + 1) * PAGESIZE - PAGESIZE)
								button_pressed(selected_index + Buttons::CHARACTER_SLOT0);
							else
								button_pressed(Buttons::BtPageL);
						}
					}
					else if (keycode == KeyAction::Id::RIGHT)
					{
						if (selected_index != characters_count - 1)
						{
							selected_index++;

							if (selected_index < index_total)
								button_pressed(selected_index + Buttons::CHARACTER_SLOT0);
							else
								button_pressed(Buttons::BtPageR);
						}
					}
					else if (keycode == 'S' || keycode == 's' || keycode == 'N' || keycode == 'n')
					{
						UI::get().emplace<UISoftKey>([this](const std::string& s) {
							try {
								int idx = std::stoi(s);
								if (idx < 0 || idx >= characters_count) return;
								button_pressed(static_cast<uint16_t>(idx + Buttons::CHARACTER_SLOT0));
								LOG(LOG_DEBUG, "UICharSelect: SoftKey slot=" << idx);
							} catch (...) {}
						}, []() {}, "Char slot #");
					}
					else if (keycode == 'C' || keycode == 'c')
					{
						button_pressed(Buttons::BtNew);
						LOG(LOG_DEBUG, "UICharSelect: SoftKey create");
					}
					else if (keycode == 'D' || keycode == 'd')
					{
						button_pressed(Buttons::BtDelete);
						LOG(LOG_DEBUG, "UICharSelect: SoftKey delete");
					}
				}
			}
		}
	}

	UIElement::Type UICharSelect::get_type() const
	{
		return TYPE;
	}

	void UICharSelect::add_character(CharEntry&& character)
	{
		charlooks.emplace_back(character.look);
		nametags.emplace_back(nametag, Text::Font::A13M, character.stats.name);
		characters.emplace_back(std::forward<CharEntry>(character));

		characters_count++;
	}

	void UICharSelect::post_add_character()
	{
		bool page_matches = (characters_count - 1) / PAGESIZE == selected_page;

		if (!page_matches)
			button_pressed(Buttons::BtPageR);

		update_buttons();

		if (characters_count > 1)
			select_last_slot();
		else
			update_selected_character();

		makeactive();

		charslotlabel.change_text(get_slot_text());
	}

	void UICharSelect::remove_character(int32_t id)
	{
		for (size_t i = 0; i < characters.size(); i++)
		{
			if (characters[i].id == id)
			{
				charlooks.erase(charlooks.begin() + i);
				nametags.erase(nametags.begin() + i);
				characters.erase(characters.begin() + i);

				characters_count--;

				charslotlabel.change_text(get_slot_text());

				if (selected_page > 0)
				{
					bool page_matches = (characters_count - 1) / PAGESIZE == selected_page;

					if (!page_matches)
						button_pressed(Buttons::BtPageL);
				}

				update_buttons();

				if (selected_character < characters_count)
					update_selected_character();
				else
					select_last_slot();

				return;
			}
		}
	}

	void UICharSelect::clear_pending()
	{
		request_pending = false;
	}

	const CharEntry& UICharSelect::get_character(int32_t id)
	{
		for (CharEntry& character : characters)
			if (character.id == id)
				return character;

		LOG(LOG_DEBUG, "Invalid character id: [" << id << "]");

		static const CharEntry null_character = { {}, {}, 0 };

		return null_character;
	}

	Button::State UICharSelect::button_pressed(uint16_t buttonid)
	{
		if (request_pending)
			return Button::State::NORMAL;

		switch (buttonid)
		{
			case Buttons::BtSelect:
			{
				if (selected_character < static_cast<uint8_t>(characters_count)
					&& static_cast<size_t>(selected_character) < characters.size())
				{
					Setting<DefaultCharacter>::get().save(selected_character);
					int32_t id = characters[selected_character].id;

					switch (require_pic)
					{
						case 0:
						{
							std::function<void()> onok = [&]()
							{
								request_pic();
							};

							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::PIC_REQ, onok);
							break;
						}
						case 1:
						{
							std::function<void(const std::string&)> onok = [this, id](const std::string& pic)
							{
								request_pending = true;
								SelectCharPicPacket(pic, id).dispatch();
							};

							UI::get().emplace<UISoftKey>(onok);
							break;
						}
						case 2:
						{
							request_pending = true;
							SelectCharPacket(id).dispatch();
							break;
						}
					}
				}

				break;
			}
			case Buttons::BtNew:
			{
				Sound(Sound::Name::SCROLLUP).play();

				deactivate();

				tab_index = 0;
				tab_active = false;
				tab_move = false;
				tab_move_pos = 0;

				UI::get().emplace<UIRaceSelect>();
				break;
			}
			case Buttons::BtDelete:
			{
				if (selected_character >= static_cast<uint8_t>(characters_count)
					|| static_cast<size_t>(selected_character) >= characters.size())
					break;

				int32_t id = characters[selected_character].id;

				switch (require_pic)
				{
					case 0:
					{
						std::function<void()> onok = [&]()
						{
							charslotlabel.change_text(get_slot_text());
						};

						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::CHAR_DEL_FAIL_NO_PIC, onok);
						break;
					}
					case 1:
					{
						std::function<void()> oncancel = [&]()
						{
							charslotlabel.change_text(get_slot_text());
						};

						std::function<void()> onok = [&, id, oncancel]()
						{
							std::function<void(const std::string&)> onok = [&, id](const std::string& pic)
							{
								request_pending = true;
								DeleteCharPicPacket(pic, id).dispatch();
							};

							UI::get().emplace<UISoftKey>(onok, oncancel);
						};

						const StatsEntry& character_stats = characters[selected_character].stats;
						uint16_t cjob = character_stats.stats[MapleStat::Id::JOB];

						if (cjob < 1000)
							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::DELETE_CONFIRMATION, onok, oncancel);
						else
							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::CASH_ITEMS_CONFIRM_DELETION, onok, oncancel);

						break;
					}
					case 2:
					{
						request_pending = true;
						DeleteCharPacket(id).dispatch();
						break;
					}
				}

				break;
			}
			case Buttons::BtPageL:
			{
				uint8_t previous_page = selected_page;

				if (selected_page > 0)
					selected_page--;
				else
					selected_page = page_count - 1;

				if (previous_page != selected_page)
					update_buttons();

				select_last_slot();
				break;
			}
			case Buttons::BtPageR:
			{
				uint8_t previous_page = selected_page;

				if (selected_page < page_count - 1)
					selected_page++;
				else
					selected_page = 0;

				if (previous_page != selected_page)
				{
					update_buttons();

					button_pressed(Buttons::CHARACTER_SLOT0);
				}

				break;
			}
			case Buttons::BtChangePIC:
			{
				// 修改 PIC：复用 request_pic()（两次软键盘输入新 PIC 比对后发
				// REGISTER_PIC，含硬件信息）。v83 注册/修改 PIC 共用 REGISTER_PIC(29)，
				// 故无需新增独立 UpdatePicPacket。
				// 注意：s9 的 RegisterPicHandler 仅支持首次注册（已设 PIC 时直接断会话），
				// 客户端在此完成功能闭环（窗口 + 发包）；服务端是否支持「修改」取决于
				// s9 配置（enable_pic=true 且需扩展 handler）。见 r10 notes task4。
				if (static_cast<size_t>(selected_character) < characters.size())
					request_pic();

				break;
			}
			case Buttons::BtResetPIC:
			{
				std::string url = Configuration::get().get_resetpic();

				SDL_OpenURL(url.c_str());
				break;
			}
			case Buttons::BtCharacter:
			{
				// 查看全部角色：发 VIEW_ALL_CHAR(13) 请求，服务端回
				// showAllCharacter/showAllCharacterInfo，由 ViewAllCharHandler
				// 填充 UIStateLogin::viewall_worlds 并 emplace<UIViewAllChar>。
				ViewAllCharPacket().dispatch();
				break;
			}
			case Buttons::BtPreview:
			{
				deactivate();

				Sound(Sound::Name::SCROLLUP).play();

				// 参考端回到 step 1：背景先滚回选世界页，窗口在补间结束后再显示。
				if (LoginBackground::get().available())
					LoginBackground::get().set_step(1);

				UI::get().activate_after_transition(UIElement::Type::WORLDSELECT);

				break;
			}
			default:
			{
				if (buttonid >= Buttons::CHARACTER_SLOT0)
				{
					uint8_t previous_character = selected_character;
					selected_character = buttonid - Buttons::CHARACTER_SLOT0 + selected_page * PAGESIZE;

					if (previous_character != selected_character)
					{
						if (previous_character < characters_count)
						{
							charlooks[previous_character].set_stance(Stance::Id::STAND1);
							nametags[previous_character].set_selected(false);
						}

						if (selected_character < characters_count)
							update_selected_character();
					}
				}

				break;
			}
		}

		return Button::State::NORMAL;
	}

	void UICharSelect::update_buttons()
	{
		for (uint8_t i = 0; i < PAGESIZE; i++)
		{
			uint8_t index = i + selected_page * PAGESIZE;

			if (index < characters_count)
				buttons[Buttons::CHARACTER_SLOT0 + i]->set_state(Button::State::NORMAL);
			else
				buttons[Buttons::CHARACTER_SLOT0 + i]->set_state(Button::State::DISABLED);
		}

		if (characters_count >= slots)
			buttons[Buttons::BtNew]->set_state(Button::State::DISABLED);
		else
			buttons[Buttons::BtNew]->set_state(Button::State::NORMAL);

		bool character_found = false;

		for (int8_t i = PAGESIZE - 1; i >= 0; i--)
		{
			uint8_t index = i + selected_page * PAGESIZE;

			if (index < characters_count)
			{
				character_found = true;

				break;
			}
		}

		buttons[Buttons::BtSelect]->set_active(character_found);
		buttons[Buttons::BtDelete]->set_state(character_found ? Button::State::NORMAL : Button::State::DISABLED);
	}

	void UICharSelect::update_selected_character()
	{
		Sound(Sound::Name::CHARSELECT).play();

		// 详情卷轴展开（参考端选中角色后卷轴自顶部展开）。
		// 仅在选中项变化时重播：同一角色的重复调用（如 Charlist 重发触发
		// post_add_character）不再重置动画，否则面板会一直闪烁（用户实测反馈）。
		const int16_t detail_key = static_cast<int16_t>(selected_page * PAGESIZE + selected_character);
		if (charinfo_has_scroll && detail_key != last_detail_key)
		{
			last_detail_key = detail_key;
			charinfo_scroll_frame = 0;
			charinfo_scroll_elapsed = 0;
			charinfo_scroll_playing = true;
		}

		if (charlooks.size() > selected_character)
		{
			charlooks[selected_character].set_stance(Stance::Id::WALK1);
			nametags[selected_character].set_selected(true);

			const StatsEntry& character_stats = characters[selected_character].stats;

			namelabel.change_text(character_stats.name);

			for (size_t i = 0; i < InfoLabel::NUM_LABELS; i++)
				infolabels[i].change_text(get_infolabel(i, character_stats));

			if (character_stats.has_rank)
			{
				const std::string rank = std::to_string(character_stats.rank.first);

				// 升降箭头是「数字 + 后缀」，后缀连空格一起进模板：加载器会 trim 掉值的
				// 两端空白，单独存 " up" 只会得到 "up"，所以占位符必须写在模板里。
				if (character_stats.rank.second == '+')
					ranklabel.change_text(Locale::format("charselect.rank_up", rank));
				else if (character_stats.rank.second == '-')
					ranklabel.change_text(Locale::format("charselect.rank_down", rank));
				else
					ranklabel.change_text(rank);
			}
			else
			{
				ranklabel.change_text(Locale::text("charselect.rank_unavailable"));
			}
		}
	}

	void UICharSelect::select_last_slot()
	{
		for (int8_t i = PAGESIZE - 1; i >= 0; i--)
		{
			uint8_t index = i + selected_page * PAGESIZE;

			if (index < characters_count)
			{
				button_pressed(i + Buttons::CHARACTER_SLOT0);

				return;
			}
		}
	}

	std::string UICharSelect::get_slot_text()
	{
		if (use_timestamp)
		{
			show_timestamp = true;
			timestamp = 7 * 1000;
			charslot_y = 0;
		}

		return pad_number_with_leading_zero(characters_count) + "/" + pad_number_with_leading_zero(slots);
	}

	std::string UICharSelect::pad_number_with_leading_zero(uint8_t value) const
	{
		std::string return_val = std::to_string(value);
		return_val.insert(return_val.begin(), 2 - return_val.length(), '0');

		return return_val;
	}

	Point<int16_t> UICharSelect::get_character_slot_pos(size_t index, bool occupied) const
	{
		// 北斗 gms083 版式：每行 **3** 个槽位、横向间距 **130**
		// （依据 CUICharDetail ctor 0x602960：x = 130 * (slot % 3) - 220，
		//  相对间隔即 130；本实现原为每行 6 个，与北斗不符）。
		static constexpr int16_t SLOTS_PER_ROW = 3;
		static constexpr int16_t SLOT_STEP_X = 130;
		// 画布 → 帧的**横向刚性平移量**：帧内 x = 画布 x + CS_CANVAS_TO_FRAME_X。
		//
		// 依据链：
		//   1) CUICharDetail ctor(0x60292F) 的 `x = 130 * (i % 3) - 220` 与 y 同处
		//      **登录画布坐标系**（CreateFadeWnd 的第 6 参 bScreenCoord = 0）。
		//   2) 纵向可借 GetStepHeight(2) = -8-600*2 = -1208 推出带内偏移；
		//      横向**无法**从现有 dump 反推画布原点的绝对位置：
		//        - CLogin::Init(0x5F42CD) 不直接创建任何窗口（无 CreateWnd/CreateDlg 调用）；
		//        - CWndMan::UpdateWindowPosition(0x9E03A6) 仅重排窗口 z 序，不含坐标换算；
		//        - CWndMan::GetOrgWindow(0x48BBA5, UIOrigin) 在登录链路中未见调用点。
		//   3) 故改用实测反推：i = 0/1/2 三个槽位的「按帧内 -220 系列预测」与实测落点
		//      差值**恒为 +140**（295/425/555 对 155/285/415），说明两坐标系之间是
		//      **刚性平移**而非缩放或非线性，因此可安全固化为本常量。
		static constexpr int16_t CS_CANVAS_TO_FRAME_X = 330;
		static constexpr int16_t SLOT_X0 = -220 + CS_CANVAS_TO_FRAME_X;
		// 占用/空槽共用桥面 Y：CS_LIST_T + 空槽 inset + DY（正值下移到红框桥面）。
		(void)occupied;
		const int16_t slot_y = static_cast<int16_t>(CS_LIST_T + CS_SLOT_INSET_BRIDGE + CS_SLOT_BRIDGE_DY);
		return Point<int16_t>(
			static_cast<int16_t>(SLOT_X0 + SLOT_STEP_X * (index % SLOTS_PER_ROW)),
			slot_y
		);
	}

	Point<int16_t> UICharSelect::get_infolabel_pos(size_t index) const
	{
		switch (index)
		{
			case InfoLabel::JOB:
				return jobPos;
			case InfoLabel::FAME:
				return famePos;
			case InfoLabel::STR:
				return statSTRPos;
			case InfoLabel::DEX:
				return statDEXPos;
			case InfoLabel::INT:
				return statINTPos;
			case InfoLabel::LUK:
				return statLUKPos;
			case InfoLabel::NUM_LABELS:
			default:
				break;
		}

		return Point<int16_t>();
	}

	std::string UICharSelect::get_infolabel(size_t index, StatsEntry character_stats) const
	{
		switch (index)
		{
			case InfoLabel::JOB:
				return Job(character_stats.stats[MapleStat::Id::JOB]).get_name();
			case InfoLabel::FAME:
				return std::to_string(character_stats.stats[MapleStat::Id::FAME]);
			case InfoLabel::STR:
				return std::to_string(character_stats.stats[MapleStat::Id::STR]);
			case InfoLabel::DEX:
				return std::to_string(character_stats.stats[MapleStat::Id::DEX]);
			case InfoLabel::INT:
				return std::to_string(character_stats.stats[MapleStat::Id::INT]);
			case InfoLabel::LUK:
				return std::to_string(character_stats.stats[MapleStat::Id::LUK]);
			case InfoLabel::NUM_LABELS:
				break;
			default:
				break;
		}

		return "";
	}

	void UICharSelect::request_pic()
	{
		std::function<void(const std::string&)> enterpic = [&](const std::string& entered_pic)
		{
			std::function<void(const std::string&)> verifypic = [&, entered_pic](const std::string& verify_pic)
			{
				if (entered_pic == verify_pic)
				{
					RegisterPicPacket(
						characters[selected_character].id,
						entered_pic
					).dispatch();
				}
				else
				{
					std::function<void()> onreturn = [&]()
					{
						request_pic();
					};

					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::PASSWORD_IS_INCORRECT, onreturn);
				}
			};

			UI::get().emplace<UISoftKey>(verifypic, []() {}, "Please re-enter your new PIC.", Point<int16_t>(24, 0));
		};

		UI::get().emplace<UISoftKey>(enterpic, []() {}, "Your new PIC must at least be 6 characters long.", Point<int16_t>(24, 0));
	}
}
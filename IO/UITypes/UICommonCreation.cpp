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
#include "UICommonCreation.h"

#include "UICharSelect.h"
#include "UILoginNotice.h"
#include "UIRaceSelect.h"
#include "UISoftKey.h"
#include "UIWorldSelect.h"

#include "../UI.h"
#include "../LoginBackground.h"

#include "../Components/AreaButton.h"
#include "../Components/LocalTwoSpriteButton.h"
#include "../Components/MapleButton.h"

#include "../../Configuration.h"
#include "../../Constants.h"

#include "../../Audio/Audio.h"
#include "../../Data/ItemData.h"

#include "../../Net/Packets/CharCreationPackets.h"
#include "../../Net/Packets/SelectCharPackets.h"
#include "../../Util/LoginUiAssets.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <algorithm>
#include <cctype>

namespace ms
{
	UICommonCreation::UICommonCreation(const std::string& c) : UIElement(Point<int16_t>(0, 0), Point<int16_t>(800, 600)),
		classic_mode(false), classic_look_stage(false), classic_creation_sent(false), classic_dice_frame(0),
		classic_dice_elapsed(0), classic_dice_playing(false),
		classType(c), className(class_map[c]), genderSelected(false), charLookSelected(false),
		charNameSelected(false), newCharNameDisabled(true), female(false), choosableGender(true),
		vCanvasCount(0), genderTopHeight(0), boardMidHeight(0), hairSelectSize(0),
		faceIndex(0), hairStyleIndex(0), hairColorEnabled(false), hairColorIndex(0), skinColorIndex(0),
		topIndex(0), capeEnabled(false), capeIndex(0), bottomEnabled(false), bottomIndex(0),
		shoeIndex(0), weaponIndex(0)
	{
		nl::node Login = nl::nx::UI["Login.img"];
		nl::node Common = Login["Common"];

		if (LoginUiAssets::use_classic_creation())
		{
			classic_mode = true;
			load_classic_char_look();
			init_classic(Login, Common);
			LoginUiAssets::log_info(std::string("[Creation] mode=classic class=") + classType);
		}
		else
		{
			classic_mode = false;
			load_char_look();
			init_modern(Login, Common);
			LoginUiAssets::log_info(std::string("[Creation] mode=modern class=") + classType);
		}
	}

	void UICommonCreation::init_modern(nl::node Login, nl::node Common)
	{
		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::LEMONGRASS);
		bind_text(version, "version.prefix", version_text);

		version_pos = Common["version"]["pos"];

		nl::node createLimit = Login["NewChar"]["createLimit"];
		nl::node CustomizeChar = Login["CustomizeChar"][classType];
		nl::node board = CustomizeChar["board"];
		nl::node genderSelect = CustomizeChar["genderSelect"];
		nl::node CustomizeCharInfo = CustomizeChar["info"];

		avatarOrigin = CustomizeCharInfo["avatarOrigin"];
		Point<int16_t> avatarSize = CustomizeCharInfo["avatarSize"];
		charNameOrigin = CustomizeCharInfo["charNameOrigin"];
		Point<int16_t> genderFemale = CustomizeCharInfo["genderFemale"];
		Point<int16_t> genderMale = CustomizeCharInfo["genderMale"];
		int16_t genderMarginX = CustomizeCharInfo["genderMarginX"];
		int16_t genderMarginY = CustomizeCharInfo["genderMarginY"];
		genderOrigin = CustomizeCharInfo["genderOrigin"];
		Point<int16_t> genderSize = CustomizeCharInfo["genderSize"];
		Point<int16_t> nameEditCtrlPos = CustomizeCharInfo["nameEditCtrlPos"];
		std::string nameEditFontColor = CustomizeCharInfo["nameEditFontColor"];
		newAvatarPos = CustomizeCharInfo["newAvatarPos"];
		Point<int16_t> offsetForElemBack = CustomizeCharInfo["offsetForElemBack"];
		vCanvasCount = CustomizeCharInfo["vCanvasCount"];

		if (vCanvasCount == 0)
		{
			if (className == "adventurer")
				vCanvasCount = 6;
			else if (className == "cygnusKnight")
				vCanvasCount = 7;
		}

		nl::node CustomizeCharObj = nl::nx::Map["Obj"]["login.img"]["CustomizeChar"][class_map[classType]];

		int16_t genderHeight = board["genderHeight"];
		int16_t avatarHeight = board["avatarHeight"];

		nl::node genderTop = board["genderTop"];
		genderTopHeight = Texture(genderTop).height();

		nl::node boardBottom = board["boardBottom"];
		int16_t boardBottomHeight = Texture(boardBottom).height();

		nl::node boardMid = board["boardMid"];
		boardMidHeight = Texture(boardMid).height();

		sprites_background.emplace_back(CustomizeCharObj["0"], Point<int16_t>(512, 384));

		sprites.emplace_back(createLimit["backgrnd"]);

		if (className == "adventurer" || className == "cygnusKnight")
			sprites_gender_select.emplace_back(genderTop, genderOrigin);
		else
			sprites_gender_select.emplace_back(genderTop, genderOrigin - Point<int16_t>(1, 0));

		if (className == "cygnusKnight")
			sprites_gender_select.emplace_back(boardMid, genderOrigin - Point<int16_t>(0, 112));
		else
			sprites_gender_select.emplace_back(boardMid, genderOrigin);

		if (className == "cygnusKnight")
			sprites_gender_select.emplace_back(boardBottom, genderOrigin + Point<int16_t>(0, genderHeight - boardBottomHeight) - Point<int16_t>(0, 238));
		else
			sprites_gender_select.emplace_back(boardBottom, genderOrigin + Point<int16_t>(0, genderHeight - boardBottomHeight));

		if (className == "adventurer")
			sprites_lookboard.emplace_back(board["avatarTop"], genderOrigin);
		else
			sprites_lookboard.emplace_back(board["avatarTop"], genderOrigin + Point<int16_t>(1, 0));

		sprites_lookboard.emplace_back(boardMid, genderOrigin);

		if (className == "cygnusKnight")
			sprites_lookboard.emplace_back(boardBottom, genderOrigin);
		else
			sprites_lookboard.emplace_back(boardBottom, genderOrigin + Point<int16_t>(0, avatarHeight - boardBottomHeight));

		nl::node avatarSel = CustomizeChar["avatarSel"];

		Point<int16_t> avatarSelAdj = Point<int16_t>(0, 0);

		if (className == "adventurer")
			avatarSelAdj = Point<int16_t>(44, 108);
		else if (className == "aran")
			avatarSelAdj = Point<int16_t>(36, 92);
		else if (className == "cygnusKnight")
			avatarSelAdj = Point<int16_t>(0, 18);
		else
			avatarSelAdj = Point<int16_t>(0, 0);

		if (className == "cygnusKnight")
			sprites_char_look.emplace_back(avatarSel["WZ2_skin"], avatarOrigin - Point<int16_t>(0, 18 * 3));
		else
			sprites_char_look.emplace_back(avatarSel["WZ2_skin"], avatarOrigin + avatarSelAdj);

		// avatarSel 可能为空（资源裁剪 / 节点缺失）：size()-1 是无符号下溢，
		// 会变成 SIZE_MAX 让构造期挂在死循环里。i + 1 < size() 在
		// size>=1 时与原式逐字等价（同样跳过最后一个子节点），size==0 时 0 次迭代。
		for (size_t i = 0; i + 1 < avatarSel.size(); i++)
		{
			size_t t = i;

			if (className == "adventurer" && i >= 2)
				t++;

			if (className == "cygnusKnight")
			{
				if (i == 2)
					sprites_char_look.emplace_back(avatarSel[i]["normal"], avatarOrigin + avatarSelAdj - Point<int16_t>(0, 16));
				else if (i > 2)
					sprites_char_look.emplace_back(avatarSel[i]["normal"], avatarOrigin + avatarSelAdj - Point<int16_t>(0, 18));
				else
					sprites_char_look.emplace_back(avatarSel[i]["normal"], avatarOrigin + avatarSelAdj);
			}
			else
			{
				sprites_char_look.emplace_back(avatarSel[i]["normal"], avatarOrigin + avatarSelAdj + Point<int16_t>(0, 18 * (t + 1)));
			}
		}

		nameboard = CustomizeChar["charName"];
		createLimitPos = Texture(createLimit["backgrnd"]).get_origin();
		createLimitNum = Charset(createLimit["num"], Charset::Alignment::RIGHT);

		buttons[Buttons::BtStart] = std::make_unique<MapleButton>(Common["BtStart"], Point<int16_t>(0, 1));
		buttons[Buttons::BtPreview] = std::make_unique<MapleButton>(Common["BtPreview"]);
		buttons[Buttons::BtGenderMale] = std::make_unique<MapleButton>(genderSelect["male"], getGenderSelectMalePos());
		buttons[Buttons::BtGenderFemale] = std::make_unique<MapleButton>(genderSelect["female"], getGenderSelectFemalePos());

		Point<int16_t> charLookAdj = Point<int16_t>(0, 0);

		if (className == "adventurer")
			charLookAdj = Point<int16_t>(75, 108);
		else if (className == "aran")
			charLookAdj = Point<int16_t>(69, 92);
		else
			charLookAdj = Point<int16_t>(0, 0);

		Point<int16_t> charLookLeft = avatarOrigin + charLookAdj;
		Point<int16_t> charLookRight = charLookLeft;

		if (className == "adventurer")
			charLookRight += Point<int16_t>(118, 0);
		else if (className == "aran")
			charLookRight += Point<int16_t>(110, 0);
		else
			charLookRight += Point<int16_t>(0, 0);

		buttons[Buttons::BtSkinLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
		buttons[Buttons::BtSkinRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

		charLookLeft.shift_y(18);
		charLookRight.shift_y(18);

		buttons[Buttons::BtFaceLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
		buttons[Buttons::BtFaceRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

		charLookLeft.shift_y(18);
		charLookRight.shift_y(18);

		buttons[Buttons::BtHairLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
		buttons[Buttons::BtHairRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

		charLookLeft.shift_y(18);
		charLookRight.shift_y(18);

		if (hairColorEnabled)
		{
			charLookLeft.shift_y(18);
			charLookRight.shift_y(18);
		}

		buttons[Buttons::BtTopLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
		buttons[Buttons::BtTopRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

		charLookLeft.shift_y(18);
		charLookRight.shift_y(18);

		if (bottomEnabled)
		{
			buttons[Buttons::BtBottomLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
			buttons[Buttons::BtBottomRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

			charLookLeft.shift_y(18);
			charLookRight.shift_y(18);
		}

		if (capeEnabled)
		{
			buttons[Buttons::BtCapeLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
			buttons[Buttons::BtCapeRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

			charLookLeft.shift_y(18);
			charLookRight.shift_y(18);
		}

		buttons[Buttons::BtShoesLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
		buttons[Buttons::BtShoesRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

		charLookLeft.shift_y(18);
		charLookRight.shift_y(18);

		buttons[Buttons::BtWeaponLeft] = std::make_unique<MapleButton>(CustomizeChar["BtLeft"], charLookLeft);
		buttons[Buttons::BtWeaponRight] = std::make_unique<MapleButton>(CustomizeChar["BtRight"], charLookRight);

		nl::node BtYes = CustomizeChar["BtYes"];
		BtYesPos = BtYes["pos"];

		nl::node BtNo = CustomizeChar["BtNo"];
		BtNoPos = BtNo["pos"];

		buttons[Buttons::BtYes] = std::make_unique<MapleButton>(BtYes, genderOrigin + Point<int16_t>(BtYesPos["gender"]));
		buttons[Buttons::BtNo] = std::make_unique<MapleButton>(BtNo, genderOrigin + Point<int16_t>(BtNoPos["gender"]));

		nl::node hairSelect = CustomizeChar["hairSelect"];
		hairSelectSize = hairSelect.size();

		for (size_t i = 0; i < hairSelectSize; i++)
			buttons[Buttons::BtHairSelect0 + i] = std::make_unique<MapleButton>(hairSelect[i], getHairSelectPos(i));

		disable_char_look();

		newCharNamePos = charNameOrigin + nameEditCtrlPos - Point<int16_t>(5, 5);

		if (className == "adventurer")
			newCharNamePos += Point<int16_t>(3, 0);
		else if (className == "cygnusKnight")
			newCharNamePos += Point<int16_t>(-1, 1);

		Point<int16_t> newCharNameDim = Point<int16_t>(0, 0);

		if (className == "adventurer")
			newCharNameDim = Point<int16_t>(163, 24);
		else if (className == "aran")
			newCharNameDim = Point<int16_t>(149, 25);
		else if (className == "cygnusKnight")
			newCharNameDim = Point<int16_t>(147, 19);
		else
			newCharNameDim = Point<int16_t>(0, 0);

		newCharName = Textfield(Text::Font::A12M, Text::Alignment::LEFT, getNewCharNameColor(), Rectangle<int16_t>(newCharNamePos, newCharNamePos + newCharNameDim), 12);

		newCharName.set_enter_callback(
			[&](std::string)
			{
				button_pressed(Buttons::BtYes);
			}
		);

		newCharName.set_key_callback(
			KeyAction::Id::ESCAPE,
			[&]()
			{
				button_pressed(Buttons::BtNo);
			}
		);

		skinColorName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		faceName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		hairStyleName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		topName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		bottomName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		capeName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		shoeName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		weaponName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);

		randomize_look();

		newCharLook.set_direction(true);

		(void)avatarSize;
		(void)genderFemale;
		(void)genderMale;
		(void)genderMarginX;
		(void)genderMarginY;
		(void)genderSize;
		(void)nameEditFontColor;
		(void)offsetForElemBack;
	}

	void UICommonCreation::init_classic(nl::node Login, nl::node Common)
	{
		// 原生 classic 建角页只有「命名」一个形态（职业团选择在 UIRaceSelect 完成）：
		// 吊牌 charName + 确定/取消 + 名称输入框 + 平台角色预览，无性别/外观选择。
		// 元素铺满屏幕（position=0,0），按钮/输入框坐标直接使用 1280x720 原生屏幕值；
		// nameboard 与 newCharLook 的 draw 调用本身即绝对坐标。
		const int16_t view_w = Constants::Constants::get().get_viewwidth();
		const int16_t view_h = Constants::Constants::get().get_viewheight();
		dimension = Point<int16_t>(view_w, view_h);
		position = Point<int16_t>(0, 0);

		if (LoginBackground::get().available())
			LoginBackground::get().set_step(3);

		std::string root_name = LoginUiAssets::classic_newchar_root(classType);
		nl::node NewChar = Login[root_name];

		if (!NewChar)
		{
			LoginUiAssets::log_error("[Creation] missing " + root_name);
			return;
		}

		// 直接进入命名态（GMS 三态流 gender→look→name 的前两态在原生 classic 中不存在）。
		genderSelected = true;
		charLookSelected = true;
		newCharNameDisabled = false;

		// 原生锚点：由正确端建角页截图（1280x720）模板匹配测得。
		// sign = 吊牌 NewChar*/charName 左上；yes/no = 确定/取消左上（各职业专属按钮资产）；
		// slot = 输入槽相对吊牌偏移；avatar = 预览角色脚底。
		Point<int16_t> sign_pos;
		Point<int16_t> yes_pos;
		Point<int16_t> no_pos;
		Point<int16_t> slot_pos;
		Point<int16_t> avatar_pos;

		if (className == "adventurer")
		{
			sign_pos = Point<int16_t>(721, 155);
			yes_pos = Point<int16_t>(748, 333);
			no_pos = Point<int16_t>(822, 333);
			slot_pos = Point<int16_t>(28, 104);
			avatar_pos = Point<int16_t>(627, 430);
		}
		else if (className == "cygnusKnight")
		{
			sign_pos = Point<int16_t>(725, 155);
			yes_pos = Point<int16_t>(756, 336);
			no_pos = Point<int16_t>(829, 336);
			slot_pos = Point<int16_t>(28, 104);
			avatar_pos = Point<int16_t>(637, 396);
		}
		else
		{
			sign_pos = Point<int16_t>(716, 166);
			yes_pos = Point<int16_t>(750, 304);
			no_pos = Point<int16_t>(824, 304);
			slot_pos = Point<int16_t>(28, 96);
			avatar_pos = Point<int16_t>(652, 401);
		}

		nameboard = NewChar["charName"];
		charNameOrigin = sign_pos;
		newAvatarPos = avatar_pos;

		classic_yes_name = yes_pos;
		classic_no_name = no_pos;

		// 侧栏「回到首页」与原生 UIWorldSelect 同位 (0,427)；职业专属确定/取消。
		buttons[Buttons::BtStart] = std::make_unique<MapleButton>(Common["BtStart"], Point<int16_t>(0, 427));
		buttons[Buttons::BtYes] = std::make_unique<MapleButton>(NewChar["BtYes"], classic_yes_name);
		buttons[Buttons::BtNo] = std::make_unique<MapleButton>(NewChar["BtNo"], classic_no_name);

		// Textfield::draw 会叠加 position（此处为 0,0），输入框落在吊牌输入槽内。
		// 输入槽 150x24 与 img 位图实测吻合（evidence/img-node-layout.txt §2）。
		// 槽位图像素均色约 RGB(135,69,14) 深棕木纹，黑字不可读 → 取与吊牌
		// 白色描边标题一致的白字（用户缺陷「字色不对」）。
		newCharNamePos = charNameOrigin + slot_pos;
		Point<int16_t> newCharNameDim(150, 24);
		newCharName = Textfield(
			Text::Font::A12M,
			Text::Alignment::LEFT,
			Color::Name::WHITE,
			Rectangle<int16_t>(newCharNamePos, newCharNamePos + newCharNameDim),
			12);

		// A12M 约 12px，FreeType 以 y 为基线，字形可见区约 [y-9, y+3]；
		// 24px 槽垂直居中需把基线放到 y=15（槽中线 12 + 降部 3）。
		// 这是字体度量推导（img 无文字基线节点），实机截图可再微调；
		// 只作用于本窗口，不改通用 Textfield。
		classic_name_shift = Point<int16_t>(0, 15);
		newCharName.set_text_shift(classic_name_shift);
		confirmed_name = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE);

		newCharName.set_enter_callback([&](std::string) { button_pressed(Buttons::BtYes); });
		newCharName.set_key_callback(KeyAction::Id::ESCAPE, [&]() { button_pressed(Buttons::BtNo); });

		// 左侧栏：创建角色步骤框（step/3）与选择的:世界信息框（selectWorld），与原生 UIWorldSelect 同位。
		version = Text(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK,
					   std::string("Ver. ") + Configuration::get().get_version());
		// 原生参考「Ver. 0.83」文字中心 y≈17；实测当前绘制中心 y≈25，上移 7px 对齐。
		version_pos = Point<int16_t>(1061, 9);

		step_create = Sprite(Common["step"]["3"], Point<int16_t>(0, 33));
		selworld_box = Sprite(Common["selectWorld"], Point<int16_t>(0, 96));

		// 选择的:世界信息框内的世界名 / 频道文本（从 UIWorldSelect 取当前选择）。
		std::string wname;
		uint8_t wch = 0xFF;
		if (auto worldselect = UI::get().get_element<UIWorldSelect>())
		{
			if (const World* w = worldselect->get_selected_world())
				wname = w->name;
			wch = worldselect->get_channelid();
		}
		// 进入选角后 UIWorldSelect 会清空选中频道（channelid=0xFF）；
		// enter_world() 已把实际频道存入 Configuration，回退读取。
		if (wch == 0xFF)
			wch = Configuration::get().get_channelid();
		// 原生侧栏世界名为全大写（参考截图：SCANIA / CH.1）。
		std::transform(wname.begin(), wname.end(), wname.begin(),
			[](unsigned char c) { return static_cast<char>(std::toupper(c)); });
		world_text = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, wname);
		channel_text = Text(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE,
							"CH." + std::to_string(wch + 1));

		// 查看所有角色按钮（与原生 UIWorldSelect 同位）。
		// 运行时 Login.img 的 BtViewAll 资产与 BtViewChoice 同图（均为「查看世界地图」，
		// 已由 nx_inspect 导出核实），原生端为「查看所有角色」——用参考截图裁出的
		// 真资产 PNG（99x35）覆盖；PNG 缺失时回退 img 资产。
		LocalPngTexture btva_normal;
		LocalPngTexture btva_pressed;
		constexpr size_t ATLAS_BTVA = 0x43524254ull;	// 'CRBT'
		bool btva_ok = btva_normal.load(ATLAS_BTVA, {
				"UI/Creation/BtViewAll_normal.png",
				"docs/features/login-flow-ui-ida-align/resources/creation/BtViewAll_normal.png" })
			&& btva_pressed.load(ATLAS_BTVA + 1, {
				"UI/Creation/BtViewAll_pressed.png",
				"docs/features/login-flow-ui-ida-align/resources/creation/BtViewAll_pressed.png" });

		if (btva_ok)
			buttons[Buttons::BtViewAll] = std::make_unique<LocalTwoSpriteButton>(
				std::move(btva_normal), std::move(btva_pressed), Point<int16_t>(0, 377));
		else
			buttons[Buttons::BtViewAll] = std::make_unique<MapleButton>(
				Login["WorldSelect"]["BtViewAll"], Point<int16_t>(0, 377));

		// 建角场景（原生整屏背景：冒险家蘑菇村/骑士团凉亭/战神雪原）。
		// 位图真身在 Map001/Back/login.img/back/<no>：16=1280x720、30=1280x900、33=1280x720，
		// 由 MapLogin.img/back/<no> 的 bS=no 引用。后者只是视差参数容器
		// （bS/no/type/x/y/cx/rx/ry，无 bitmap），传给 Sprite 只会得到空动画 ——
		// 这正是建角页此前仍显示「选职业大树场景」的根因。
		// 落点由正确端截图模板匹配测得（场景图左上在 1280x720 窗口内的位置）。
		const char* scene_no = nullptr;
		Point<int16_t> scene_pos;
		if (className == "adventurer")
		{
			scene_no = "16";
			scene_pos = Point<int16_t>(-3, -26);
		}
		else if (className == "cygnusKnight")
		{
			scene_no = "30";
			scene_pos = Point<int16_t>(-33, -32);
		}
		else
		{
			scene_no = "33";
			scene_pos = Point<int16_t>(-3, -25);
		}
		if (LoginBackground::get().available())
		{
			// R1：场景位图必须以运行时部署数据为准。UI/MapLogin.img/back/<no>
			// 只是 [null] 视差参数容器（bS/no/type/x/y/cx/rx/ry，无 bitmap 子帧），
			// 传给 Sprite 会得到空动画 —— 建角页因此一直显示「选职业大树」背景。
			// 真位图在 Map001/Back/login.img/back/<no>：16=蘑菇村(冒险家)、
			// 30=凉亭(骑士团)、33=雪原(战神)；编号对应关系由
			// MapLogin.img/back/41|45|39 的 no 字段确认。
			nl::node scene = nl::nx::Map001["Back"]["login.img"]["back"][scene_no];

			if (!scene)
				scene = nl::nx::Map["Back"]["login.img"]["back"][scene_no];

			if (scene)
				LoginUiAssets::log_info(std::string("[Creation] scene ") + scene_no
					+ " for " + classType);
			else
				LoginUiAssets::log_error(std::string("[Creation] missing scene ")
					+ scene_no + " for " + classType);

			// 空节点走 set_scene 内既有的 clear_scene()，不残留上一张场景。
			LoginBackground::get().set_scene(scene, scene_pos);
		}

		skinColorName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		faceName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		hairStyleName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		topName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		bottomName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		capeName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		shoeName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);
		weaponName = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK);

		if (skinColors.empty() || faces[false].empty() || hairStyles[false].empty()
			|| tops[false].empty() || shoes[false].empty() || weapons[false].empty())
		{
			LoginUiAssets::log_error("[Creation] MakeCharInfo pools empty for " + classType);
		}

		// ── 原生 step4「设定角色」外观 / 发色阶段（R1：运行时 NewChar* 节点） ──
		// 板内锚点来自 charSet 位图逐像素量测（notes/native-step4-layout.md §3）：
		// 米黄内容区板内 (8,100)-(211,273) 204x174；9 行 avatarSel 200x17 × 18px
		// 步距 = 162px 恰好容纳；底部小板 x≈22..200, y≈323..377 放骰子 / 性别。
		// 屏幕落点 (323,150) 为几何避让推导（左栏 150px、吊牌 x=721、角色 x=627），
		// 置信度低，已在对照表 §5 标为决策点，不伪装成原生常量。
		classic_charset_pos = Point<int16_t>(323, 150);

		// REA：sub_5F5962 证明 step4 复用同一窗口单例显隐、不新建窗口类，
		// 故确定 / 取消沿用命名页锚点，阶段切换只换内容不挪按钮。
		classic_yes_gender = classic_yes_name;
		classic_no_gender = classic_no_name;
		classic_yes_avatar = classic_yes_name;
		classic_no_avatar = classic_no_name;

		classic_charset = NewChar["charSet"];

		if (!classic_charset.is_valid())
			LoginUiAssets::log_error("[Creation] missing " + root_name + "/charSet");

		for (size_t i = 0; i < 9; i++)
		{
			nl::node row = NewChar["avatarSel"][i];

			classic_avatar_sel[i][0] = Texture(row["normal"]);
			classic_avatar_sel[i][1] = Texture(row["disabled"]);
		}

		classic_stat_tb = NewChar["statTb"];

		// dice/0..3 无 delay：手动帧播放并定格末帧，避开循环 Animation 永真陷阱。
		if (nl::node dice = NewChar["dice"])
		{
			for (const nl::node& f : dice)
			{
				Frame frame(f);

				if (frame.get_dimensions().x() > 0)
					classic_dice_frames.push_back(frame);
			}
		}

		// 9 行行序：sub_5FCF8A 的 9 类外观池 + 运行时 avatarSel/0..8。
		// 0 肤色 / 1 脸型 / 2 发型 / 3 发色(色板, 无左右箭头) / 4 上衣 /
		// 5 裤裙 / 6 披风 / 7 鞋 / 8 武器；与现有 Buttons 枚举一一对应。
		auto add_look_pair = [&](uint16_t left_id, uint16_t right_id, size_t row)
		{
			int16_t y = classic_charset_pos.y() + 100 + static_cast<int16_t>(row * 18);

			try_maple_btn(buttons, left_id, NewChar["BtLeft"],
				Point<int16_t>(classic_charset_pos.x() + 168, y), "Creation/step4/BtLeft");
			try_maple_btn(buttons, right_id, NewChar["BtRight"],
				Point<int16_t>(classic_charset_pos.x() + 185, y), "Creation/step4/BtRight");
		};

		add_look_pair(Buttons::BtSkinLeft, Buttons::BtSkinRight, 0);
		add_look_pair(Buttons::BtFaceLeft, Buttons::BtFaceRight, 1);
		add_look_pair(Buttons::BtHairLeft, Buttons::BtHairRight, 2);
		add_look_pair(Buttons::BtTopLeft, Buttons::BtTopRight, 4);
		add_look_pair(Buttons::BtBottomLeft, Buttons::BtBottomRight, 5);
		add_look_pair(Buttons::BtCapeLeft, Buttons::BtCapeRight, 6);
		add_look_pair(Buttons::BtShoesLeft, Buttons::BtShoesRight, 7);
		add_look_pair(Buttons::BtWeaponLeft, Buttons::BtWeaponRight, 8);

		// 发色色板：运行时无 hairSelect 专属资产（R6 精简包），
		// 用 BtLeft 15x16 作热区 + ColorBox 色块作颜色呈现。
		static const Color::Name hair_swatch_colors[8] =
		{
			Color::Name::BLACK, Color::Name::BROWN, Color::Name::DARKRED, Color::Name::ORANGE,
			Color::Name::YELLOW, Color::Name::BLUE, Color::Name::VIOLET, Color::Name::WHITE
		};

		hairSelectSize = 8;

		for (size_t i = 0; i < 8; i++)
		{
			Point<int16_t> swatch(classic_charset_pos.x() + 38 + static_cast<int16_t>(i * 20),
				classic_charset_pos.y() + 100 + 3 * 18);

			try_maple_btn(buttons, static_cast<uint16_t>(Buttons::BtHairSelect0 + i),
				NewChar["BtLeft"], swatch, "Creation/step4/BtHairSelect");
			classic_hair_swatches.emplace_back(12, 12, hair_swatch_colors[i], 1.0f);
		}

		// 底部小板：性别切换 ×2 + 骰子随机（BtCheck 50x24 兜底资产）。
		int16_t bottom_y = classic_charset_pos.y() + 330;

		try_maple_btn(buttons, Buttons::BtGenderMale, NewChar["BtCheck"],
			Point<int16_t>(classic_charset_pos.x() + 30, bottom_y), "Creation/step4/BtGenderMale");
		try_maple_btn(buttons, Buttons::BtGenderFemale, NewChar["BtCheck"],
			Point<int16_t>(classic_charset_pos.x() + 85, bottom_y), "Creation/step4/BtGenderFemale");
		try_maple_btn(buttons, Buttons::BtDice, NewChar["BtCheck"],
			Point<int16_t>(classic_charset_pos.x() + 140, bottom_y), "Creation/step4/BtDice");

		// 首屏仍是命名态，step4 控件全部先隐藏。
		disable_char_look();

		LoginUiAssets::log_info(std::string("[Creation] step4 rows=9 dice=")
			+ std::to_string(classic_dice_frames.size()) + " class=" + classType);

		randomize_look();
		newCharLook.set_direction(true);
	}

	void UICommonCreation::load_classic_gender_pool(bool female_flag, nl::node gender_node)
	{
		if (!gender_node)
			return;

		std::vector<int64_t> colors;

		for (nl::node color_node : gender_node[2])
			colors.push_back(color_node.get_integer());

		if (colors.empty())
			colors.push_back(0);

		for (nl::node face_node : gender_node[0])
			faces[female_flag].push_back(face_node.get_integer());

		for (nl::node hair_node : gender_node[1])
		{
			int64_t hair = hair_node.get_integer();
			hairStyles[female_flag].push_back(hair);

			int64_t base = (hair / 10) * 10;
			for (int64_t color : colors)
			{
				hairColors[female_flag][hair].push_back(base + color);
				hairColorEnabled = true;
			}
		}

		if (!female_flag)
		{
			for (nl::node skin_node : gender_node[3])
				skinColors.push_back(skin_node.get_integer());
		}

		for (nl::node top_node : gender_node[4])
			tops[female_flag].push_back(top_node.get_integer());

		for (nl::node bottom_node : gender_node[5])
		{
			bottoms[female_flag].push_back(bottom_node.get_integer());
			bottomEnabled = true;
		}

		for (nl::node shoe_node : gender_node[6])
			shoes[female_flag].push_back(shoe_node.get_integer());

		for (nl::node weapon_node : gender_node[7])
			weapons[female_flag].push_back(weapon_node.get_integer());
	}

	void UICommonCreation::load_classic_char_look()
	{
		choosableGender = true;
		load_classic_gender_pool(false, LoginUiAssets::classic_makechar_male(classType));
		load_classic_gender_pool(true, LoginUiAssets::classic_makechar_female(classType));

		if (skinColors.empty())
		{
			skinColors.push_back(0);
			skinColors.push_back(1);
		}
	}

	void UICommonCreation::set_classic_yes_no(uint8_t step)
	{
		if (!classic_mode)
			return;

		Point<int16_t> yes_pos = classic_yes_gender;
		Point<int16_t> no_pos = classic_no_gender;

		if (step == 1)
		{
			yes_pos = classic_yes_avatar;
			no_pos = classic_no_avatar;
		}
		else if (step == 2)
		{
			yes_pos = classic_yes_name;
			no_pos = classic_no_name;
		}

		if (buttons[Buttons::BtYes])
			buttons[Buttons::BtYes]->set_position(yes_pos);
		if (buttons[Buttons::BtNo])
			buttons[Buttons::BtNo]->set_position(no_pos);
	}

	void UICommonCreation::draw(float inter) const
	{
		for (const Sprite& sprite : sprites_background)
			sprite.draw(position, inter);

		if (classic_mode && classic_look_stage)
		{
			draw_classic_look_stage(inter);
		}
		else if (!genderSelected)
		{
			for (size_t i = 0; i < sprites_gender_select.size(); i++)
			{
				if (i == 1)
				{
					for (size_t f = 0; f < vCanvasCount; f++)
						sprites_gender_select[i].draw(position + Point<int16_t>(0, genderTopHeight + (boardMidHeight * f)), inter);
				}
				else
				{
					sprites_gender_select[i].draw(position, inter);
				}
			}

			if (classic_mode)
			{
				if (classic_gender_male.is_valid())
					classic_gender_male.draw(position + genderOrigin + Point<int16_t>(50, 55));
				if (classic_gender_female.is_valid())
					classic_gender_female.draw(position + genderOrigin + Point<int16_t>(190, 55));
			}

			UIElement::draw(inter);

			newCharLook.draw(newAvatarPos, inter);
		}
		else
		{
			if (!charLookSelected)
			{
				UIElement::draw_sprites(inter);

				for (size_t i = 0; i < sprites_lookboard.size(); i++)
				{
					if (i == 0 && className == "cygnusKnight")
					{
						sprites_lookboard[i].draw(position - Point<int16_t>(1, 0), inter);
					}
					else if (i == 1)
					{
						for (size_t f = 0; f < vCanvasCount; f++)
						{
							if (className == "cygnusKnight")
								sprites_lookboard[i].draw(position + Point<int16_t>(0, boardMidHeight * f), inter);
							else
								sprites_lookboard[i].draw(position + Point<int16_t>(0, genderTopHeight + (boardMidHeight * f)), inter);
						}
					}
					else
					{
						sprites_lookboard[i].draw(position, inter);
					}
				}

				for (auto& sprite : sprites_char_look)
					sprite.draw(position, inter);

				Point<int16_t> label_pos = avatarOrigin;

				if (className == "adventurer")
					label_pos += Point<int16_t>(140, 107);
				else if (className == "aran")
					label_pos += Point<int16_t>(132, 91);
				else if (className == "cygnusKnight")
					label_pos += Point<int16_t>(199, 105);
				else
					label_pos += Point<int16_t>(0, 0);

				skinColorName.draw(label_pos);

				label_pos.shift_y(18);

				faceName.draw(label_pos);

				label_pos.shift_y(18);

				hairStyleName.draw(label_pos);

				label_pos.shift_y(18);

				if (hairColorEnabled)
					label_pos.shift_y(18);

				topName.draw(label_pos);

				label_pos.shift_y(18);

				if (bottomEnabled)
				{
					bottomName.draw(label_pos);

					label_pos.shift_y(18);
				}

				if (capeEnabled)
				{
					capeName.draw(label_pos);

					label_pos.shift_y(18);
				}

				shoeName.draw(label_pos);

				label_pos.shift_y(18);

				weaponName.draw(label_pos);
				newCharLook.draw(newAvatarPos, inter);

				if (classic_mode)
					classic_dice.draw(position + avatarOrigin + Point<int16_t>(90, 300), inter);

				UIElement::draw_buttons(inter);
			}
			else
			{
				if (!charNameSelected)
				{
					UIElement::draw_sprites(inter);

					nameboard.draw(charNameOrigin);

					if (classic_mode)
						newCharName.draw(position, Point<int16_t>(0, 0));
					else if (className == "adventurer")
						newCharName.draw(position + Point<int16_t>(2, -2), Point<int16_t>(1, -1));
					else if (className == "cygnusKnight")
						newCharName.draw(position + Point<int16_t>(6, -3), Point<int16_t>(1, -1));
					else if (className == "aran")
						newCharName.draw(position + Point<int16_t>(6, -2), Point<int16_t>(1, -1));
					else
						newCharName.draw(position);

					newCharLook.draw(newAvatarPos, inter);

					UIElement::draw_buttons(inter);
				}
				else
				{
					UIElement::draw_sprites(inter);

					nameboard.draw(charNameOrigin);

					// 名字查重通过到 UIKeySelect 关闭之间仍要保留已确认的名字与角色，
					// 否则吊牌空槽 + 空平台就是「输入名字后角色与名字消失」。
					// Textfield 在此态是 DISABLED（draw 直接 return），故用只读 Text 呈现。
					confirmed_name.draw(position + newCharNamePos + classic_name_shift);
					newCharLook.draw(newAvatarPos, inter);

					UIElement::draw_buttons(inter);
				}
			}
		}

		// TOOD: Get number of character able to create
		if (!classic_mode)
			createLimitNum.draw("10", 5, position - createLimitPos + Point<int16_t>(7, 0));

		if (classic_mode)
		{
			step_create.draw(position, inter);
			selworld_box.draw(position, inter);
			world_text.draw(position + Point<int16_t>(99, 111));
			channel_text.draw(position + Point<int16_t>(99, 127));
		}

		if (version_pos.x() || version_pos.y() || !classic_mode)
			version.draw(position + version_pos - Point<int16_t>(0, 5));
	}

	void UICommonCreation::update()
	{
		if (classic_mode && classic_look_stage && classic_dice_playing
			&& !classic_dice_frames.empty())
		{
			// dice/0..3 无 delay：按 ~80ms/帧手动推进，播完定格末帧；
			// 不使用 Animation::update()（循环动画永不返回 true 的已知陷阱）。
			classic_dice_elapsed = static_cast<uint16_t>(classic_dice_elapsed + 16);

			if (classic_dice_elapsed >= 80)
			{
				classic_dice_elapsed = 0;

				if (classic_dice_frame + 1 < classic_dice_frames.size())
					classic_dice_frame++;
				else
					classic_dice_playing = false;
			}
		}

		if (!genderSelected)
		{
			for (Sprite& sprite : sprites_gender_select)
				sprite.update();
		}
		else
		{
			if (!charLookSelected)
			{
				for (Sprite& sprite : sprites_lookboard)
					sprite.update();

				for (Sprite& sprite : sprites_char_look)
					sprite.update();
			}
			else
			{
				if (!charNameSelected)
				{
					newCharName.update();

					if (!newCharNameDisabled)
						newCharName.set_state(Textfield::State::FOCUSED);
					else
						newCharName.set_state(Textfield::State::DISABLED);
				}
				else
				{
					newCharName.set_state(Textfield::State::DISABLED);
				}
			}
		}

		UIElement::update();
	}

	void UICommonCreation::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
				button_pressed(Buttons::BtNo);
			else if (keycode == KeyAction::Id::RETURN)
				button_pressed(Buttons::BtYes);
			else if (keycode == 'N' || keycode == 'n' || keycode == 'S' || keycode == 's')
			{
				UI::get().emplace<UISoftKey>([this](const std::string& name) {
					if (name.empty()) return;
					newCharName.change_text(name);
					newCharNameDisabled = false;
					LOG(LOG_DEBUG, "UICommonCreation: SoftKey name=" << name);
				}, []() {}, "Char name");
			}
			else if (keycode == 'Y' || keycode == 'y')
			{
				button_pressed(Buttons::BtYes);
			}
			else if (keycode == 'G' || keycode == 'g')
			{
				female = !female;
				LOG(LOG_DEBUG, "UICommonCreation: SoftKey gender female=" << female);
			}
		}
	}

	UIElement::Type UICommonCreation::get_type() const
	{
		return TYPE;
	}

	void UICommonCreation::draw_classic_look_stage(float inter) const
	{
		UIElement::draw_sprites(inter);

		// 命名态的吊牌与已确认名字保持可见（缺陷 3），角色继续站在平台上。
		nameboard.draw(charNameOrigin);
		confirmed_name.draw(position + newCharNamePos + classic_name_shift);

		if (classic_charset.is_valid())
			classic_charset.draw(classic_charset_pos);

		// 9 行条件：avatarSel/disabled 的深棕底色表示该行当前无可切换项，
		// 与 enable_char_look() 的池大小判断一致（R5 资产语义核验）。
		const bool row_enabled[9] =
		{
			skinColors.size() > 1,
			faces[female].size() > 1,
			hairStyles[female].size() > 1,
			hairColorEnabled,
			tops[female].size() > 1,
			bottomEnabled && bottoms[female].size() > 1,
			capeEnabled && capes[female].size() > 1,
			shoes[female].size() > 1,
			weapons[female].size() > 1
		};

		for (size_t i = 0; i < 9; i++)
		{
			const Texture& row = classic_avatar_sel[i][row_enabled[i] ? 0 : 1];

			if (row.is_valid())
				row.draw(classic_charset_pos + Point<int16_t>(8, 100)
					+ Point<int16_t>(0, static_cast<int16_t>(i * 18)));
		}

		// 发色色板内嵌在 15x16 热区里（留 1px 边框）；池为空时不呈现。
		for (size_t i = 0; hairColorEnabled && i < classic_hair_swatches.size(); i++)
		{
			Point<int16_t> swatch(classic_charset_pos.x() + 38 + static_cast<int16_t>(i * 20),
				classic_charset_pos.y() + 100 + 3 * 18);

			classic_hair_swatches[i].draw(swatch + Point<int16_t>(1, 1));
		}

		// dice/0..3 无 delay：按 80ms/帧手动推进并定格末帧。
		if (!classic_dice_frames.empty())
		{
			const Frame& frame = classic_dice_frames[
				std::min(classic_dice_frame, classic_dice_frames.size() - 1)];

			frame.draw(DrawArgument(Point<int16_t>(classic_charset_pos.x() + 155,
				classic_charset_pos.y() + 290)));
		}

		// 四属性表落在面板右侧空白带，避开吊牌 (721,155) 与角色 (627,430)。
		if (classic_stat_tb.is_valid())
			classic_stat_tb.draw(classic_charset_pos + Point<int16_t>(225, 60));

		newCharLook.draw(newAvatarPos, inter);

		UIElement::draw_buttons(inter);
	}

	void UICommonCreation::enter_classic_look_stage()
	{
		classic_look_stage = true;
		classic_creation_sent = false;
		newCharNameDisabled = true;
		classic_dice_frame = 0;
		classic_dice_elapsed = 0;
		classic_dice_playing = !classic_dice_frames.empty();

		enable_char_look();
		set_classic_yes_no(1);

		// 命名阶段发出查重包前把 Yes/No 置成 DISABLED，外观阶段必须恢复，
		// 否则「确定」不可点、流程卡死。
		if (buttons[Buttons::BtYes])
			buttons[Buttons::BtYes]->set_state(Button::State::NORMAL);
		if (buttons[Buttons::BtNo])
			buttons[Buttons::BtNo]->set_state(Button::State::NORMAL);

		LoginUiAssets::log_info(std::string("[Creation] step4 entered class=") + classType);
	}

	void UICommonCreation::leave_classic_look_stage()
	{
		classic_look_stage = false;
		classic_creation_sent = false;
		charNameSelected = false;
		newCharNameDisabled = false;

		disable_char_look();
		set_classic_yes_no(2);

		if (buttons[Buttons::BtYes])
			buttons[Buttons::BtYes]->set_state(Button::State::NORMAL);
		if (buttons[Buttons::BtNo])
			buttons[Buttons::BtNo]->set_state(Button::State::NORMAL);

		LoginUiAssets::log_info(std::string("[Creation] step4 left class=") + classType);
	}

	void UICommonCreation::create_new_character()
	{
		if (classic_creation_sent)
			return;

		classic_creation_sent = true;
		disable_char_look();

		// 名字已在命名阶段查重；重入 send_naming_result 复用同一条 CreateCharPacket
		// 路径（11 字段、hair 基础发型 + hairColor 增量、UIKeySelect 时序均不变）。
		// 进入发包路径时 classic_look_stage 仍为 true，作为「外观已确认」
		// 门闩跳过 send_naming_result 内的再次 gate。
		charNameSelected = false;
		send_naming_result(false);

		// 发包后回到命名确认态：吊牌 + 已确认名字 + 角色继续可见（缺陷 3）。
		classic_look_stage = false;
	}

	void UICommonCreation::send_naming_result(bool nameused)
	{
		if (!charNameSelected)
		{
			if (!nameused)
			{
				charNameSelected = true;

				std::string name = newCharName.get_text();
				confirmed_name = Text(Text::Font::A12M, Text::Alignment::LEFT,
					Color::Name::WHITE, name);

				// classic：名字查重通过后先进入原生 step4 外观 / 发色阶段，
				// 吊牌上的已确认名字与平台上的角色保持可见；外观阶段点「确定」
				// 才发 CreateCharPacket。外观已确认时 classic_look_stage=true，
				// 重入本函数不再 gate，发包逻辑保持原样。
				if (classic_mode && !classic_look_stage)
				{
					enter_classic_look_stage();
					return;
				}

				int32_t job = -1;
				int64_t face = faces[female][faceIndex];

				// 服务端 CreateCharHandler 分别读 hair（基础发型）与 hairColor（颜色增量），
				// 最终发型 = hair + hairColor；且 hair 必须落在
				// ItemConstants.isNewCharDefaultHair 的基础发型白名单内。
				// 因此这里把「发型 + 颜色」拆成两段发送，而不是发合成后的 id。
				int64_t hairStyle = hairStyles[female][hairStyleIndex];
				auto hairColors_ = hairColors[female][hairStyle];

				int64_t hair = (hairStyle / 10) * 10;
				int64_t hairColor = 0;

				if (hairColors_.size() > 0)
					hairColor = hairColors_[hairColorIndex] - hair;
				else
					hair = hairStyle;

				int64_t skin = skinColors[skinColorIndex];
				int64_t top = tops[female][topIndex];

				int64_t bottom = 0;

				if (bottoms[female].size() > 0)
					bottom = bottoms[female][bottomIndex];

				// 北斗 gms083 的 CreateCharHandler 只读到 weapon 为止（不含 cape），
				// 故 cape 仅用于捏人预览，不进包；v83 创建角色也不发放披风。
				int64_t cape = 0;

				if (capes[female].size() > 0)
					cape = capes[female][capeIndex];

				int64_t shoe = shoes[female][shoeIndex];
				int64_t weapon = weapons[female][weaponIndex];

				if (className == "adventurer")
					job = 1;
				else if (className == "aran")
					job = 2;
				else if (className == "cygnusKnight")
					job = 0;

				if (job >= 0)
				{
					LoginUiAssets::log_info(
						std::string("[Creation] CREATE_CHAR name=") + name
						+ " job=" + std::to_string(job)
						+ " female=" + (female ? "1" : "0")
						+ " face=" + std::to_string(face)
						+ " hair=" + std::to_string(hair)
						+ " hairColor=" + std::to_string(hairColor)
						+ " skin=" + std::to_string(skin));

					CreateCharPacket(name, job, face, static_cast<int32_t>(hair), static_cast<int32_t>(hairColor), static_cast<int32_t>(skin), static_cast<int32_t>(top), static_cast<int32_t>(bottom), static_cast<int32_t>(shoe), static_cast<int32_t>(weapon), female).dispatch();

					auto onok = [&](bool alternate)
					{
						Sound(Sound::Name::SCROLLUP).play();

						UI::get().remove(UIElement::Type::LOGINNOTICE_CONFIRM);
						UI::get().remove(UIElement::Type::LOGINNOTICE);
						UI::get().remove(UIElement::Type::CLASSCREATION);
						UI::get().remove(UIElement::Type::RACESELECT);

						if (auto charselect = UI::get().get_element<UICharSelect>())
							charselect->post_add_character();
					};

					UI::get().emplace<UIKeySelect>(onok, true);
				}
			}
			else
			{
				auto onok = [&]()
				{
					newCharNameDisabled = false;

					buttons[Buttons::BtYes]->set_state(Button::State::NORMAL);
					buttons[Buttons::BtNo]->set_state(Button::State::NORMAL);
				};

				UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NAME_IN_USE, onok);
			}
		}
	}

	Button::State UICommonCreation::button_pressed(uint16_t buttonid)
	{
		if (buttonid == Buttons::BtViewAll)
		{
			Sound(Sound::Name::SCROLLUP).play();
			ViewAllCharPacket().dispatch();
			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtStart)
		{
			Sound(Sound::Name::SCROLLUP).play();

			UI::get().remove(UIElement::Type::CLASSCREATION);
			UI::get().remove(UIElement::Type::RACESELECT);
			UI::get().remove(UIElement::Type::CHARSELECT);

			if (auto worldselect = UI::get().get_element<UIWorldSelect>())
				worldselect->makeactive();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtPreview)
		{
			Sound(Sound::Name::SCROLLUP).play();

			UI::get().remove(UIElement::Type::CLASSCREATION);

			UI::get().emplace<UIRaceSelect>();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtYes)
		{
			// classic step4 外观阶段的「确定」才真正发建角包。
			if (classic_mode && classic_look_stage)
			{
				create_new_character();
				return Button::State::NORMAL;
			}

			if (!genderSelected)
			{
				genderSelected = true;

				buttons[Buttons::BtGenderMale]->set_active(false);
				buttons[Buttons::BtGenderFemale]->set_active(false);

				enable_char_look();
				set_classic_yes_no(1);

				return Button::State::NORMAL;
			}
			else
			{
				if (!charLookSelected)
				{
					charLookSelected = true;

					disable_char_look();

					if (classic_mode)
						set_classic_yes_no(2);
					else
					{
						buttons[Buttons::BtYes]->set_position(genderOrigin + Point<int16_t>(BtYesPos["name"]));
						buttons[Buttons::BtNo]->set_position(genderOrigin + Point<int16_t>(BtNoPos["name"]));
					}

					newCharNameDisabled = false;

					return Button::State::NORMAL;
				}
				else
				{
					if (!charNameSelected)
					{
						std::string name = newCharName.get_text();

						if (name.size() <= 0)
						{
							return Button::State::NORMAL;
						}
						else if (name.size() >= 4)
						{
							newCharNameDisabled = true;

							buttons[Buttons::BtYes]->set_state(Button::State::DISABLED);
							buttons[Buttons::BtNo]->set_state(Button::State::DISABLED);

							if (auto raceselect = UI::get().get_element<UIRaceSelect>())
							{
								if (raceselect->check_name(name))
								{
									NameCharPacket(name).dispatch();

									return Button::State::IDENTITY;
								}
							}

							auto okhandler = [&]()
							{
								newCharNameDisabled = false;

								buttons[Buttons::BtYes]->set_state(Button::State::NORMAL);
								buttons[Buttons::BtNo]->set_state(Button::State::NORMAL);
							};

							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::ILLEGAL_NAME, okhandler);

							return Button::State::NORMAL;
						}
						else
						{
							newCharNameDisabled = true;

							buttons[Buttons::BtYes]->set_state(Button::State::DISABLED);
							buttons[Buttons::BtNo]->set_state(Button::State::DISABLED);

							auto okhandler = [&]()
							{
								newCharNameDisabled = false;

								buttons[Buttons::BtYes]->set_state(Button::State::NORMAL);
								buttons[Buttons::BtNo]->set_state(Button::State::NORMAL);
							};

							UI::get().emplace<UILoginNotice>(UILoginNotice::Message::ILLEGAL_NAME, okhandler);

							return Button::State::IDENTITY;
						}
					}
					else
					{
						return Button::State::NORMAL;
					}
				}
			}
		}
		else if (buttonid == Buttons::BtNo)
		{
			if (classic_mode)
			{
				// step4 外观阶段的「取消」退回命名态，输入框重新获得焦点。
				if (classic_look_stage)
				{
					leave_classic_look_stage();
					return Button::State::NORMAL;
				}

				// 原生 classic 命名态取消直接回职业团选择页。
				Sound(Sound::Name::SCROLLUP).play();

				UI::get().remove(UIElement::Type::CLASSCREATION);

				UI::get().emplace<UIRaceSelect>();

				return Button::State::NORMAL;
			}

			if (charLookSelected)
			{
				charLookSelected = false;

				enable_char_look();

				newCharNameDisabled = true;

				return Button::State::NORMAL;
			}
			else
			{
				if (genderSelected && choosableGender)
				{
					genderSelected = false;

					buttons[Buttons::BtGenderMale]->set_active(true);
					buttons[Buttons::BtGenderFemale]->set_active(true);

					disable_char_look();

					if (classic_mode)
						set_classic_yes_no(0);
					else
					{
						buttons[Buttons::BtYes]->set_position(genderOrigin + Point<int16_t>(BtYesPos["gender"]));
						buttons[Buttons::BtNo]->set_position(genderOrigin + Point<int16_t>(BtNoPos["gender"]));
					}

					return Button::State::NORMAL;
				}
				else
				{
					button_pressed(Buttons::BtPreview);

					return Button::State::NORMAL;
				}
			}
		}
		else if (buttonid == Buttons::BtSkinLeft)
		{
			skinColorIndex = (skinColorIndex > 0) ? skinColorIndex - 1 : skinColors.size() - 1;

			newCharLook.set_body(skinColors[skinColorIndex]);
			skinColorName.change_text(newCharLook.get_body()->get_name());

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtSkinRight)
		{
			skinColorIndex = (skinColorIndex < skinColors.size() - 1) ? skinColorIndex + 1 : 0;

			newCharLook.set_body(skinColors[skinColorIndex]);
			skinColorName.change_text(newCharLook.get_body()->get_name());

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtFaceLeft)
		{
			faceIndex = (faceIndex > 0) ? faceIndex - 1 : faces[female].size() - 1;

			newCharLook.set_face(faces[female][faceIndex]);
			faceName.change_text(newCharLook.get_face()->get_name());

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtFaceRight)
		{
			faceIndex = (faceIndex < faces[female].size() - 1) ? faceIndex + 1 : 0;

			newCharLook.set_face(faces[female][faceIndex]);
			faceName.change_text(newCharLook.get_face()->get_name());

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtHairLeft)
		{
			hairStyleIndex = (hairStyleIndex > 0) ? hairStyleIndex - 1 : hairStyles[female].size() - 1;

			int64_t hairStyle = hairStyles[female][hairStyleIndex];
			auto hairColors_ = hairColors[female][hairStyle];

			newCharLook.set_hair(hairColors_[hairColorIndex]);
			hairStyleName.change_text(newCharLook.get_hair()->get_name());

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtHairRight)
		{
			hairStyleIndex = (hairStyleIndex < hairStyles[female].size() - 1) ? hairStyleIndex + 1 : 0;

			int64_t hairStyle = hairStyles[female][hairStyleIndex];
			auto hairColors_ = hairColors[female][hairStyle];

			newCharLook.set_hair(hairColors_[hairColorIndex]);
			hairStyleName.change_text(newCharLook.get_hair()->get_name());

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtTopLeft)
		{
			topIndex = (topIndex > 0) ? topIndex - 1 : tops[female].size() - 1;

			newCharLook.add_equip(tops[female][topIndex]);
			topName.change_text(get_equipname(EquipSlot::Id::TOP));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtTopRight)
		{
			topIndex = (topIndex < tops[female].size() - 1) ? topIndex + 1 : 0;

			newCharLook.add_equip(tops[female][topIndex]);
			topName.change_text(get_equipname(EquipSlot::Id::TOP));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtBottomLeft)
		{
			bottomIndex = (bottomIndex > 0) ? bottomIndex - 1 : bottoms[female].size() - 1;

			newCharLook.add_equip(bottoms[female][bottomIndex]);
			bottomName.change_text(get_equipname(EquipSlot::Id::BOTTOM));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtBottomRight)
		{
			bottomIndex = (bottomIndex < bottoms[female].size() - 1) ? bottomIndex + 1 : 0;

			newCharLook.add_equip(bottoms[female][bottomIndex]);
			bottomName.change_text(get_equipname(EquipSlot::Id::BOTTOM));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtCapeLeft)
		{
			capeIndex = (capeIndex > 0) ? capeIndex - 1 : capes[female].size() - 1;

			newCharLook.add_equip(capes[female][capeIndex]);
			capeName.change_text(get_equipname(EquipSlot::Id::CAPE));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtCapeRight)
		{
			capeIndex = (capeIndex < capes[female].size() - 1) ? capeIndex + 1 : 0;

			newCharLook.add_equip(capes[female][capeIndex]);
			capeName.change_text(get_equipname(EquipSlot::Id::CAPE));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtShoesLeft)
		{
			shoeIndex = (shoeIndex > 0) ? shoeIndex - 1 : shoes[female].size() - 1;

			newCharLook.add_equip(shoes[female][shoeIndex]);
			shoeName.change_text(get_equipname(EquipSlot::Id::SHOES));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtShoesRight)
		{
			shoeIndex = (shoeIndex < shoes[female].size() - 1) ? shoeIndex + 1 : 0;

			newCharLook.add_equip(shoes[female][shoeIndex]);
			shoeName.change_text(get_equipname(EquipSlot::Id::SHOES));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtWeaponLeft)
		{
			weaponIndex = (weaponIndex > 0) ? weaponIndex - 1 : weapons[female].size() - 1;

			newCharLook.add_equip(weapons[female][weaponIndex]);
			weaponName.change_text(get_equipname(EquipSlot::Id::WEAPON));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtWeaponRight)
		{
			weaponIndex = (weaponIndex < weapons[female].size() - 1) ? weaponIndex + 1 : 0;

			newCharLook.add_equip(weapons[female][weaponIndex]);
			weaponName.change_text(get_equipname(EquipSlot::Id::WEAPON));

			check_names();

			return Button::State::NORMAL;
		}
		else if (buttonid == Buttons::BtGenderMale)
		{
			if (female)
			{
				female = false;

				randomize_look();

				// 性别切换后外观池大小可能变化：重算各行的可用性 / 按钮激活状态，
				// 与 draw_classic_look_stage() 的 row_enabled 保持一致。
				if (classic_mode && classic_look_stage)
				{
					disable_char_look();
					enable_char_look();
				}
				}

				return Button::State::NORMAL;
				}
				else if (buttonid == Buttons::BtGenderFemale)
		{
			if (!female)
			{
				female = true;

				randomize_look();

				if (classic_mode && classic_look_stage)
				{
					disable_char_look();
					enable_char_look();
				}
				}

				return Button::State::NORMAL;
				}
				else if (buttonid == Buttons::BtDice)
		{
			randomize_look();

			if (classic_mode && !classic_dice_frames.empty())
			{
				classic_dice_frame = 0;
				classic_dice_elapsed = 0;
				classic_dice_playing = true;
			}

			return Button::State::NORMAL;
		}
		else if (buttonid >= Buttons::BtHairSelect0 && buttonid < Buttons::LENGTH)
		{
			size_t picked = buttonid - Buttons::BtHairSelect0;
			int64_t hairStyle = hairStyles[female].empty() ? 0 : hairStyles[female][hairStyleIndex];
			auto hairColors_ = hairColors[female][hairStyle];

			// classic 色板固定 8 格，实际发色池可能更少；越界时不改外观。
			if (picked >= hairColors_.size())
				return Button::State::NORMAL;

			hairColorIndex = picked;

			newCharLook.set_hair(hairColors_[hairColorIndex]);
			hairStyleName.change_text(newCharLook.get_hair()->get_name());

			check_names();

			return Button::State::NORMAL;
		}
		else
		{
			return Button::State::PRESSED;
		}
	}

	void UICommonCreation::randomize_look()
	{
		auto next_or_zero = [&](size_t size) -> size_t
		{
			return size > 0 ? randomizer.next_int(size) : 0;
		};

		skinColorIndex = next_or_zero(skinColors.size());
		faceIndex = next_or_zero(faces[female].size());
		hairStyleIndex = next_or_zero(hairStyles[female].size());

		int64_t hairStyle = hairStyles[female].empty() ? 0 : hairStyles[female][hairStyleIndex];
		auto& hairColors_ = hairColors[female][hairStyle];
		hairColorIndex = next_or_zero(hairColors_.size());

		topIndex = next_or_zero(tops[female].size());
		bottomIndex = next_or_zero(bottoms[female].size());
		capeIndex = next_or_zero(capes[female].size());
		shoeIndex = next_or_zero(shoes[female].size());
		weaponIndex = next_or_zero(weapons[female].size());

		if (!skinColors.empty())
			newCharLook.set_body(skinColors[skinColorIndex]);
		if (!faces[female].empty())
			newCharLook.set_face(faces[female][faceIndex]);

		if (hairColors_.size() > 0)
			newCharLook.set_hair(hairColors_[hairColorIndex]);
		else if (hairStyle != 0)
			newCharLook.set_hair(hairStyle);

		if (!tops[female].empty())
			newCharLook.add_equip(tops[female][topIndex]);

		if (bottoms[female].size() > 0)
			newCharLook.add_equip(bottoms[female][bottomIndex]);

		if (capes[female].size() > 0)
			newCharLook.add_equip(capes[female][capeIndex]);

		if (!shoes[female].empty())
			newCharLook.add_equip(shoes[female][shoeIndex]);
		if (!weapons[female].empty())
			newCharLook.add_equip(weapons[female][weaponIndex]);

		skinColorName.change_text(newCharLook.get_body() ? newCharLook.get_body()->get_name() : "");
		faceName.change_text(newCharLook.get_face() ? newCharLook.get_face()->get_name() : "");
		hairStyleName.change_text(newCharLook.get_hair() ? newCharLook.get_hair()->get_name() : "");
		topName.change_text(get_equipname(EquipSlot::Id::TOP));
		bottomName.change_text(get_equipname(EquipSlot::Id::BOTTOM));
		capeName.change_text(get_equipname(EquipSlot::Id::CAPE));
		shoeName.change_text(get_equipname(EquipSlot::Id::SHOES));
		weaponName.change_text(get_equipname(EquipSlot::Id::WEAPON));

		check_names();
	}

	void UICommonCreation::load_char_look()
	{
		size_t color_count = 0, bottom_count = 0, cape_count = 0;

		nl::node MakeCharInfo = nl::nx::Etc["MakeCharInfo.img"][classType];

		for (nl::node gender_node : MakeCharInfo)
		{
			std::string gender_node_name = gender_node.name();

			if (gender_node_name == "info")
			{
				choosableGender = gender_node["choosableGender"].get_bool();

				if (!choosableGender)
					button_pressed(Buttons::BtYes);

				nl::node skins = gender_node["WZ2_skin"];

				for (nl::node skin : skins)
					skinColors.push_back(skin["id"].get_integer() - 12000);

				continue;
			}

			bool f = gender_node_name == "female";

			for (size_t type = 0; type < gender_node.size(); type++)
			{
				for (nl::node id_node : gender_node[type])
				{
					std::string id_node_name = id_node.name();

					if (id_node_name == "color" || id_node_name == "name")
						continue;

					int64_t id_node_value = id_node.get_integer();

					switch (type)
					{
						case 0:
						{
							faces[f].push_back(id_node_value);
							break;
						}
						case 1:
						{
							hairStyles[f].push_back(id_node_value);

							for (nl::node color_node : gender_node[type]["color"][id_node_value])
							{
								hairColors[f][id_node_value].push_back(color_node);

								color_count++;
							}

							break;
						}
						case 2:
						{
							tops[f].push_back(id_node_value);
							break;
						}
						default:
						{
							int64_t id_node_prefix = id_node_value / 10000;

							if (id_node_prefix == 106)
							{
								bottoms[f].push_back(id_node_value);

								bottom_count++;
								break;
							}
							else if (id_node_prefix == 107)
							{
								shoes[f].push_back(id_node_value);
								break;
							}
							else if (id_node_prefix == 110)
							{
								capes[f].push_back(id_node_value);

								cape_count++;
								break;
							}
							else
							{
								weapons[f].push_back(id_node_value);
								break;
							}
						}
					}
				}
			}
		}

		if (color_count > 0)
			hairColorEnabled = true;

		if (bottom_count > 0)
			bottomEnabled = true;

		if (cape_count > 0)
			capeEnabled = true;
	}

	void UICommonCreation::enable_char_look()
	{
		// classic 的按钮是可选兜底资产（R6 精简包可能缺节点），逐个判空，
		// 避免第三轮指出的空 unique_ptr 解引用；不伤 modern 原有行为。
		auto set_active = [&](uint16_t id, bool active)
		{
			if (buttons[id])
				buttons[id]->set_active(active);
		};

		if (classic_mode)
			set_classic_yes_no(1);
		else
		{
			if (buttons[Buttons::BtYes])
				buttons[Buttons::BtYes]->set_position(genderOrigin + Point<int16_t>(BtYesPos["avatar"]));
			if (buttons[Buttons::BtNo])
				buttons[Buttons::BtNo]->set_position(genderOrigin + Point<int16_t>(BtNoPos["avatar"]));
		}

		if (faces[female].size() > 1)
		{
			set_active(Buttons::BtFaceLeft, true);
			set_active(Buttons::BtFaceRight, true);
		}

		if (hairStyles[female].size() > 1)
		{
			set_active(Buttons::BtHairLeft, true);
			set_active(Buttons::BtHairRight, true);
		}

		if (skinColors.size() > 1)
		{
			set_active(Buttons::BtSkinLeft, true);
			set_active(Buttons::BtSkinRight, true);
		}

		if (tops[female].size() > 1)
		{
			set_active(Buttons::BtTopLeft, true);
			set_active(Buttons::BtTopRight, true);
		}

		if (bottomEnabled && bottoms[female].size() > 1)
		{
			set_active(Buttons::BtBottomLeft, true);
			set_active(Buttons::BtBottomRight, true);
		}

		if (capeEnabled && capes[female].size() > 1)
		{
			set_active(Buttons::BtCapeLeft, true);
			set_active(Buttons::BtCapeRight, true);
		}

		if (shoes[female].size() > 1)
		{
			set_active(Buttons::BtShoesLeft, true);
			set_active(Buttons::BtShoesRight, true);
		}

		if (weapons[female].size() > 1)
		{
			set_active(Buttons::BtWeaponLeft, true);
			set_active(Buttons::BtWeaponRight, true);
		}

		if (hairColorEnabled)
			for (size_t i = 0; i < hairSelectSize; i++)
				set_active(static_cast<uint16_t>(Buttons::BtHairSelect0 + i), true);

		if (classic_mode)
		{
			set_active(Buttons::BtDice, true);
			set_active(Buttons::BtGenderMale, true);
			set_active(Buttons::BtGenderFemale, true);
		}
	}

	void UICommonCreation::disable_char_look()
	{
		auto set_active = [&](uint16_t id, bool active)
		{
			if (buttons[id])
				buttons[id]->set_active(active);
		};

		set_active(Buttons::BtFaceLeft, false);
		set_active(Buttons::BtFaceRight, false);
		set_active(Buttons::BtHairLeft, false);
		set_active(Buttons::BtHairRight, false);
		set_active(Buttons::BtSkinLeft, false);
		set_active(Buttons::BtSkinRight, false);
		set_active(Buttons::BtTopLeft, false);
		set_active(Buttons::BtTopRight, false);

		if (bottomEnabled)
		{
			set_active(Buttons::BtBottomLeft, false);
			set_active(Buttons::BtBottomRight, false);
		}

		if (capeEnabled)
		{
			set_active(Buttons::BtCapeLeft, false);
			set_active(Buttons::BtCapeRight, false);
		}

		set_active(Buttons::BtShoesLeft, false);
		set_active(Buttons::BtShoesRight, false);
		set_active(Buttons::BtWeaponLeft, false);
		set_active(Buttons::BtWeaponRight, false);

		for (size_t i = 0; i < hairSelectSize; i++)
			set_active(static_cast<uint16_t>(Buttons::BtHairSelect0 + i), false);

		// 性别按钮在 modern 首屏由 button_pressed 自行激活，此处只收敛 classic。
		if (classic_mode)
		{
			// classic 的 9 行按钮全部由本阶段独占，命名态一律隐藏；
			// 不受 bottom/cape 池开关影响，避免首屏漏出箭头。
			set_active(Buttons::BtBottomLeft, false);
			set_active(Buttons::BtBottomRight, false);
			set_active(Buttons::BtCapeLeft, false);
			set_active(Buttons::BtCapeRight, false);
			set_active(Buttons::BtDice, false);
			set_active(Buttons::BtGenderMale, false);
			set_active(Buttons::BtGenderFemale, false);
		}
	}

	void UICommonCreation::check_names()
	{
		int16_t max_width = getMaxWidth();

		string_format::format_with_ellipsis(skinColorName, max_width, 3, false);
		string_format::format_with_ellipsis(faceName, max_width, 3, false);
		string_format::format_with_ellipsis(hairStyleName, max_width, 3, false);
		string_format::format_with_ellipsis(topName, max_width, 3, false);
		string_format::format_with_ellipsis(bottomName, max_width, 3, false);
		string_format::format_with_ellipsis(capeName, max_width, 3, false);
		string_format::format_with_ellipsis(shoeName, max_width, 3, false);
		string_format::format_with_ellipsis(weaponName, max_width, 3, false);
	}

	const std::string& UICommonCreation::get_equipname(EquipSlot::Id slot) const
	{
		if (int32_t item_id = newCharLook.get_equips().get_equip(slot))
		{
			return ItemData::get(item_id).get_name();
		}
		else
		{
			static const std::string& nullstr = "NULL";

			return nullstr;
		}
	}

	Point<int16_t> UICommonCreation::getGenderTopPos()
	{
		if (className == "adventurer")
			return Point<int16_t>(486, 95);
		else if (className == "aran")
			return Point<int16_t>(491, 168);
		else if (className == "cygnusKnight")
			return Point<int16_t>(423, 104);
		else
			return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getBoardMidPos()
	{
		if (className == "adventurer")
			return Point<int16_t>(486, 209);
		else if (className == "aran")
			return Point<int16_t>(491, 220);
		else if (className == "cygnusKnight")
			return Point<int16_t>(423, 222);
		else
			return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getBoardBottomPos()
	{
		if (className == "adventurer")
			return Point<int16_t>(486, 329);
		else if (className == "aran")
			return Point<int16_t>(491, 313);
		else if (className == "cygnusKnight")
			return Point<int16_t>(423, 348);
		else
			return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getAvatarSelPos(size_t index)
	{
		if (className == "adventurer")
		{
			if (index >= 2)
				index++;

			return Point<int16_t>(497, 197 + index * 18);
		}
		else if (className == "aran")
		{
			return Point<int16_t>(504, 187 + index * 18);
		}
		else if (className == "cygnusKnight")
		{
			int16_t y = 0;

			if (index == 3)
				y = 2;

			return Point<int16_t>(416, 98 + y);
		}
		else
		{
			return Point<int16_t>(0, 0);
		}
	}

	Point<int16_t> UICommonCreation::getGenderSelectMalePos()
	{
		Point<int16_t> genderSelectAdj = Point<int16_t>(-33, -13);

		if (className == "adventurer")
			genderSelectAdj = Point<int16_t>(0, -4);
		else if (className == "aran")
			genderSelectAdj = Point<int16_t>(-33, -13);
		else if (className == "cygnusKnight")
			genderSelectAdj = Point<int16_t>(2, 3);

		return genderOrigin + genderSelectAdj;
	}

	Point<int16_t> UICommonCreation::getGenderSelectFemalePos()
	{
		Point<int16_t> genderSelectMalePos = getGenderSelectMalePos();

		if (className == "adventurer")
			return genderSelectMalePos;
		else
			return genderSelectMalePos - Point<int16_t>(2, 0);
	}

	int16_t UICommonCreation::getCharLookY()
	{
		if (className == "adventurer")
			return 198;
		else if (className == "aran")
			return 187;
		else if (className == "cygnusKnight")
			return 81;
		else
			return 0;
	}

	Point<int16_t> UICommonCreation::getCharLookLeftPos()
	{
		int16_t charLookY = getCharLookY();

		if (className == "adventurer")
			return Point<int16_t>(552, charLookY);
		else if (className == "aran")
			return Point<int16_t>(562, charLookY);
		else if (className == "cygnusKnight")
			return Point<int16_t>(418, charLookY);
		else
			return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getCharLookRightPos()
	{
		int16_t charLookY = getCharLookY();

		if (className == "adventurer")
			return Point<int16_t>(684, charLookY);
		else if (className == "aran")
			return Point<int16_t>(699, charLookY);
		else if (className == "cygnusKnight")
			return Point<int16_t>(415, charLookY);
		else
			return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getBtYesPos(uint8_t step)
	{
		if (className == "adventurer")
		{
			switch (step)
			{
				case 0:
					return Point<int16_t>(514, 394);
				case 1:
					return Point<int16_t>(523, 425);
				case 2:
					return Point<int16_t>(513, 273);
			}
		}
		else if (className == "aran")
		{
			switch (step)
			{
				case 0:
					return Point<int16_t>(520, 397);
				case 1:
					return Point<int16_t>(533, 368);
				case 2:
					return Point<int16_t>(523, 243);
			}
		}
		else if (className == "cygnusKnight")
		{
			switch (step)
			{
				case 0:
					return Point<int16_t>(510, 396);
				case 1:
					return Point<int16_t>(502, 381);
				case 2:
					return Point<int16_t>(510, 289);
			}
		}

		return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getBtNoPos(uint8_t step)
	{
		if (className == "adventurer")
		{
			switch (step)
			{
				case 0:
					return Point<int16_t>(590, 394);
				case 1:
					return Point<int16_t>(597, 425);
				case 2:
					return Point<int16_t>(587, 273);
			}
		}
		else if (className == "aran")
		{
			switch (step)
			{
				case 0:
					return Point<int16_t>(594, 397);
				case 1:
					return Point<int16_t>(607, 368);
				case 2:
					return Point<int16_t>(597, 243);
			}
		}
		else if (className == "cygnusKnight")
		{
			switch (step)
			{
				case 0:
					return Point<int16_t>(615, 396);
				case 1:
					return Point<int16_t>(607, 381);
				case 2:
					return Point<int16_t>(615, 289);
			}
		}

		return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getHairSelectPos(size_t index)
	{
		if (className == "adventurer")
			return Point<int16_t>(677 + (index * 15), 339);
		else if (className == "cygnusKnight")
			return Point<int16_t>(674 + (index * 15), 349);
		else
			return Point<int16_t>(0, 0);
	}

	Color::Name UICommonCreation::getNewCharNameColor()
	{
		if (className == "aran")
			return Color::Name::WHITE;
		else
			return Color::Name::BLACK;
	}

	Rectangle<int16_t> UICommonCreation::getNewCharNameRect()
	{
		if (className == "adventurer")
		{
			Point<int16_t> pos = Point<int16_t>(514, 198);

			return Rectangle<int16_t>(pos, pos + Point<int16_t>(148, 24));
		}
		else if (className == "aran")
		{
			Point<int16_t> pos = Point<int16_t>(522, 197);

			return Rectangle<int16_t>(pos, pos + Point<int16_t>(146, 25));
		}
		else if (className == "cygnusKnight")
		{
			Point<int16_t> pos = Point<int16_t>(533, 213);

			return Rectangle<int16_t>(pos, pos + Point<int16_t>(147, 19));
		}
		else
		{
			return Rectangle<int16_t>(Point<int16_t>(0, 0), Point<int16_t>(0, 0));
		}
	}

	size_t UICommonCreation::getVerticalCount() const
	{
		if (className == "adventurer" || className == "aran")
			return 4;
		else if (className == "cygnusKnight")
			return 6;
		else
			return 0;
	}

	int16_t UICommonCreation::getVerticalAdj() const
	{
		if (className == "adventurer" || className == "aran")
			return 24;
		else if (className == "cygnusKnight")
			return 18;
		else
			return 0;
	}

	Point<int16_t> UICommonCreation::getNamePos(size_t index) const
	{
		int16_t name_x = 0;
		int16_t name_y = 0;

		if (className == "adventurer")
			name_x = 626;
		else if (className == "aran")
			name_x = 647;
		else if (className == "cygnusKnight")
			name_x = 618;

		if (className == "adventurer")
			name_y = 196;
		else if (className == "aran")
			name_y = 186;
		else if (className == "cygnusKnight")
			name_y = 203;

		return Point<int16_t>(name_x, name_y + index * 18);
	}

	Point<int16_t> UICommonCreation::getNameboardPos() const
	{
		if (className == "adventurer")
			return Point<int16_t>(486, 95);
		else if (className == "aran")
			return Point<int16_t>(489, 106);
		else if (className == "cygnusKnight")
			return Point<int16_t>(423, 104);
		else
			return Point<int16_t>(0, 0);
	}

	Point<int16_t> UICommonCreation::getTextfieldPos() const
	{
		if (className == "adventurer")
			return Point<int16_t>(8, -2);
		else if (className == "aran")
			return Point<int16_t>(3, 0);
		else if (className == "cygnusKnight")
			return Point<int16_t>(6, -3);
		else
			return Point<int16_t>(0, 0);
	}

	int16_t UICommonCreation::getMaxWidth() const
	{
		if (className == "adventurer")
			return 89;
		else if (className == "aran")
			return 94;
		else if (className == "cygnusKnight")
			return 97;
		else
			return 0;
	}

	UICommonCreation::~UICommonCreation()
	{
		if (LoginBackground::get().available() && LoginBackground::get().has_scene())
			LoginBackground::get().clear_scene();
	}
}
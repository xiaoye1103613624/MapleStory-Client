#pragma once
#include "../UIDragElement.h"
#include "../Components/Slider.h"
#include "../Components/Textfield.h"
#include "../../Audio/Audio.h"
#include "../../Graphics/Animation.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
#include <vector>
namespace ms {
class UIReport : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::REPORT;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIReport();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_target(const std::string& name);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
	std::string preset_target;
};
class UICoupon : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::COUPON;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UICoupon();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIClaim : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::CLAIM;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIClaim();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIIncubator : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::INCUBATOR;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIIncubator();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOk };
	bool find_incubator(int16_t& slot, int32_t& itemid) const;
	bool has_assets = false; Texture backgrnd; Text tip_label;
	int8_t egg_inv = 0; int16_t egg_slot = 0;
};
class UIRadio : public UIElement {
public:
	static constexpr Type TYPE = UIElement::Type::RADIO;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = false;
	UIRadio();
	void draw(float inter) const override;
	void update_screen(int16_t new_width, int16_t new_height) override;
	Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_on(bool on);
private:
	bool has_assets = false; bool on = false; Texture tex_on; Texture tex_off;
};
class UIKarmaScissors : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::KARMASCISSORS;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIKarmaScissors();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
	UIElement::Type get_type() const override;
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOk };
	bool has_assets = false; Texture backgrnd; Text tip_label;
	int16_t equip_slot = 0;
};
class UIEnchantSkill : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ENCHANTSKILL;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIEnchantSkill();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture effect; Text tip_label;
	int16_t equip_slot = 0;
	int16_t scroll_slot = 0;
};
class UIItemProtector : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ITEMPROTECTOR;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIItemProtector();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
	UIElement::Type get_type() const override;
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOk };
	bool has_assets = false; Texture backgrnd; Text tip_label;
	int16_t equip_slot = 0;
};
class UITeleport : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::TELEPORTUI;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UITeleport();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOk, BtSave };
	void refresh_tip();
	bool find_rock(int16_t& slot, int32_t& itemid, bool& vip) const;
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UISpeedQuiz : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::SPEEDQUIZ;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UISpeedQuiz();
	void draw(float inter) const override;
	void update() override;
	Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	// 开窗：nType/dwAnswer/nCorrect/nRemain/remainSec（IDA A26C66 → 7EC595）
	// nType 0=Npc/%07d/stand/0；1=Mob/%07d/stand/0；≥2=道具图标
	void open(int32_t n_type, int32_t dw_answer, int32_t n_correct, int32_t n_remain, int32_t remain_sec);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOk, BtNext, BtGiveup };
	void setup_input();
	void clear_input();
	void load_tip(int32_t n_type, int32_t dw_answer);
	void refresh_labels();
	bool try_submit();
	void submit_answer(const std::string& ans);
	void abort_empty(); // 超时 / Next / Esc → 空串
	void give_up();     // BtGiveup → "__GIVEUP__"

	bool has_assets = false;
	Texture backgrnd;
	Texture digit[10];
	Texture colon_tex;
	bool has_digits = false;
	Texture tip_tex;
	bool has_tip = false;

	Text pts_label;
	Text remain_label;
	Textfield input_field;
	bool input_active = false;
	bool closing = false;

	int32_t n_type = 0;
	int32_t dw_answer = 0;
	int32_t n_correct = 0;
	int32_t n_remain = 0;
	int32_t remain_sec = 0;
	int32_t remain_ms = 0;
	// IDA 7EC7CB / StringPool 1563 → Sound/MiniGame.img/Timer（≤120s 每秒 tick）
	Sound timer_sfx;
	bool has_timer_sfx = false;
};
class UIInitialQuiz : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::INITIALQUIZ;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIInitialQuiz();
	void draw(float inter) const override;
	void update() override;
	Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	// 开窗：title/problem/hint + min/max 字长 + remainSec（IDA A26B09 / 7EB0CC）
	void open(const std::string& title, const std::string& problem, const std::string& hint,
		int32_t min_len, int32_t max_len, int32_t remain_sec);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOk, BtCancel };
	void setup_input();
	void clear_input();
	void show_tip(const std::string& message);
	bool try_submit();
	void submit_answer(const std::string& ans);
	void abort_empty(); // 超时 / ESC / Cancel → 空串

	bool has_assets = false;
	Texture backgrnd;
	Texture digit[10];
	Texture colon_tex;
	bool has_digits = false;

	Text title_label;
	Text problem_label;
	Text hint_label;
	Textfield input_field;
	bool input_active = false;
	bool closing = false;

	int32_t min_len = 0;
	int32_t max_len = 0;
	int32_t remain_sec = 0;
	int32_t remain_ms = 0;
};

// NPC AskPet / AskPetAll（IDA 7474A2 / 74775C → UtilDlgEx dlgType=6）
// IDA 9A9B25/9ACB27/9AA9F0：UtilDlgEx_Pet 419×296；Prev/Next 轮播；stand0 立绘；nameTag+等级/亲密条。
class UIAskPet : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ASKPET;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIAskPet();
	void draw(float inter) const override;
	void update() override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void open(int8_t lastmsg, const std::string& tip, const std::vector<int64_t>& unique_ids, uint8_t flag = 0);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	// 与 AskAvatar 同族：Prev=0x2000 / Next=0x2001；OK/Cancel/Exit 槽位见 9A9B25。
	enum Buttons : uint16_t { BtPrev, BtNext, BtOk, BtCancel, BtExit };
	struct Entry {
		int64_t unique_id = 0;
		int32_t item_id = 0;
		std::string name;
		uint8_t level = 1;
		uint16_t closeness = 0;
		uint8_t fullness = 100;
		bool expired = false;
		bool has_stats = false;
	};

	void refresh_selected();
	void load_pet_stand(int32_t item_id);
	void submit_ok();
	void submit_cancel();

	// IDA sub_9A3CAE/9A3CED dlgType6；9A9B25 钮槽；9AA9F0 立绘脚点。
	static constexpr int16_t WND_W = 419;
	static constexpr int16_t WND_H = 297;
	static constexpr int16_t TEXT_X = 110;
	static constexpr int16_t TEXT_W = 289;
	static constexpr int16_t TEXT_AREA_H = 100;
	static constexpr int16_t PET_LOOK_X = 209;
	static constexpr int16_t PET_LOOK_Y = 218;
	static constexpr int16_t PET_ICON_X = 226;
	static constexpr int16_t NAMETAG_X = 123;
	static constexpr int16_t NAMETAG_Y = 237;
	static constexpr int16_t BTN_PREV_X = 15;
	static constexpr int16_t BTN_PREV_Y = 191;
	static constexpr int16_t BTN_NEXT_X = 341;
	static constexpr int16_t BTN_NEXT_Y = 191;
	static constexpr int16_t BTN_OK_X = 314;
	static constexpr int16_t BTN_CANCEL_X = 363;
	static constexpr int16_t BTN_EXIT_X = 9;
	static constexpr int16_t BTN_BOTTOM_Y = 272;
	static constexpr int16_t BAR_W = 173;
	static constexpr int16_t BAR_H = 6;
	static constexpr int16_t CLOSE_MAX = 30000;
	static constexpr int16_t FULL_MAX = 100;

	bool has_assets = false;
	Texture backgrnd;
	Texture name_tag;
	Texture shadow;
	bool has_name_tag = false;
	bool has_shadow = false;
	Text tip_label;
	Text name_label;
	Text level_label;
	Text close_label;
	Text full_label;
	Texture pet_icon;
	bool has_pet_icon = false;
	Animation pet_stand;
	bool has_pet_stand = false;
	std::vector<Entry> entries;
	int32_t selected = 0;
	int8_t lastmsg = 9;
	bool closing = false;
	int16_t tip_draw_y = 28;
};

// 维度镜（IDA 76B3F0 / 76B9FC / 747A46）：UIWindow.img/SlideMenu/<param>
// IsAniBackgrd=1 → backgrd 多帧动画；BtMain/<id> 为 Choice 内 38×38 目的地图标（非 BtMove 替身）。
class UIDimensionalMirror : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::DIMENSIONALMIRROR;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIDimensionalMirror();
	void draw(float inter) const override;
	void update() override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void open(int32_t mirror_param, const std::string& talk);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtLeft, BtRight, BtCancel, BtGo };
	struct Choice {
		int32_t id = 0;
		std::string label;
	};
	void parse_choices(const std::string& talk);
	void refresh_labels();
	void rebuild_main_icon();
	void submit_ok();
	void submit_cancel();
	nl::node resolve_skin() const;

	// IDA sub_4EDAB7(235,165)；Choice 框 (90,50) 40×40；BtMain 38×38 居中内嵌。
	static constexpr int16_t WND_W = 235;
	static constexpr int16_t WND_H = 165;
	static constexpr int16_t CHOICE_X = 90;
	static constexpr int16_t CHOICE_Y = 50;
	static constexpr int16_t MAIN_ICON_X = 91;
	static constexpr int16_t MAIN_ICON_Y = 51;

	bool has_assets = false;
	bool ani_backgrd = false;
	Animation backgrd_ani;
	Texture backgrnd;
	Texture cover;
	bool has_cover = false;
	Texture choice_mark;
	bool has_choice_mark = false;
	Texture main_icon;
	bool has_main_icon = false;
	Text prev_label;
	Text cur_label;
	Text next_label;
	std::vector<Choice> choices;
	int32_t selected = 0;
	int32_t mirror_param = 0;
	bool closing = false;
};

class UIRanking : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::RANKINGUI;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIRanking();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIItemMegaphone : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ITEMMEGAPHONE;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIItemMegaphone();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
	UIElement::Type get_type() const override;
private:
	void open_softkey();
	bool find_item_mega(int16_t& slot, int32_t& itemid) const;
	bool has_assets = false; Texture backgrnd; Text tip_label;
	int8_t attach_inv = 0; int16_t attach_slot = 0; bool has_attach = false;
};
class UICounsel : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::COUNSEL;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UICounsel();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMuruengRaid : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MURUENGRAID;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIMuruengRaid();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UICreateCygnus : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::CREATECYGNUS;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UICreateCygnus();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMegaphoneBasic : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MEGAPHONEBASIC;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIMegaphoneBasic();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	void open_softkey();
	bool find_megaphone(int16_t& slot, int32_t& itemid) const;
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIPersonalShop : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::PERSONALSHOP;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIPersonalShop();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMemberShop : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MEMBERSHOP;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIMemberShop();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UITradingRoom : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::TRADINGROOM;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UITradingRoom();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIEntrustedShop : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ENTRUSTEDSHOP;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIEntrustedShop();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UICashTradingRoom : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::CASHTRADINGROOM;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UICashTradingRoom();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIItemSearch : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ITEMSEARCH;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIItemSearch();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIAranSkillGuide : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ARANSKILLGUIDE;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIAranSkillGuide();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UITransform : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::TRANSFORMUI;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UITransform();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UIMonsterKilling : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MONSTERKILLING;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIMonsterKilling();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMapleEvent : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MAPLEEVENT;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIMapleEvent();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UISkillEx : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::SKILLEX;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UISkillEx();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIBook : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::BOOKUI;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIBook();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIQuestAlarm : public UIElement {
public:
	static constexpr Type TYPE = UIElement::Type::QUESTALARM;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = false;
	UIQuestAlarm();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	void update_screen(int16_t new_width, int16_t new_height) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMateMessage : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MATEMESSAGE;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIMateMessage();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UIBetaEdition : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::BETAEDITION;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIBetaEdition();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UIGMBoard : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::GMBOARD;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIGMBoard();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIShortCut : public UIElement {
public:
	static constexpr Type TYPE = UIElement::Type::SHORTCUTUI;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = false;
	UIShortCut();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	void update_screen(int16_t new_width, int16_t new_height) override;
	UIElement::Type get_type() const override;
protected:
	// UIWindow.img/ShortCut 的 8 颗真按钮（此前一个都没实例化 → 「图标缺失」）。
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t {
		BT_ITEM, BT_EQUIP, BT_STAT, BT_SKILL, BT_COMM, BT_QUEST,
		BT_MOBBOOK, BT_MESSENGER
	};
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UISkillMacroEx : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::SKILLMACROEX;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	// 原生技能组合栏一屏三行（Macroicon/0..2 在截图里逐像素命中，行距 45）。
	static constexpr int16_t VISIBLE_ROWS = 3;
	// SkillMacros::MAX_SLOTS 的镜像：头文件不引 Character/SkillMacros.h，改那里要同步这里。
	static constexpr size_t MACRO_COUNT = 5;
	UISkillMacroEx();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false;
	// true = UIWindow.img/SkillMacro 原生拼装外壳；false = 退到 SkillMacroEx 整块底衬。
	bool shell_ok = false;
	Texture backgrnd, macroslot, macroname, line02;
	Texture macro_icon[MACRO_COUNT];
	Texture macro_icon_sel[MACRO_COUNT];
	mutable Slider slider;	// 宏条数随封包变化，draw() 里同步 setrows（同 name_label 的先例）
	int16_t offset = 0;		// 首个可见宏
	int16_t selected = 0;	// 正在编辑的宏
	// 底部白框里的宏名。draw 是 const，所以照 UIStatusBar::classic_qs_label 的先例
	// 用 mutable + 变更检测。
	mutable Text name_label;
	mutable std::string shown_name;
};
class UICharacterClone : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::CHARACTERCLONE;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UICharacterClone();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UICancelRequests : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::CANCELREQUESTS;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UICancelRequests();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIReset : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::RESETUI;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIReset();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UIItemMove : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ITEMMOVE;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIItemMove();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
	int8_t src_inv = 0; int16_t src_slot = 0;
};
class UIFadeYesNo : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::FADEYESNO;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIFadeYesNo();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIContextMenu : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::CONTEXTMENU;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIContextMenu();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	// 在光标处打开；cid=0 表示空白地右键（仅系统项）
	void open_at(Point<int16_t> screen_pos, int32_t cid, const std::string& name);
	Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtInfo, BtWhisper, BtClose };
	bool has_assets = false; Texture backgrnd;
	int32_t target_cid = 0;
	std::string target_name;
	Text title_label;
	void activate_row(int row);
};
class UIEventWindow : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::EVENTWINDOW;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIEventWindow();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UISlideMenu : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::SLIDEMENU;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UISlideMenu();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void toggle_expand();
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtToggle, BtEvent, BtPartySearch };
	bool has_assets = false;
	bool expanded = false;
	Texture cover;
	Texture body;
};
class UIUtilDlgEx : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::UTILDLGEX;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIUtilDlgEx();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UIMultiLine : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MULTILINE;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIMultiLine();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIGameMenu : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::GAMEMENU;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIGameMenu();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	// 气球挂在底栏「目录」钮正上方（下摆尖端对准钮中心），替掉屏幕居中。
	void update_screen(int16_t new_width, int16_t new_height) override;
	UIElement::Type get_type() const override;
protected:
	// UIWindow.img/GameMenu 的 4 颗真按钮（频道变更/游戏设置/系统设置/游戏结束）。
	// 此前只画底板 + 一行英文按键提示，等于把开发用的调试壳暴露给了玩家。
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t {
		BT_CHANNEL, BT_GAMEOPT, BT_SYSOPT, BT_QUIT
	};
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMacro : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MACROUI;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIMacro();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIUtilDlgExAvatar : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::UTILDLGEX_AVATAR;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIUtilDlgExAvatar();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIUtilDlgExPet : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::UTILDLGEX_PET;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIUtilDlgExPet();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
};
class UIUtilDlgExMultiPet : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::UTILDLGEX_MULTIPET;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIUtilDlgExMultiPet();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIExceptionItemSearch : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::EXCEPTIONITEMSEARCH;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIExceptionItemSearch();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMobGage : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MOBGAGE;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = false;
	UIMobGage();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd;
	Text tip_label;
};
class UISkillUp : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::SKILLUP;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = false;
	UISkillUp();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd;
	Text tip_label;
};
class UIMacroSkill : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MACROSKILL;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = true;
	UIMacroSkill();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIQuestIcon : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::QUESTICON;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = false;
	UIQuestIcon();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIMinigameTable : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MINIGAMETABLE;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIMinigameTable();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
	void set_tip(const std::string& tip);
	void set_game_kind(uint8_t kind); // 1=Omok 2=MatchCard
	void note_place(uint8_t x, uint8_t y, uint8_t piece);
	void clear_board();
private:
	bool has_assets = false; Texture backgrnd;
	Text tip_label;
	uint8_t game_kind = 1;
	uint8_t next_piece = 1;
	// Omok 15x15：0 空 / 1 黑 / 2 白
	uint8_t board[15][15] = {};
	Texture stone_black;
	Texture stone_white;
	bool has_stones = false;
};
class UIFloatNotice : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::FLOATNOTICE;
	static constexpr bool FOCUSED = false; static constexpr bool TOGGLED = false;
	UIFloatNotice();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd;
	Text tip_label;
};

/// 悄悄话目标弹层（原生「请输入悄悄话的对象。」那一层）。
/// 框体不是整张底图，而是 Basic.img/Notice4 的 t/c/s 竖拼：原生截帧 261x117
/// 逐行反查（evidence/w4-chatbar/whisper-dlg-prep.md §5'）里，截帧每一行的左带
/// 指纹都能在 Notice4 的件里报到出处 —— 顶片 t(266x21) 占截帧行 -1..19，底片
/// s(266x78) 占行 43..120，中间夹一段被纵向拉到位的 c(266x20)。截帧裁掉了件
/// 自带的左 3 / 上 1 投影，所以本窗用 FRAME_H = 21+22+78，内部坐标一律相对
/// 框左上角（= 截帧坐标 + (3, 1)），实测值见下表注释。
class UIWhisperTargetDlg : public UIElement {
public:
	static constexpr Type TYPE = UIElement::Type::WHISPERTARGET;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIWhisperTargetDlg();

	void draw(float inter) const override;
	void update() override;
	void remove_cursor() override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
	UIElement::Type get_type() const override;

	/// 打开即刷新候选名单（队友 + 好友在线），并把输入框聚焦。
	void open();

protected:
	Button::State button_pressed(uint16_t buttonid) override;

private:
	enum Buttons : uint16_t { BtOk, BtNo };

	// —— 以下全部来自截帧实测（+3/+1 折算到框坐标），不是排版审美 ——
	static constexpr int16_t FRAME_W = 266;
	static constexpr int16_t TOP_H = 21;
	static constexpr int16_t MID_H = 22;	// Notice4/c 被纵向拉到 43-21
	static constexpr int16_t BOT_H = 78;
	static constexpr int16_t HINT_INK_TOP = 30;	// 提示墨迹顶
	static constexpr int16_t HINT_X = 25;		// 提示墨迹左沿
	static constexpr int16_t FIELD_X = 22;		// ComboBox 左件起点
	static constexpr int16_t FIELD_Y = 47;		// 黑上沿那一行
	static constexpr int16_t FIELD_W = 238;		// 右件（含箭头）止于 x=259
	static constexpr int16_t FIELD_H = 17;		// ComboBox 件高
	static constexpr int16_t NAME_X = 27;		// 光标实测列
	static constexpr int16_t OK_X = 162;
	static constexpr int16_t NO_X = 212;
	static constexpr int16_t BTN_Y = 91;
	static constexpr int16_t ROW_H = 15;		// 本客户端 ComboBox 弹表行高

	static int16_t frame_height();
	int16_t arrow_x() const;	// 右件（箭头）左沿
	bool in_arrow(Point<int16_t> cursorpos) const;
	bool in_row(Point<int16_t> cursorpos, size_t row) const;
	void pick_candidate(size_t row);
	void confirm();
	// UI::send_cursor (IO/UI.cpp:155) 在「点击落在输入框外」时会把 focusedtextfield
	// 置空，而 Textfield 自己的 state 仍是 FOCUSED —— 于是 set_state(FOCUSED) 是空操作，
	// 焦点回不来。必须先 NORMAL（触发 remove_textfield）再 FOCUSED（重新登记）。
	void refocus_field();
	std::vector<std::string> collect_candidates() const;

	bool has_assets = false;
	bool arrow_hover = false;
	// Notice4 三段 + ComboBox 三件（normal/pressed/mouseOver 各一套）。
	Texture frame_top, frame_mid, frame_bottom;
	Texture combo[3][3];	// [state][part]
	int16_t combo_left_w = 5, combo_mid_w = 5, combo_right_w = 18;
	Text hint_label;
	Textfield name_field;
	std::vector<std::string> candidates;
	std::vector<Text> candidate_text;
	bool list_open = false;
	size_t hovered = 0;
	// 行高随候选数变，draw() 是 const，所以高度在这里 mutable（只有 setheight 会被调）。
	mutable ColorBox list_back, list_inner;
	ColorBox list_mark;
};
}

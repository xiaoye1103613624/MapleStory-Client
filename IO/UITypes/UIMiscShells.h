#pragma once
#include "../UIDragElement.h"
#include "../Components/Slider.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
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
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
private:
	bool has_assets = false; Texture backgrnd; Text tip_label;
};
class UIInitialQuiz : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::INITIALQUIZ;
	static constexpr bool FOCUSED = true; static constexpr bool TOGGLED = false;
	UIInitialQuiz();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
private:
	bool has_assets = false; Texture backgrnd;
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
}

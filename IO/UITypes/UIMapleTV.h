#pragma once
#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
namespace ms {
class UIMapleTV : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::MAPLETV;
	static constexpr bool FOCUSED = true;
	static constexpr bool TOGGLED = false;
	UIMapleTV();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_tip(const std::string& tip);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOk, BtCancel };
	bool has_assets = false;
	Texture backgrnd;
	Text tip_label;
};
class UIAdminClaim : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ADMINCLAIM;
	static constexpr bool FOCUSED = true;
	static constexpr bool TOGGLED = false;
	UIAdminClaim();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtClaim, BtCancel };
	bool has_assets = false;
	Texture backgrnd;
};
class UIClassMatch : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::CLASSMATCH;
	static constexpr bool FOCUSED = false;
	static constexpr bool TOGGLED = true;
	UIClassMatch();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtOK };
	bool has_assets = false;
	Texture backgrnd;
};
}

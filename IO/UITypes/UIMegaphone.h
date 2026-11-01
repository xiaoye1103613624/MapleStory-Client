#pragma once
#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
namespace ms {
class UIArtMegaphone : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::ARTMEGAPHONE;
	static constexpr bool FOCUSED = true;
	static constexpr bool TOGGLED = false;
	UIArtMegaphone();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtEx, BtPreView };
	bool has_assets = false;
	Texture backgrnd;
};
class UIAvatarMegaphone : public UIDragElement<PosNOTICE> {
public:
	static constexpr Type TYPE = UIElement::Type::AVATARMEGAPHONE;
	static constexpr bool FOCUSED = true;
	static constexpr bool TOGGLED = false;
	UIAvatarMegaphone();
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
}

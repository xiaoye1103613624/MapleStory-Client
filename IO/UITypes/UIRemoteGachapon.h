#pragma once
#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
#include <string>
namespace ms {
class UIRemoteGachapon : public UIDragElement<PosREMOTEGACHAPON> {
public:
	static constexpr Type TYPE = UIElement::Type::REMOTEGACHAPON;
	static constexpr bool FOCUSED = false;
	static constexpr bool TOGGLED = true;
	UIRemoteGachapon();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_result(int8_t mode);
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { Bt0 = 0, BtClose = 20 };
	bool has_assets; Texture backgrnd;
	Text status;
};
}

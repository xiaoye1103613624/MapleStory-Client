#pragma once
#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"
#include <string>
namespace ms {
class UICashGachapon : public UIDragElement<PosCASHGACHAPON> {
public:
	static constexpr Type TYPE = UIElement::Type::CASHGACHAPON;
	static constexpr bool FOCUSED = false;
	static constexpr bool TOGGLED = true;
	UICashGachapon();
	void draw(float inter) const override;
	void send_key(int32_t keycode, bool pressed, bool escape) override;
	UIElement::Type get_type() const override;
	void set_result(int32_t reward_id, int8_t qty, int32_t remaining, bool jackpot);
	void set_failed();
protected:
	Button::State button_pressed(uint16_t buttonid) override;
private:
	enum Buttons : uint16_t { BtClose, BtOpen };
	bool has_assets; Texture backgrnd;
	Text status;
};
}

//////////////////////////////////////////////////////////////////////////////////
//	MonsterCarnival HUD
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIMonsterCarnival : public UIDragElement<PosPARTYRACE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MONSTERCARNIVAL;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMonsterCarnival();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_cp(int16_t team, int16_t total, int16_t available);
		void set_enemy_cp(int16_t total, int16_t available);
		int16_t get_local_team() const { return local_team; }
		void set_message(const std::string& msg);

	private:
		bool has_assets = false;
		Texture backgrnd;
		Text cp_label;
		Text enemy_label;
		Text msg_label;
		int16_t local_team = 0;
	};
}

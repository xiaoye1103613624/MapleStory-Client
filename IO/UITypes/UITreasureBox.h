//////////////////////////////////////////////////////////////////////////////////
//	TreasureBox — 金/银宝箱成功开启后的开箱结果窗（非地图 Reactor）
//	入站：SUCCESS_IN_USE_GACHAPON_BOX（s9 UseTreasureBox = writeInt itemId）
//	出站：背包双击 4280000/4280001 → UseTreasureChestPacket
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"

namespace ms
{
	class UITreasureBox : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::TREASUREBOX;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UITreasureBox();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		/// 按道具 id 选 TreasureBox/0|1 皮肤，并切到 opened 态（GachaponBox 入站）。
		void apply_result(int32_t item_id);
		void set_opened(bool opened);

	private:
		void load_variant(int variant_index);

		bool has_assets = false;
		bool opened = false;
		int variant = 0;
		Texture closed_tex;
		Texture opened_tex;
	};
}

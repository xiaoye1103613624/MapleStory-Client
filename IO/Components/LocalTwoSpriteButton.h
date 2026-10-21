//////////////////////////////////////////////////////////////////////////////////
//	LocalTwoSpriteButton — TwoSpriteButton for LocalPngTexture pairs
//	Same state rules as inventory tabs: NORMAL→disabled, PRESSED/MOUSEOVER→enabled
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Button.h"

#include "../../Graphics/LocalPngTexture.h"
#include "../../Template/BoolPair.h"

namespace ms
{
	class LocalTwoSpriteButton : public Button
	{
	public:
		// nsrc = normal/disabled canvas, ssrc = selected/enabled canvas (same order as TwoSpriteButton)
		LocalTwoSpriteButton(LocalPngTexture nsrc, LocalPngTexture ssrc, Point<int16_t> position);
		LocalTwoSpriteButton();

		void draw(Point<int16_t> parentpos) const override;
		void update() override {}
		Rectangle<int16_t> bounds(Point<int16_t> parentpos) const override;
		int16_t width() const override;
		Point<int16_t> origin() const override;
		Cursor::State send_cursor(bool, Point<int16_t>) override { return Cursor::State::IDLE; }

	private:
		BoolPair<LocalPngTexture> textures; // [true]=enabled/selected, [false]=disabled/normal
		Point<int16_t> pos;
	};
}

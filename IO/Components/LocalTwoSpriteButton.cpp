//////////////////////////////////////////////////////////////////////////////////
//	LocalTwoSpriteButton
//////////////////////////////////////////////////////////////////////////////////
#include "LocalTwoSpriteButton.h"

namespace ms
{
	LocalTwoSpriteButton::LocalTwoSpriteButton(LocalPngTexture nsrc, LocalPngTexture ssrc, Point<int16_t> position) :
		textures(std::move(ssrc), std::move(nsrc)),
		pos(position)
	{
		state = Button::State::NORMAL;
		active = true;
		pressed = false;
	}

	LocalTwoSpriteButton::LocalTwoSpriteButton() :
		textures(LocalPngTexture(), LocalPngTexture()),
		pos()
	{
		state = Button::State::NORMAL;
		active = false;
		pressed = false;
	}

	void LocalTwoSpriteButton::draw(Point<int16_t> parentpos) const
	{
		if (!active)
			return;

		bool selected = state == Button::State::MOUSEOVER || state == Button::State::PRESSED;
		const LocalPngTexture& tex = textures[selected];

		if (tex.is_valid())
			tex.draw(pos + parentpos);
	}

	Rectangle<int16_t> LocalTwoSpriteButton::bounds(Point<int16_t> parentpos) const
	{
		bool selected = state == Button::State::MOUSEOVER || state == Button::State::PRESSED;
		const LocalPngTexture& tex = textures[selected];
		Point<int16_t> absp = parentpos + pos;
		Point<int16_t> dim(
			tex.is_valid() ? tex.width() : 0,
			tex.is_valid() ? tex.height() : 0
		);
		return Rectangle<int16_t>(absp, absp + dim);
	}

	int16_t LocalTwoSpriteButton::width() const
	{
		bool selected = state == Button::State::MOUSEOVER || state == Button::State::PRESSED;
		const LocalPngTexture& tex = textures[selected];
		return tex.is_valid() ? tex.width() : 0;
	}

	Point<int16_t> LocalTwoSpriteButton::origin() const
	{
		return Point<int16_t>(0, 0);
	}
}

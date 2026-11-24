//////////////////////////////////////////////////////////////////////////////////
//	LocalFrameButton — MainNotice-style Default/Notify (optional Appear) PNG frames.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Button.h"

#include "../../Graphics/LocalPngTexture.h"

#include <string>
#include <vector>

namespace ms
{
	// Plays Default loop while NORMAL; Notify loop while MOUSEOVER/PRESSED.
	// Appear (if present) plays once on first draw cycle then switches to Default.
	class LocalFrameButton : public Button
	{
	public:
		LocalFrameButton() = default;
		LocalFrameButton(std::vector<LocalPngTexture> defaults,
			std::vector<LocalPngTexture> notifys,
			std::vector<LocalPngTexture> appears,
			Point<int16_t> position);

		void draw(Point<int16_t> parentpos) const override;
		void update() override;
		void set_position(Point<int16_t> position) override;
		Rectangle<int16_t> bounds(Point<int16_t> parentpos) const override;
		int16_t width() const override;
		Point<int16_t> origin() const override;
		Cursor::State send_cursor(bool, Point<int16_t>) override { return Cursor::State::IDLE; }

		// Load frames for iconKey from UI/SideIcons/frames/{key}/ and ICON fallbacks.
		// atlas_base must be unique per button; consumes up to 64 ids.
		static bool try_load(const std::string& icon_key, size_t atlas_base,
			std::vector<LocalPngTexture>& out_default,
			std::vector<LocalPngTexture>& out_notify,
			std::vector<LocalPngTexture>& out_appear);

	private:
		const LocalPngTexture* current_frame() const;

		std::vector<LocalPngTexture> defaults;
		std::vector<LocalPngTexture> notifys;
		std::vector<LocalPngTexture> appears;
		Point<int16_t> pos;
		int frame = 0;
		int anim_ms = 0;
		bool appear_done = false;
		static constexpr int FRAME_MS = 100;
	};
}

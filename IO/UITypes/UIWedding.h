//////////////////////////////////////////////////////////////////////////////////
//	Wedding invitation / progress shell
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIWedding : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WEDDING;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIWedding();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_invitation(bool cathedral, const std::string& tip);
		void set_progress(uint8_t step, int32_t groom, int32_t bride);
		/// 婚礼礼物台：wishlist = 心愿单条目名；gifts = 已收礼物 itemid 列表。
		/// 数据来自 WEDDING_GIFT_RESULT 的 0x09/0x0A/0x0B/0x0F 分支。
		void set_gifts(const std::vector<std::string>& wishlist, const std::vector<int32_t>& gifts);
		/// 礼物台取礼：按 gifts 列表下标 → WEDDING_ACTION mode7。
		void take_gift(size_t index);
		/// 礼物台送礼：从 ETC 背包选一条 → WEDDING_ACTION mode6。
		void send_gift(int16_t slot, int32_t itemid, int16_t quantity);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtOK };

		/// 请客：需本地已有订婚关系（relationship_id）+ ETC 栏一张请柬。
		void invite_guest(const std::string& guest);

		bool has_assets = false;
		Texture invite;
		Text tip_label;
		Text progress_label;
		// 礼物台数据面（WEDDING_GIFT_RESULT 各 mode 落进来的快照）
		std::vector<std::string> wishlist;
		std::vector<int32_t> gifts;
		// 心愿单展示行：随 set_gifts 刷新，避免每帧构造 Text。
		Text wishlist_label;
	};
}

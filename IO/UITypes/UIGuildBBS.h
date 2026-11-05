////////////////////////////////////////////////////////////////////////////////
//	Guild BBS 原生窗 — UI/GuildBBS.img（列表 / 读帖 / 发帖）
//	证据：BeiDou-Client-xy REA wz_dump/GuildBBS.img_runtime.json；
//	s9 GuildPackets.BBSThreadList / showThread；c2s BBS_OPERATION(155)。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/SpecialText.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	struct BbsThreadSummary
	{
		int32_t local_id = 0;
		int32_t poster_cid = 0;
		std::string subject;
		int64_t timestamp = 0;
		int32_t icon = 0;
		int32_t reply_count = 0;
		bool is_notice = false;
	};

	struct BbsReply
	{
		int32_t reply_id = 0;
		int32_t poster_cid = 0;
		int64_t timestamp = 0;
		std::string content;
	};

	struct BbsThreadDetail
	{
		int32_t local_id = 0;
		int32_t poster_cid = 0;
		int64_t timestamp = 0;
		std::string subject;
		std::string body;
		int32_t icon = 0;
		std::vector<BbsReply> replies;
	};

	class UIGuildBBS : public UIDragElement<PosGUILDBBS>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::GUILDBBS;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIGuildBBS();

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void doubleclick(Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		/// s2c mode 0x06：刷新列表（含可选公告条）
		/// page_index_hint < 0 时保留窗内已有页码（由 request_list 写入）
		void apply_list(bool has_notice, BbsThreadSummary notice,
			int32_t total_count, std::vector<BbsThreadSummary> page,
			int32_t page_index_hint = -1);
		/// s2c mode 0x07：打开读帖页
		void apply_thread(BbsThreadDetail detail);
		/// SoftKey / 公会窗入口：请求指定页列表
		void request_list(int32_t page = 0);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtQuit = 0,
			BtWrite,
			BtRetouch,
			BtDelete,
			BtRegister,
			BtCancel,
			BtNotice,
			BtReply,
			BtReplyDelete,
			BtPageLeft,
			BtPageRight
		};

		enum class Page : uint8_t { List, View, Write };

		static constexpr int16_t LIST_ROW_H = 22;
		static constexpr int16_t LIST_ROW_Y0 = 78;
		static constexpr int16_t LIST_MAX_ROWS = 12;
		static constexpr int16_t BTN_Y_LIST = 488;

		void set_page(Page p);
		void refresh_button_visibility();
		int16_t list_row_at(Point<int16_t> rel) const;
		const BbsThreadSummary* selected_summary() const;
		void prompt_write(bool as_notice, bool edit);
		void prompt_reply();
		void dispatch_delete_selected();
		void dispatch_delete_reply_selected();

		bool has_assets = false;
		Page page = Page::List;
		Texture backgrnd;
		Texture backgrnd2;
		Texture backgrnd3;
		Texture emoticon_basic[3];
		Texture emoticon_select;

		bool has_notice = false;
		BbsThreadSummary notice;
		std::vector<BbsThreadSummary> threads;
		int32_t total_threads = 0;
		int32_t page_index = 0;
		int selected_row = -1;
		int selected_reply = -1;
		int32_t write_icon = 0;
		bool write_as_notice = false;
		bool write_edit = false;
		int32_t edit_local_id = 0;

		BbsThreadDetail current;

		mutable OutlinedText title_label;
		mutable OutlinedText notice_label;
		mutable OutlinedText tip_label;
		mutable Text body_label;
	};
}

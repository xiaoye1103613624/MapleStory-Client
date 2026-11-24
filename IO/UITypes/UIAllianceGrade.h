////////////////////////////////////////////////////////////////////////////////
//	CWndAllianceGrade 原生还原：联盟五段职衔名编辑窗
//
//	Angel 083：OnCreate sub_917546 / OnButtonClicked @0x9181e7 /
//	OnChangeGradeName @0x918204 / OnSaveGradeName @0x91848d
//	入口：CUIUserList 联盟页 0x809（GuildUnion BtInfo 位）Toggle。
//
//	与 UNION 底栏 BtUp/BtDown（成员职级升降 action 0x09）不同：
//	本窗改的是联盟 5 段职衔称号字符串（action 0x08）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <string>

namespace ms
{
	class UIAllianceGrade : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ALLIANCE_GRADE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIAllianceGrade();

		void draw(float inter) const override;
		Cursor::State send_cursor(bool pressed, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Button::State button_pressed(uint16_t buttonid) override;
		UIElement::Type get_type() const override;

		/// 从 AllianceModel 重载五段职衔（0x1A 入站或开窗时）。
		void reload_from_model();
		/// UIEnterString 回调：写入当前选中职级名（禁 SoftKeyboard）。
		void set_selected_grade_name(std::string name);
		/// UIYesNo 确认后发包 0x08。
		void confirm_save_grade_names();

	private:
		enum Buttons : uint16_t
		{
			BtClose,
			BtMod,		// Angel 2140 → OnChangeGradeName
			BtSave		// Angel 2141 → OnSaveGradeName
		};

		// Angel GetGradeIndexFromPoint（v95 @0x8b7040 同构）：y=75 起、行高 20、共 5 行。
		int32_t grade_index_at(Point<int16_t> cursorpos) const;
		void on_change_grade_name();
		void on_save_grade_name();

		Texture background;
		Texture grade_row[5];
		bool has_background = false;
		Text grade_labels[5];
		std::string grades[5];
		/// 1..5；0 = 未选中（Angel this+31 / m_nSelected）。
		int32_t selected = 0;
	};
}

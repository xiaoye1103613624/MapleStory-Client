////////////////////////////////////////////////////////////////////////////////
//	Wave A · 学院（Family）信息窗 —— 2026-11-07 按原生「学院.png」重排
//
//	术语钉死（用户指正）：本窗 = 中文客户端「学院」= s9 Family 系统
//	（同学/名声度/名人冒险家特权）。与「家族」= Guild（公会，UIUserList 第 3 页）
//	是两个不同系统，勿再混。
//
//	布局事实源：
//	  底图 UIWindow.img/Family/backgrnd 224x392（烘焙「学院」标题、名声度/今天
//	  获得的名声标签、特权面板框、[01/11] 计数框 —— 这些不重绘）。
//	  动态件落位按 docs/features/social-loop-audit-2026-11/evidence/
//	  academy_top_3x.png / academy_priv_3x.png 行带实测（tools/_measure_academy.py）：
//	    蓝横幅名行  y≈24..50（居中）      → 有学院名 / 「(还没有冒险学院)」
//	    箴言白行    y≈58..70（左 8）      → family.message
//	    同学 x≈112 y≈77..92（右对齐）     → juniors "/" juniors_max
//	    名声度值    右缘 x≈212 y≈107..122 → cur_rep "/" tot_rep
//	    今日名声    右缘 x≈212 y≈124..139 → "+" today_rep
//	    特权名行    y≈156..168（居中）     → defs[cur].name
//	    消耗/限制   y≈178..190            → rep_cost / used+1 "/" usage_limit
//	    描述多行    y≈196..300            → desc（\n 分行，#c 色标剥离）
//	  钮（BtLeft/BtRight 13x13 在计数框两侧，实测 y≈150；BtSpecial 使用特权
//	  右下；BtJuniorEntry/BtFamilyPrecept 同学行；BtTree 谱系；BtOK 确认）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Character/FamilyModel.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	class UIFamily : public UIDragElement<PosFAMILY>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::FAMILY;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIFamily();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		/// 从 FamilyModel 拉全量刷新（入站 handler 只写模型，窗在场才调这里）。
		void refresh_from_model();

		int32_t selected_privilege() const { return priv_index; }

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		void page_privilege(int8_t delta);
		void use_selected();
		std::string priv_used_text() const;

		enum Buttons : uint16_t { BtClose, BtOK, BtTree, BtLeft, BtRight, BtJunior, BtPrecept, BtSpecial };

		bool has_assets;
		Texture backgrnd;
		FamilyInfoData info;
		// 原生：标题/横幅名是白字（蓝底上）；数值右对齐黑字。
		Text banner_label;		// 学院名 / (还没有冒险学院)
		Text precept_label;		// 箴言（白行）
		Text junior_label;		// 同学 0/2
		Text rep_label;			// 名声度 值
		Text today_label;		// 今天获得的名声 +值
		Text priv_title_label;	// 特权名（计数框行）
		Text priv_cost_label;		// 消耗名声 值（右对齐）
		Text priv_limit_label;	// 限制次数 值（右对齐）
		Text priv_desc_label;		// 描述（多行）
		int32_t priv_index = 0;
	};
}

//////////////////////////////////////////////////////////////////////////////////
//  UIEnhance — 数据表驱动的通用强化窗（UI_004 族 1..20）
//
//  设计依据：docs/features/ui004-enhancement-ui-port-2026-11/CONTRACT.md
//  「族 id 表」定义了 action 1..20 与 UI 节点名的映射；本类只做 UI 壳，
//  不持有任何 opcode / 概率逻辑，发包统一走 root agent 提供的
//  EnhanceActionPacket（Net/Packets/EnhancePackets.h）。
//
//  参考实现：IO/UITypes/UIViciousHammer.{h,cpp}
//  —— 资源缺失 has_assets=false 优雅降级、Texture 原点 shift、
//     按钮 bottom-row 定位、E 键从 UIItemInventory 取装备槽，均沿用其模式。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <cstdint>
#include <string>

namespace ms
{
	class UIEnhance : public UIDragElement<PosENHANCE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ENHANCE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		// action 取 CONTRACT.md 族 id 表 1..20；越界值在 cpp 内退到族 1 元数据。
		explicit UIEnhance(uint8_t action = 1);

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;
		UIElement::Type get_type() const override;

		// 结果反馈：0=失败 / 1=成功 / 2=破坏（契约 S→C result 字段）。
		// 文案走 Locale::text_or("ench.result.N", 简单中文回退)，缺键不显示裸 key。
		void apply_result(uint8_t result);

		void set_status(const std::string& msg);

		// 子模式，对应契约请求包 mode：0=执行 / 1=预览 / 2=应用预览。默认 0。
		void set_mode(uint8_t m) { mode = m; }

		uint8_t get_action() const { return action; }

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		// 候选按钮：节点有哪个就加载哪个。BtCancel 关窗，其余一律视为「确认发包」。
		// 之所以不按族分派按钮语义：契约里 20 个族共用同一 EnhanceActionPacket，
		// 差异全在 action 字段；按钮名差异只影响外观，壳层无需知道业务细节。
		enum Buttons : uint16_t {
			BtOk,
			BtStart,
			BtConfirm,
			BtCancel,
			BtOnemore,
			BtPurify
		};

		// 族元数据在 cpp 里以 static 数组给出；这两个小函数只是把内部表的查询
		// 封装起来，供初始化列表（hint 不能在函数体里赋）与 ctor 使用。
		static const char* node_name_for(uint8_t action);
		static std::string hint_for(uint8_t action);

		uint8_t action;
		uint8_t mode;
		int16_t equip_slot;
		int32_t equip_itemid;
		int16_t cashSlot;
		int32_t cashId;
		int16_t matSlot;   // 第二材料槽（MasterPiece 铁砧），0=无

		bool has_assets;
		Texture backgrnd;
		Texture equip_icon;
		Texture cash_icon;
		Text hint;         // 族默认提示（元数据 locale key 解析后的文案）
		Text status;       // 动态状态：装备槽 / 材料 / 发包 / 结果

		Rectangle<int16_t> drop_area() const;
		void send_action();  // 统一构造并 dispatch EnhanceActionPacket
	};
}

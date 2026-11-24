//////////////////////////////////////////////////////////////////////////////////
//  UIEnhance — 数据表驱动的通用强化窗（UI_004 族 1..20）
//
//  所有族共用一套 UI 壳与同一个请求包 EnhanceActionPacket，差异只在：
//    1. action（族 id）—— 决定加载哪张 UIWindow2Enh.img 背景与默认提示；
//    2. 节点里有哪些按钮 —— 节点有哪个加载哪个；
//    3. mode（子模式）—— 预览/应用预览，由外部 set_mode 设置。
//
//  本文件不含 opcode / 概率逻辑（协议由 root agent 负责），只负责采集槽位、
//  绘制资源与构造契约签名的数据包。
//////////////////////////////////////////////////////////////////////////////////
#include "UIEnhance.h"
#include "UIItemInventory.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/EnhancePackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"      // LOG 宏（参考 UIViciousHammer.cpp 的 include）
#include "../../Util/Locale.h"

#include <memory>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 族元数据表 —— 与 CONTRACT.md「族 id 表」逐行对应。
		// 每项：{ action id, UI 节点名, 默认提示 locale key }。
		// 说明：
		//   · 节点名来自契约表「UI 节点」列，少数族在契约里给了多个候选（如
		//     CUBE_MIRACLE = MiracleCube / Amazing / Master / ...），这里取表中
		//     列出的第一个节点名作为主节点；UiAssetResolve::window_enh 自带
		//     UIWindow2Enh → UIWindow2 → UIWindow 回退，缺节点时优雅降级。
		//   · 提示 key 统一用 "ench.hint.<族常量小写>"；locale 未提供时
		//     hint_for() 回退到简单中文文案，绝不显示裸 key。
		struct FamilyMeta
		{
			uint8_t action;
			const char* node_name;
			const char* hint_key;
		};

		constexpr FamilyMeta FAMILIES[] = {
			{  1, "Cube_Red",                 "ench.hint.cube_red" },
			{  2, "Cube_Black",                "ench.hint.cube_black" },
			{  3, "MiracleCube_Platinum",      "ench.hint.cube_platinum" },
			{  4, "MiracleCube_Additional",    "ench.hint.cube_additional" },
			{  5, "AddtionalCube_White",       "ench.hint.cube_white" },
			{  6, "MiracleCube",               "ench.hint.cube_miracle" },
			{  7, "HyperMiracleCube",          "ench.hint.cube_hyper" },
			{  8, "NewHexaCube",               "ench.hint.cube_hexa" },
			{  9, "CN_10thHugeCube",           "ench.hint.cube_10th" },
			{ 10, "GoldHammer",                "ench.hint.hammer_gold" },
			{ 11, "WhiteGoldHammer",           "ench.hint.hammer_white" },
			{ 12, "PotentialStamp",            "ench.hint.potential_stamp" },
			{ 13, "AdditionalPotentialStamp",  "ench.hint.additional_stamp" },
			{ 14, "stonePurify",               "ench.hint.stone_purify" },
			{ 15, "jewelCraft",                "ench.hint.jewel_craft" },
			{ 16, "SynergyMachine",            "ench.hint.synergy" },
			{ 17, "ScrollSynergyMachine",      "ench.hint.scroll_synergy" },
			{ 18, "itemPot",                   "ench.hint.item_pot" },
			{ 19, "MasterPiece",               "ench.hint.masterpiece" },
			{ 20, "SoulCollector",             "ench.hint.soul_collector" }
		};

		// 按 action 查表；找不到（越界/0）时退到族 1，保证 UI 永远有背景可画。
		const FamilyMeta& meta_for(uint8_t action)
		{
			for (const FamilyMeta& m : FAMILIES)
			{
				if (m.action == action)
					return m;
			}

			return FAMILIES[0];
		}

		// 槽位提示：键存在走 Locale::format（支持 "{}" 占位），缺键直接拼
		// 简单中文回退 + 数字，避免界面出现 "enh.equip_slot12" 这种裸 key。
		std::string slot_text(const char* key, const char* fallback, int32_t slot)
		{
			if (Locale::get().has(key))
				return Locale::format(key, std::to_string(slot));

			return std::string(fallback) + std::to_string(slot);
		}
	}

	const char* UIEnhance::node_name_for(uint8_t a)
	{
		return meta_for(a).node_name;
	}

	std::string UIEnhance::hint_for(uint8_t a)
	{
		const FamilyMeta& m = meta_for(a);

		// 缺键回退到简单中文文案（Locale::text_or，Util/Locale.h:63-69）。
		// 文案不带道具号段/概率——那是服务端配置，不属于 UI 壳。
		const char* fallback = nullptr;
		switch (m.action)
		{
			case  1: fallback = "红魔方：放入装备后点击确认重洗潜能"; break;
			case  2: fallback = "黑魔方：放入装备后重洗，可对比前后潜能"; break;
			case  3: fallback = "白金魔方：放入装备后把潜能拉满当前等级"; break;
			case  4: fallback = "附加潜能魔方：放入装备后重洗附加潜能"; break;
			case  5: fallback = "白色附加魔方：放入装备后可对比前后附加潜能"; break;
			case  6: fallback = "神奇魔方：放入装备后重洗潜能"; break;
			case  7: fallback = "超级魔方：放入装备后重洗潜能"; break;
			case  8: fallback = "六角魔方：放入装备后确认重洗"; break;
			case  9: fallback = "十周年巨大魔方：放入装备后重洗潜能"; break;
			case 10: fallback = "金锤：放入装备后点击开始升级"; break;
			case 11: fallback = "白金锤：放入装备后点击开始升级"; break;
			case 12: fallback = "潜能印章：放入装备后增加一条潜能"; break;
			case 13: fallback = "附加潜能印章：放入装备后增加一条附加潜能"; break;
			case 14: fallback = "石净化：放入装备后点击净化"; break;
			case 15: fallback = "宝石炼金：放入材料后炼制宝石"; break;
			case 16: fallback = "协同机：放入材料后进行协同"; break;
			case 17: fallback = "卷轴协同机：放入材料后进行卷轴协同"; break;
			case 18: fallback = "物品罐：放入材料后开启"; break;
			case 19: fallback = "大师作品：放入装备与材料后合成"; break;
			case 20: fallback = "灵魂收集：放入装备后收集灵魂"; break;
			default: fallback = "放入装备或材料后点击确认"; break;
		}

		return Locale::text_or(m.hint_key, fallback);
	}

	UIEnhance::UIEnhance(uint8_t a)
		: UIDragElement<PosENHANCE>()
		, action(a)
		, mode(0)
		, equip_slot(0)
		, equip_itemid(0)
		, cashSlot(0)
		, cashId(0)
		, matSlot(0)
		, has_assets(false)
		, hint(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, hint_for(a))
		, status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "")
	{
		// 坐标初值参考 UIViciousHammer；真正尺寸在拿到 backgrnd 后覆盖。
		position = Point<int16_t>(320, 140);
		dimension = Point<int16_t>(200, 240);
		dragarea = Point<int16_t>(dimension.x(), 20);

#ifdef USE_NX
		// 族元数据表决定加载哪张面板；window_enh 自带回退链。
		nl::node root = UiAssetResolve::window_enh(node_name_for(a));
		if (!root) {
			LOG(LOG_WARN, "[DBG-ENH] UIEnhance: missing action=" << static_cast<int>(a)
				<< " node=" << node_name_for(a));
			return;
		}
		has_assets = true;

		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();

			// 与 UIViciousHammer 一致：有非零原点就 shift 回 (0,0)，
			// 使 position 就是窗口左上角，按钮定位可直接用 (x, dimension.y()-h)。
			if (o.x() || o.y())
				backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}

		// 按契约列出的候选按钮名逐个探测，节点有哪个加载哪个。
		// 统一放在底边一行，间距 6px；按钮实际宽高由 MapleButton 从节点读取，
		// 这里只给左上角锚点，不要求像素级对齐（CONTRACT 参考 Charlotte/Alignment）。
		const int16_t by = static_cast<int16_t>(dimension.y() - 34);
		int16_t bx = 12;
		const int16_t gap = 6;
		const char* candidates[] = {
			"BtOk", "BtStart", "BtConfirm", "BtCancel", "BtOnemore", "BtPurify"
		};
		const uint16_t ids[] = {
			BtOk, BtStart, BtConfirm, BtCancel, BtOnemore, BtPurify
		};

		for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
			if (!root[candidates[i]])
				continue;
			buttons[ids[i]] = std::make_unique<MapleButton>(root[candidates[i]], Point<int16_t>(bx, by));
			// 粗估按钮宽度 64px 作为下一颗的起点：宁可略微收紧也不要越出窗口右边界。
			bx += 70 + gap;
			if (bx + 64 > dimension.x())
				bx = 12;   // 超出右边界就换行/回到左侧，保证按钮始终在窗口内
		}

		LOG(LOG_DEBUG, "[DBG-ENH] UIEnhance: open action=" << static_cast<int>(a)
			<< " node=" << node_name_for(a)
			<< " dim=" << dimension.x() << "x" << dimension.y()
			<< " buttons=" << buttons.size());
#endif
	}

	Rectangle<int16_t> UIEnhance::drop_area() const
	{
		// 背景中央的装备放置区，尺寸参考 UIViciousHammer 的 48x48。
		return {
			position + Point<int16_t>(40, 56),
			position + Point<int16_t>(40 + 48, 56 + 48)
		};
	}

	void UIEnhance::set_status(const std::string& msg)
	{
		status.change_text(msg);
	}

	bool UIEnhance::send_icon(const Icon& icon, Point<int16_t> cursor)
	{
		Rectangle<int16_t> win{ position, position + dimension };
		if (!drop_area().contains(cursor) && !win.contains(cursor))
			return true;

		const InventoryType::Id tab = icon.get_inventory_tab();

		if (tab == InventoryType::Id::EQUIP) {
			// 与 UIViciousHammer.cpp:81-98 一致：EQUIP 图标设目标装备槽。
			equip_slot = icon.get_inventory_slot();
			equip_itemid = icon.get_item_id();
			equip_icon = ItemData::get(equip_itemid).get_icon(false);
			status.change_text(slot_text("enh.equip_slot", "装备槽 ", equip_slot));
			LOG(LOG_DEBUG, "[DBG-ENH] UIEnhance: equip slot=" << equip_slot
				<< " id=" << equip_itemid);
		} else if (tab == InventoryType::Id::CASH || tab == InventoryType::Id::USE) {
			// CASH/USE 图标设消耗材料槽（魔方/锤/印章/炼金材料等）。
			cashSlot = icon.get_inventory_slot();
			cashId = icon.get_item_id();
			cash_icon = ItemData::get(cashId).get_icon(false);
			status.change_text(slot_text("enh.cash_slot", "材料槽 ", cashSlot));
			LOG(LOG_DEBUG, "[DBG-ENH] UIEnhance: cash slot=" << cashSlot
				<< " id=" << cashId);
		} else {
			// 其它页签（SETUP/ETC 等）不参与强化，明确提示，不静默吞掉。
			status.change_text(Locale::text_or("enh.need_equip_or_cash", "请拖入装备或消耗材料"));
		}

		return true;
	}

	void UIEnhance::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);

		hint.draw(position + Point<int16_t>(12, 12));
		status.draw(position + Point<int16_t>(12, 30));

		if (equip_icon)
			equip_icon.draw(position + Point<int16_t>(48, 58));
		if (cash_icon)
			cash_icon.draw(position + Point<int16_t>(104, 58));

		UIElement::draw_buttons(inter);
	}

	void UIEnhance::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			deactivate();
			return;
		}
		if (!pressed)
			return;

		// Enter / 小键盘 Enter：确认发包（参考 UIViciousHammer.cpp:112-137 的 13）。
		if (keycode == 13) {
			button_pressed(BtOk);
			return;
		}

		// E 键：从 UIItemInventory 当前选中的 EQUIP 槽取装备。
		if (keycode == 'E' || keycode == 'e') {
			int16_t from_inv = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				if (inv->get_tab() == InventoryType::Id::EQUIP)
					from_inv = inv->get_selected_slot();
			}
			if (from_inv > 0) {
				equip_slot = from_inv;
				equip_itemid = Stage::get().get_player().get_inventory()
					.get_item_id(InventoryType::Id::EQUIP, from_inv);
				equip_icon = ItemData::get(equip_itemid).get_icon(false);
				status.change_text(slot_text("enh.equip_num", "装备 #", from_inv));
				LOG(LOG_DEBUG, "[DBG-ENH] UIEnhance: SoftKey equip slot=" << from_inv);
			} else {
				status.change_text(Locale::text_or("enh.drop_first", "请先放入装备"));
			}
		}
	}

	UIElement::Type UIEnhance::get_type() const { return TYPE; }

	void UIEnhance::send_action()
	{
		// 统一发包口：所有按钮的「确认」语义都走到这里，字段顺序严格按契约
		// （CONTRACT.md C→S 请求包定义）：
		//   byte action / short cashSlot / int cashId / short equipSlot
		//   / short matSlot / byte mode
		LOG(LOG_DEBUG, "[DBG-ENH] UIEnhance: dispatch action=" << static_cast<int>(action)
			<< " cashSlot=" << cashSlot << " cashId=" << cashId
			<< " equipSlot=" << equip_slot << " matSlot=" << matSlot
			<< " mode=" << static_cast<int>(mode));
		EnhanceActionPacket(action, cashSlot, cashId, equip_slot, matSlot, mode).dispatch();
		status.change_text(Locale::text_or("enh.sent", "已发送强化请求"));
	}

	Button::State UIEnhance::button_pressed(uint16_t buttonid)
	{
		switch (buttonid) {
			case BtCancel:
				deactivate();
				break;
			case BtOk:
			case BtStart:
			case BtConfirm:
			case BtOnemore:
			case BtPurify:
				send_action();
				break;
			default:
				break;
		}
		return Button::State::NORMAL;
	}

	void UIEnhance::apply_result(uint8_t result)
	{
		// 结果码与契约 S→C 一致：0=失败 / 1=成功 / 2=破坏。
		// 14..20 族是道具融合/合成类（石净化/炼金/协同/卷轴/药罐/铁砧/灵魂），
		// 服务端回包只带 result、不带产出物 id，故客户端按 action 给贴切文案，
		// 具体产出由背包刷新体现（诚实边界：不在本壳里编造产出物显示）。
		const bool item_family = (action >= 14 && action <= 20);
		const char* key = nullptr;
		const char* fallback = nullptr;
		switch (result) {
			case 0:
				key = item_family ? "ench.result.item_fail" : "ench.result.0";
				fallback = item_family ? "操作失败，材料未消耗" : "强化失败";
				break;
			case 1:
				if (item_family) {
					// 按族给专属成功文案；Locale 缺键时由下面的中文回退兜底。
					switch (action) {
						case 14: key = "ench.result.item.purify";  fallback = "净化完成，请查看背包"; break;
						case 15: key = "ench.result.item.jewel";   fallback = "宝石合成完成"; break;
						case 16: key = "ench.result.item.synergy"; fallback = "协同合成完成"; break;
						case 17: key = "ench.result.item.scroll";  fallback = "卷轴合成完成"; break;
						case 18: key = "ench.result.item.pot";     fallback = "药剂已填充"; break;
						case 19: key = "ench.result.item.anvil";   fallback = "装备合成完成"; break;
						case 20: key = "ench.result.item.soul";    fallback = "灵魂操作完成"; break;
						default: key = "ench.result.item.ok";      fallback = "操作完成"; break;
					}
				} else {
					key = "ench.result.1"; fallback = "强化成功";
				}
				break;
			case 2:
				key = "ench.result.2"; fallback = "装备已破坏";
				break;
			default:
				key = "ench.result.unknown"; fallback = "未知结果";
				break;
		}
		status.change_text(Locale::text_or(key, fallback));
		LOG(LOG_DEBUG, "[DBG-ENH] UIEnhance: apply_result=" << static_cast<int>(result)
			<< " action=" << static_cast<int>(action));
	}
}

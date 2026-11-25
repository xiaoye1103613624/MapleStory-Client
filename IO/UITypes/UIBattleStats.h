//////////////////////////////////////////////////////////////////////////////////
// UIBattleStats — 战斗统计窗（192 原生 Battle Analysis 移植 / B2 服务端归因版）
//
// 资源（独立 img，已转 GMS key 部署）：
//   UI/UIWindow4_battlestats.img/battleStats/{main,mini}
//   main backgrnd 981×615 / mini backgrnd 211×182（ARGB4444，orange-wz verify 通过）
//   原生按钮 id 1000..1006：Close/Mini/Help/Start/End/Left/Right
//   combo:Sort 7 项排序（队列/累计伤害/使用次数/平均伤害/攻击次数/最高/最低）
//
// 数据：Character/BattleStats.h 单例，由 0x3BC(许可) + 0x3BD(队伍数据) 驱动。
// 发包：0x3BC 3×byte(bOn,bIsNew,bClear)（构造即发，见 Net/Packets/BattleStatsPackets.h）。
//
// 双模式：main（完整，含逐队员行 + 排序）与 mini（精简 4 字段），
//   对应原生 button:Mini / button:Main 互切。
// 所有面向人展示文案走 Locale/cn（缺词条用中文 fallback，不直塞 GBK）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Character/BattleStats.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIBattleStats : public UIDragElement<PosBATTLESTATS>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::BATTLESTATS;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIBattleStats();

		void draw(float inter) const override;
		void update() override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void remove_cursor() override;
		UIElement::Type get_type() const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		// 由 0x3BD handler 调用；active 时刷新行缓存。
		void reload();

	private:
		// 原生按钮 id（与资源 id 属性一致）。
		enum Buttons : uint16_t
		{
			BtClose = 1000,
			BtMini = 1001,
			BtHelp = 1002,
			BtStart = 1003,
			BtEnd = 1004,
			BtLeft = 1005,
			BtRight = 1006,
			BtMain = 1001  // mini 模式下 1001 复用作"最大化"
		};

		enum class Mode : uint8_t { Main, Mini };
		enum class SortKey : uint8_t { Order, Damage, Count, AvgDamage, AttackNum, MaxDamage, MinDamage };

		// 原生 main 981×615 / mini 211×182。
		static constexpr int16_t MAIN_W = 981;
		static constexpr int16_t MAIN_H = 615;
		static constexpr int16_t MINI_W = 211;
		static constexpr int16_t MINI_H = 182;

		static constexpr int16_t ROW_H = 20;      // 逐队员/技能行高
		static constexpr int16_t ROW_X = 30;
		static constexpr int16_t ROW_Y = 100;
		static constexpr int16_t MAX_ROWS = 16;

		void load_assets();
		bool load_tex(Texture& out, nl::node n) const;
		void rebuild_rows();
		void send_request(bool on, bool isNew, bool clear) const;
		void close();
		int16_t current_w() const { return mode == Mode::Main ? MAIN_W : MINI_W; }
		int16_t current_h() const { return mode == Mode::Main ? MAIN_H : MINI_H; }

		// 把一行数据格式化成 7 个排序维度里的显示值。
		std::string format_field(const BattleStats::Entry& e, SortKey key) const;
		static std::string fmt_num(uint64_t v);

		Mode mode = Mode::Main;
		SortKey sort_key = SortKey::Damage;
		bool assets_loaded = false;
		bool has_assets = false;
		Texture bg_main;
		Texture bg_main2;
		Texture bg_mini;
		std::vector<Texture> btns;   // 索引 = 按钮 id，四态用 Texture 组管理（简化：normal 态）
		Text title_text;
		Text empty_text;
		std::vector<Text> row_texts;
		std::vector<BattleStats::Entry> rows;
		int16_t page = 0;
		int64_t local_start_ms = 0;   // 本地计时兜底起点
	};
}

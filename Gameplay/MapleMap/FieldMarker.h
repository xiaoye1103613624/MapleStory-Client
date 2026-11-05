//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — lightweight field placeholder (summon/mist/door/kite/dragon)
//	G11：Catch / Chalkboard / CharBox / ItemFX 贴图化
//	Kite：Cash/0508 iconReward（IDA SPAWN_KITE sub_65ACDF）
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Layer.h"
#include "MapObject.h"

#include "../../Graphics/Animation.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>

namespace ms
{
	// Placeholder map object until full Summon/Mist/Door sprites land.
	// Tries Skill.wz effect/summon animation when skill_id != 0.
	class FieldMarker : public MapObject
	{
	public:
		enum class Kind : uint8_t
		{
			SUMMON,
			MIST,
			DOOR,
			KITE,
			DRAGON,
			// G7：动态传送门（SPAWN_PORTAL，镇侧门框/神秘门目标端）。
			// 与 DOOR 分开：NONE 清除命令按本 kind 全清，不误删真门/文本场标。
			PORTAL,
			// G11：捕获圈 / 黑板 / 店招 / 道具特效（Effect/Cash 真件）
			CATCH,
			CHALKBOARD,
			CHARBOX,
			ITEMFX
		};

		FieldMarker(int32_t oid, Kind kind, Point<int16_t> pos, const std::string& label, int32_t skill_id = 0, int32_t lifetime_ms = 0);

		void draw(double viewx, double viewy, float alpha) const override;
		int8_t update(const Physics& physics) override;
		int8_t get_layer() const override;

		void set_label(const std::string& label);
		// 受击：召唤兽（MapSummon）要播 hit 帧，普通场标只更新 HP 文字 ⇒ 虚化。
		virtual void apply_damage(int32_t damage);
		Kind get_kind() const;

		// 派生真对象（MapSummon 自持状态机、覆写 draw/update）需要名牌，
		// 故把 label / base_label 降到 protected。其余仍是占位实现细节。
	protected:
		Text label;
		std::string base_label;

	private:
		void try_load_anim(int32_t skill_id);
		void try_load_door();
		void try_load_mist();
		void try_load_dragon();
		// item_id=508xxxx → iconReward 动画；其它/0 → 默认 5080000 再回退 ItemUp
		void try_load_kite(int32_t item_id);
		void try_load_catch(bool success);
		void try_load_chalkboard();
		void try_load_charbox();

		Kind kind;
		Animation anim;
		bool has_anim = false;
		Texture board;
		bool has_board = false;
		int32_t lifetime_ms = 0;
		int32_t hp_cur = -1;
		static constexpr int8_t DRAW_LAYER = Layer::Id::SIX;
	};
}

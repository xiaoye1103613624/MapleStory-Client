//////////////////////////////////////////////////////////////////////////////////
//	DropAuras — 装备掉落品质光环素材（2026-11-12 装备槽位扩展 ⑥）
//
//	素材：Data/Effect/DropAuraEff.img（独立 .img，由
//	  E:\MXD\扩展改动\掉落特效\Effect.wz 经 orange-wz MCP
//	  create_img_file + copy_paste_nodes 抽出，116,221B）。
//	  为什么不是合并进 BasicEff.img：实测 copy_paste→save_as 到已存在的
//	  大 .img 时 orange-wz 只落脏节点，产出文件从 41 个顶层节点缩成 2 个
//	  （门禁 verify_ingest_integrity.py 判 FAIL），会毁掉整个 Effect 根。
//	  ImgSource.cpp:261-294 gather() 递归扫描根目录下所有 .img 并
//	  open_merge（:306），所以新增独立文件即可挂到 nl::nx::Effect，
//	  BasicEff.img 一字节都不动（3,992,551B / 2024-08-04 保持原样）。
//
//	层级（与参考实现 dropitemaura.cpp:133-134 的路径一致，仅根文件名不同）：
//	  Effect/DropAuraEff.img/dropItemAura/{Rare,Epic,Unique,Legendary}/{front,back}/{0..5}
//	  Effect/DropAuraEff.img/dropItemEffect/{0..5}
//	  实测每帧 6 个、delay=90、origin/z 齐备 → Animation 自带帧推进，无需定时器。
//
//	grade 语义（参考 E:\MXD\扩展改动\掉落特效\PacketCreator.java::getDropItemGrade）：
//	  -1 负差值 / 0 普通 / 1 Rare / 2 Epic / 3 Unique / 4 Legendary
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include "../../Graphics/Animation.h"

#include <array>

namespace ms
{
	class DropAuras
	{
	public:
		// 与下行包 grade 字节对齐：0=普通（无光环），1..4 对应四档素材。
		static constexpr int8_t GRADE_RARE = 1;
		static constexpr int8_t GRADE_EPIC = 2;
		static constexpr int8_t GRADE_UNIQUE = 3;
		static constexpr int8_t GRADE_LEGENDARY = 4;
		static constexpr int8_t GRADE_MAX = 4;

		static DropAuras& get();

		void init();

		// 素材是否存在（缺失时全部降级为「不画光环」，不崩、不空指针）。
		bool has_grade(int8_t grade) const;
		// 该档位的 back 层是否可用（front 可用不代表 back 可用，逐层判）。
		bool has_back(int8_t grade) const;
		bool has_pillar() const;

		const Animation& front(int8_t grade) const;
		const Animation& back(int8_t grade) const;
		const Animation& pillar() const;

	private:
		DropAuras();

		// index = grade（0 槽位不使用，保持无效 Animation）。
		// 有效性单独记账：Animation 没有 operator bool（Graphics/Animation.h:60-95），
		// 默认构造只有一个空 Frame，无法从对象本身区分「没素材」和「有素材」。
		std::array<Animation, GRADE_MAX + 1> fronts;
		std::array<Animation, GRADE_MAX + 1> backs;
		std::array<bool, GRADE_MAX + 1> front_ok{};
		std::array<bool, GRADE_MAX + 1> back_ok{};
		Animation fall;
		bool fall_ok = false;
		bool loaded = false;
	};
}

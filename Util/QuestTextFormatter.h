////////////////////////////////////////////////////////////////////////////////
//  QuestTextFormatter —— 富文本令牌展开（共享，无状态）
////////////////////////////////////////////////////////////////////////////////
//  背景（r07 之后 r08 补齐）：冒险岛的字符串资源里内嵌 #tag[id]# 令牌，
//  GraphicsGL::LayoutBuilder 只会吃掉 '#p'/'#t' 前缀与闭合 '#'，把中间的裸数字
//  当正文画出来 —— 任务窗里于是出现「从9310088那里收到了紧急信息」这种裸 ID。
//  正确做法是像 UINpcTalk::format_text 那样在构造 Text 之前先把令牌展开成可读串：
//    #p/#o/#m/#q/#t/#z/#h → 名字；#c → 持有数（保留色标）；#x → 进度；
//    #b/#k/#r/#g/#d/#e/#n 等色标/字重标记由 LayoutBuilder 处理，原文保留。
//
//  为什么单独抽出来：原 UINpcTalk::format_text 末尾会调 strip_simple_choices()
//  （依赖对话框自身的 simple_choices 成员），不能直接给任务窗用。这里按与它
//  **相同的令牌语义**做一份无状态实现供任务窗使用；刻意不改 UINpcTalk 那条
//  已验收的对话框路径（最小改动、零回归风险）。两处规则同源、各自独立。
//
//  已知数据缺口：live xy 的 String/Npc.img 不含 NPC 9310088（orange-wz MCP
//  10012 实查「节点路径不存在」），故 #p9310088# 会按原生语义吞掉令牌；
//  UIQuestLog 调用时传入可读占位（npcFallback）以免句子读起来缺主语。
//
//  NPC 名解析有个已知数据缺口：部分私服包（live xy）的 String/Npc.img 不含
//  某些任务 NPC（如 9310088），此时按原生语义「取不到就吞掉令牌、不留裸数字」，
//  由 fallbackName 参数给调用方一个机会用 locale 的可读占位补上。
#pragma once

#include <cstdint>
#include <string>

namespace ms
{
	class QuestTextFormatter
	{
	public:
		// 展开 #tag...# 令牌为可读文本。
		//   npcid        : '#p' 无参数时（#p#）回退用的当前 NPC id（对话框传说话人；任务窗传 0）。
		//   npcFallback  : NPC 名取不到（如 9310088 不在 String/Npc.img）时的可读占位；
		//                  空串 = 直接吞掉令牌（原生「解析失败掉落标记」语义）。
		//   keepSimpleChoices : UINpcTalk 传 false（末尾要剥 #L/#l 选项）；任务窗传 true
		//                  （任务描述里没有选项，且我们没有 simple_choices 状态）。
		static std::string format(const std::string& text, int32_t npcid,
			const std::string& npcFallback = std::string(), bool keepSimpleChoices = true);
	};
}

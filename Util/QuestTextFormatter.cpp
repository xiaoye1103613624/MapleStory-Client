////////////////////////////////////////////////////////////////////////////////
//  QuestTextFormatter —— 富文本令牌展开（共享，无状态）
////////////////////////////////////////////////////////////////////////////////
//  证据链与语义见 QuestTextFormatter.h。实现要点：
//   - 逐处展开（guard 256 防病态串），与 UINpcTalk::format_text C6 口径一致；
//   - 名字类令牌解析失败 → 吞掉整段 #tag...#，绝不把裸数字留给 LayoutBuilder
//     （GraphicsGL.cpp:1204-1244 只吃前缀、把数字当正文画，正是裸 ID 来源）；
//   - #c 展开成 "#c"+持有数，色标留给 LayoutBuilder（ORANGE）；
//   - 色标/字重/图标标记（#b/#k/#r/#i/#v/#s/#f/#B/...）原文保留，交给 Text 的
//     LayoutBuilder 绘制；#E/#I/#S/#K 指引元剥除（原生不进可见字）。
#include "QuestTextFormatter.h"

#include "../Gameplay/Stage.h"
#include "../Data/ItemData.h"
#include "../Data/SkillData.h"
#include "../Util/Misc.h"

#include <exception>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	std::string QuestTextFormatter::format(const std::string& text, int32_t npcid,
		const std::string& npcFallback, bool keepSimpleChoices)
	{
		std::string out = text;

		auto trim_body = [](std::string body) {
			while (!body.empty() && (body.front() == ' ' || body.front() == '\t'))
				body.erase(body.begin());
			while (!body.empty() && (body.back() == ' ' || body.back() == '\t'))
				body.pop_back();
			return body;
		};

		auto replace_named = [&](char tag, const auto& resolve) {
			size_t search = 0;
			for (int guard = 0; guard < 256; ++guard)
			{
				const size_t begin = out.find(std::string("#") + tag, search);
				if (begin == std::string::npos)
					break;
				const size_t end = out.find('#', begin + 2);
				if (end == std::string::npos)
					break;

				const std::string body = trim_body(out.substr(begin + 2, end - (begin + 2)));
				std::string replacement;
				const bool ok = resolve(body, replacement);
				if (!ok)
				{
					// 解析失败（id 不在数据里，如任务窗的 9310088）：吞掉整段令牌，
					// 原生即掉落该标记；否则裸数字会漏到屏幕。
					out.replace(begin, end - begin + 1, "");
					search = begin;
					continue;
				}
				out.replace(begin, end - begin + 1, replacement);
				search = begin + replacement.size();
			}
		};

		// #h → 玩家名（#h0# / #h# 同义，body 忽略）。
		replace_named('h', [&](const std::string&, std::string& rep) {
			rep = Stage::get().get_player().get_name();
			return true;
		});

		// #p → NPC 名。body 为空时用 npcid；解析失败回退到 npcFallback。
		replace_named('p', [&](const std::string& body, std::string& rep) {
			int32_t id = npcid;
			if (!body.empty())
			{
				try { id = std::stoi(body); }
				catch (const std::exception&) { return false; }
			}
			if (id <= 0)
			{
				rep = npcFallback;
				return !rep.empty();
			}
#ifdef USE_NX
			rep = nl::nx::String["Npc.img"][std::to_string(id)]["name"].get_string();
#endif
			if (rep.empty())
			{
				rep = npcFallback;
				return !rep.empty();
			}
			return true;
		});

		// #t / #z → 道具名（ItemData 全品类：Eqp/Consume/Ins/Etc/Cash）。
		auto resolve_item = [](const std::string& body, std::string& rep) {
			if (body.empty())
				return false;
			try
			{
				rep = ItemData::get(std::stoi(body)).get_name();
				return !rep.empty();
			}
			catch (const std::exception&)
			{
				return false;
			}
		};
		replace_named('t', resolve_item);
		replace_named('z', resolve_item);

		// #q → 技能名（IDA case17 + %07d → String/Skill.img）。
		replace_named('q', [](const std::string& body, std::string& rep) {
			if (body.empty())
				return false;
			try
			{
				rep = SkillData::get(std::stoi(body)).get_name();
				return !rep.empty();
			}
			catch (const std::exception&)
			{
				return false;
			}
		});

		// #m → 地图名（IDA case17 pool 1730 → sub_5CF792）。
		replace_named('m', [](const std::string& body, std::string& rep) {
			if (body.empty())
				return false;
			try
			{
				rep = NxHelper::Map::get_map_info_by_id(std::stoi(body)).name;
				return !rep.empty();
			}
			catch (const std::exception&)
			{
				return false;
			}
		});

		// #o → 怪物名（String/Mob.img/<id>/name），与 #p 同族正文名。
		replace_named('o', [](const std::string& body, std::string& rep) {
			if (body.empty())
				return false;
			try
			{
				const int32_t id = std::stoi(body);
				if (id <= 0)
					return false;
#ifdef USE_NX
				rep = nl::nx::String["Mob.img"][std::to_string(id)]["name"].get_string();
#endif
				return !rep.empty();
			}
			catch (const std::exception&)
			{
				return false;
			}
		});

		// #c → 持有数（保留 "#c" 色标，LayoutBuilder 对 #c 仍 ORANGE + skip+=2）。
		replace_named('c', [](const std::string& body, std::string& rep) {
			if (body.empty())
				return false;
			try
			{
				const int32_t id = std::stoi(body);
				if (id <= 0)
					return false;
				const int16_t n = Stage::get().get_player().get_inventory().get_total_item_count(id);
				rep = std::string("#c") + std::to_string(static_cast<int>(n));
				return true;
			}
			catch (const std::exception&)
			{
				return false;
			}
		});

		// #x → 任务进度（命中 +N / 未命中 N%，pool 1942/1941）。
		replace_named('x', [](const std::string& body, std::string& rep) {
			int b = 0, c = 0;
			try
			{
				if (!body.empty())
				{
					const int32_t qid = std::stoi(body);
					if (qid > 0 && qid <= 32767)
					{
						const std::string progress =
							Stage::get().get_player().get_quests().get_progress(static_cast<int16_t>(qid));
						if (!progress.empty())
						{
							size_t p0 = 0;
							int parts[3] = { 0, 0, 0 };
							int n = 0;
							for (; n < 3; ++n)
							{
								const size_t slash = progress.find('/', p0);
								const std::string piece = (slash == std::string::npos)
									? progress.substr(p0)
									: progress.substr(p0, slash - p0);
								try { parts[n] = std::stoi(piece); }
								catch (const std::exception&) { parts[n] = 0; }
								if (slash == std::string::npos) { ++n; break; }
								p0 = slash + 1;
							}
							if (n >= 3) { b = parts[1]; c = parts[2]; }
						}
					}
				}
			}
			catch (const std::exception&) { b = 0; c = 0; }
			rep = (b != 0) ? (std::string("+") + std::to_string(c))
				: (std::to_string(c) + "%");
			return true;
		});

		// #E/#I/#S/#K 指引元：原生吃成 CT 指引，不进可见字。
		for (size_t i = 0; i + 1 < out.size();)
		{
			if (out[i] == '#' && (out[i + 1] == 'E' || out[i + 1] == 'I'
				|| out[i + 1] == 'S' || out[i + 1] == 'K'))
			{
				out.erase(i, 2);
				continue;
			}
			++i;
		}

		// #L/#l 选项剥除留给 UINpcTalk（它要填 simple_choices 命中框）；任务窗
		// 描述里没有选项，按 keepSimpleChoices=true 原样保留色标/图标标记给 Text。
		(void)keepSimpleChoices;
		return out;
	}
}

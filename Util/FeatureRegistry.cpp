//////////////////////////////////////////////////////////////////////////////////
//	FeatureRegistry — open builtins used by SideMenu leaf bindings.
//////////////////////////////////////////////////////////////////////////////////
#include "FeatureRegistry.h"

#include "../IO/UI.h"
#include "../IO/UITypes/UIBreakthroughStone.h"
#include "../IO/UITypes/UIColorPrism.h"
#include "../IO/UITypes/UIDailyCheckin.h"
#include "../IO/UITypes/UIDailyGift.h"
#include "../IO/UITypes/UIDamageRank.h"
#include "../IO/UITypes/UIBattleStats.h"
#include "../IO/UITypes/UIAuction.h"
#include "../IO/UITypes/UIFamiliar.h"
#include "../IO/UITypes/UIDamageSkinPicker.h"
#include "../IO/UITypes/UIBeautySalon.h"
#include "../IO/UITypes/UIAnvilDialog.h"
#include "../IO/UITypes/UIEnhance.h"
#include "../IO/UITypes/UIEvent.h"
#include "../IO/UITypes/UIAlliance.h"
#include "../IO/UITypes/UIChatEmoticon.h"
#include "../IO/UITypes/UIFamily.h"
#include "../IO/UITypes/UIMaker.h"
#include "../IO/UITypes/UIMedal.h"
#include "../IO/UITypes/UIMonsterBook.h"
#include "../IO/UITypes/UIMysticBadge.h"
#include "../IO/UITypes/UIEquipInventory.h"
#include "../IO/UITypes/UINotice.h"
#include "../IO/UITypes/UIOptionMenu.h"
#include "../IO/UITypes/UIQuestLog.h"
#include "../IO/UITypes/UIWitchForest.h"
#include "../IO/UITypes/UIWindowCashShop.h"
#include "../IO/UITypes/UIWorldMap.h"
#include "../IO/UITypes/UIMapInfoPanel.h"
#include "../IO/UITypes/UIMiscShells.h"
#include "../Gameplay/Stage.h"
#include "Misc.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace ms
{
	namespace FeatureRegistry
	{
		namespace
		{
			std::string lower_copy(std::string s)
			{
				std::transform(s.begin(), s.end(), s.begin(),
					[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
				return s;
			}

			std::unordered_map<std::string, bool> remote_enable;

			struct Opener
			{
				Info info;
				std::function<void()> open;
			};

			std::vector<Opener>& table()
			{
				static std::vector<Opener> t;
				static bool once = false;
				if (once)
					return t;
				once = true;

				auto add = [&](const char* id, const char* title, const char* open_key, std::function<void()> fn) {
					t.push_back({ Info{ id, title, open_key, true }, std::move(fn) });
				};

				add("questlog", u8"任务日志", "QuestLog", [] {
					UI::get().emplace<UIQuestLog>(Stage::get().get_player().get_quests());
				});
				add("event", u8"活动", "Event", [] { open_event_window(); });
				add("eventwindow", u8"活动窗", "EventWindow", [] { UI::get().emplace<UIEventWindow>(); });
				add("dailygift", u8"每日礼物", "DailyGift", [] { UI::get().emplace<UIDailyGift>(); });
				add("damagerank", u8"伤害统计", "DamageRank", [] { UI::get().emplace<UIDamageRank>(); });
				add("battlestats", u8"战斗统计", "BattleStats", [] { UI::get().emplace<UIBattleStats>(); });
				add("damageskin", u8"伤害皮肤", "DamageSkin", [] { UI::get().emplace<UIDamageSkinPicker>(); });
				add("beautysalon", u8"美容院", "BeautySalon", [] { UI::get().emplace<UIBeautySalon>(); });
				add("fusionanvil", u8"融合外观", "FusionAnvil", [] { UI::get().emplace<UIAnvilDialog>(); });
				add("dailycheckin", u8"每日签到", "DailyCheckin", [] { UI::get().emplace<UIDailyCheckin>(); });
				add("colorprism", u8"装备染色", "ColorPrism", [] { UI::get().emplace<UIColorPrism>(); });
				add("witchforest", u8"魔女森林", "WitchForest", [] { UI::get().emplace<UIWitchForest>(); });
				add("breakthroughstone", u8"突破石", "BreakthroughStone", [] { UI::get().emplace<UIBreakthroughStone>(); });
				add("enhance", u8"强化", "Enhance", [] { UI::get().emplace<UIEnhance>(); });
				add("alliance", u8"联盟", "Alliance", [] { UI::get().emplace<UIAlliance>(); });
				add("chatemoticon", u8"聊天表情", "ChatEmoticon", [] { UI::get().emplace<UIChatEmoticon>(); });
				add("mysticbadge", u8"神秘徽章", "MysticBadge", [] {
					// 神秘徽章原生跟装备窗左侧停靠（sync_side_panels）；单独 emplace 会落在 (0,0)。
					auto& inv = Stage::get().get_player().get_inventory();
					auto eq = UI::get().get_element<UIEquipInventory>();
					if (!eq)
						eq = UI::get().emplace<UIEquipInventory>(inv);
					if (eq)
						eq->makeactive(); // makeactive → sync_side_panels → emplace/anchor mystic
				});
				add("medal", u8"勋章", "Medal", [] { UI::get().emplace<UIMedal>(); });
				add("family", u8"家族", "Family", [] { UI::get().emplace<UIFamily>(); });
				add("maker", u8"专业技术", "Maker", [] { UI::get().emplace<UIMaker>(); });
				add("worldmap", u8"世界地图", "WorldMap", [] { UI::get().emplace<UIWorldMap>(); });
				// UIMapInfoPanel 挂在 UIWorldMap 内，不是独立 UIElement（无 TYPE），
				// 不能走 UI::emplace；开世界地图即可看到地图信息侧栏。
				add("mapinfo", u8"地图信息", "MapInfo", [] { UI::get().emplace<UIWorldMap>(); });
				add("monsterbook", u8"怪物卡片", "MonsterBook", [] { UI::get().emplace<UIMonsterBook>(); });
				add("windowcashshop", u8"现金商店窗", "WindowCashShop", [] { UI::get().emplace<UIWindowCashShop>(); });
				add("optionmenu", u8"选项菜单", "OptionMenu", [] { UI::get().emplace<UIOptionMenu>(); });
				add("sysopt", u8"系统设置", "SysOpt", [] { UI::get().emplace<UISysOpt>(); });
				add("gameopt", u8"游戏设置", "GameOpt", [] { UI::get().emplace<UIGameOpt>(); });
				// 192 拍卖行：原生由 StatusBar 拍卖按钮打开；此处仅注册 builtin id，
				// 按需求不挂侧栏图标（StatusBar 原生入口由后续 lead 接）。
				add("auction", u8"拍卖行", "Auction", [] { UI::get().emplace<UIAuction>(); });
				// 197 TMS280 精灵收藏：此处仅注册 builtin id（@familiar / open_ui），
				// 按需求不挂侧栏图标（StatusBar 入口由 lead 后续接）。
				add("familiar", u8"精灵收藏", "Familiar", [] { UI::get().emplace<UIFamiliar>(); });

				return t;
			}

			const Opener* find_opener(const std::string& key)
			{
				const std::string k = lower_copy(key);
				for (const auto& o : table())
				{
					if (lower_copy(o.info.id) == k
						|| lower_copy(o.info.open_key) == k)
						return &o;
				}
				// aliases kept from legacy open_ui
				if (k == "quest") return find_opener("questlog");
				if (k == "daily") return find_opener("dailygift");
				if (k == "dpt" || k == "damage") return find_opener("damagerank");
				if (k == "battle" || k == "battlestat") return find_opener("battlestats");
				if (k == "checkin" || k == "sign") return find_opener("dailycheckin");
				if (k == "dye" || k == "prism") return find_opener("colorprism");
				if (k == "forest" || k == "witch") return find_opener("witchforest");
				if (k == "breakthrough" || k == "stone") return find_opener("breakthroughstone");
				if (k == "mobbook") return find_opener("monsterbook");
				if (k == "option") return find_opener("optionmenu");
				if (k == "system") return find_opener("sysopt");
				if (k == "game") return find_opener("gameopt");
				return nullptr;
			}
		}

		const std::vector<Info>& builtins()
		{
			static std::vector<Info> infos;
			if (infos.empty())
			{
				for (const auto& o : table())
					infos.push_back(o.info);
			}
			return infos;
		}

		bool open(const std::string& id_or_key)
		{
			const Opener* o = find_opener(id_or_key);
			if (!o)
			{
				LOG(LOG_WARN, "FeatureRegistry: unknown id=" << id_or_key);
				UI::get().emplace<UIOk>(
					"OpenUI 未接线: " + id_or_key,
					[](bool) {});
				return false;
			}

			const std::string id = lower_copy(o->info.id);
			auto it = remote_enable.find(id);
			if (it != remote_enable.end() && !it->second)
			{
				UI::get().emplace<UIOk>(
					o->info.title + u8" 已在后台禁用",
					[](bool) {});
				return false;
			}

			o->open();
			return true;
		}

		void apply_remote_enable(const std::vector<Info>& remote)
		{
			remote_enable.clear();
			for (const auto& r : remote)
				remote_enable[lower_copy(r.id)] = r.enabled;
		}

		std::vector<Info> list_bindable()
		{
			std::vector<Info> out;
			for (const auto& o : table())
			{
				Info i = o.info;
				auto it = remote_enable.find(lower_copy(i.id));
				if (it != remote_enable.end())
					i.enabled = it->second;
				if (i.enabled)
					out.push_back(i);
			}
			return out;
		}
	}
}

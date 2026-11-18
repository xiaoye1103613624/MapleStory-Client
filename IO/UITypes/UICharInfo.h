//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"

#include "../../Character/Char.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ms
{
	// 服务端 CHAR_INFO 包的完整明细（PacketCreator.charInfo 权威）。
	// 由 CharInfoHandler 一次解析后交给 UICharInfo 渲染所有子窗，避免客户端各处重复 skip。
	struct CharInfoData
	{
		struct Pet
		{
			int32_t itemid = 0;
			std::string name;
			uint8_t level = 0;
			uint16_t tameness = 0;   // 亲密度
			uint8_t fullness = 0;    // 饱清感
			int32_t equipItemid = 0; // 宠物装备
		};

		struct Mount
		{
			int32_t id = 0;
			int32_t level = 0;
			int32_t exp = 0;
			int32_t tiredness = 0;  // 疲劳度
		};

		struct Book
		{
			int32_t level = 0;
			int32_t normal = 0;
			int32_t special = 0;
			int32_t total = 0;
			int32_t cover = 0;
		};

		std::vector<Pet> pets;
		Mount mount;
		std::vector<int32_t> wishlist;
		Book book;
		int32_t medalId = 0;
		std::vector<int16_t> medalQuests;
		bool married = false;

		// 2026-11-18：他人穿戴装备（服务端 charInfo 尾部 addCharEquips 编码：(byte bodypart, int itemid)* + 0xFF）。
		// 用于「道具」子窗图标 + 悬浮 tooltip；bodypart 为负 slot 取反后的绝对值（1=HAT … 见 EquipSlot）。
		struct Equip
		{
			uint8_t bodypart = 0;
			int32_t itemid = 0;
		};
		std::vector<Equip> equips;
	};

	class UICharInfo : public UIDragElement<PosCHARINFO>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CHARINFO;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UICharInfo(int32_t cid);

		void draw(float inter) const override;
		void update() override;

		Button::State button_pressed(uint16_t buttonid) override;

		bool is_in_range(Point<int16_t> cursorpos) const override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void update_stats(int32_t character_id, int16_t job_id, uint16_t level, int16_t fame, std::string guild, std::string alliance);

		int32_t get_target_cid() const { return target_cid; }
		std::string get_target_name() const;
		void update_extras(int pet_count, int wish_count, int32_t book_cards, int32_t medal_id, bool married = false);
		// 完整明细填充（宠物/坐骑/心愿/图鉴/勋章），由 CharInfoHandler 调用。
		void update_full(const CharInfoData& data);

	private:
		void show_bottom_window(uint16_t buttonid);
		void show_right_window(uint16_t buttonid);
		// 子窗状态（互斥 + toggle）：默认 / 宠物 / 坐骑 / 心愿 / 道具 / 勋章图鉴 / 排除道具
		enum class SubPanel : uint8_t { NONE, PET, MOUNT, WISH, ITEM, MEDAL, EXCLUDE };
		void show_subpanel(SubPanel panel);
		SubPanel active_panel = SubPanel::NONE;

		void draw_subpanel(float inter) const;
		void draw_pet_panel(float inter) const;
		void draw_mount_panel(float inter) const;
		void draw_wish_panel(float inter) const;
		void draw_item_panel(float inter) const;
		void draw_medal_panel(float inter) const;
		void draw_fame_arrows() const;

		enum Buttons : uint16_t
		{
			BtClose,
			BtCollect,
			BtDamage,
			BtFamily,
			BtItem,
			BtParty,
			BtPersonality,
			BtPet,
			BtPopDown,
			BtPopUp,
			BtRide,
			BtTrad,
			BtFriend,
			BtVisit,
			BtArrayGet,
			BtArrayName,
			BtFAQ,
			BtRegist,
			BtBook,
			// 正确端个人情报框（UserInfo.img 扁平 v83）顶部/底部按钮：
			// 追加在末尾以保留既有枚举值（send_key / draw 分段依赖旧值）。
			BtWish,        // 想买的 ▶ (UserInfo/BtWish 60x19)，顶部右行下
			BtException    // 排除道具 ▶ (UserInfo/BtExceptionShow 71x17)，底部最右
		};

		/// Main Window
		Text name;
		Text job;
		Text level;
		Text fame;
		Text guild;
		Text alliance;

		Char* target_character;
		int32_t target_cid;

		/// Sub Windows
		Point<int16_t> bottom_window_adj;
		Point<int16_t> right_window_adj;

		/// Farm
		Text farm_name;
		Sprite loading;
		bool is_loading;
		uint16_t timestep;
		Charset farm_level;
		Point<int16_t> farm_dim;
		Point<int16_t> farm_adj;
		std::string farm_level_text;

		/// Personality
		bool personality_enabled;
		std::vector<Sprite> personality_sprites;
		BoolPair<std::vector<Sprite>> personality_sprites_enabled;
		Point<int16_t> personality_dimensions;

		/// Collect
		bool collect_enabled;
		std::vector<Sprite> collect_sprites;
		Point<int16_t> collect_dimensions;
		Point<int16_t> collect_adj;
		Sprite default_medal;
		Text medal_text;
		Text medal_total;

		/// Damage
		bool damage_enabled;
		std::vector<Sprite> damage_sprites;
		Point<int16_t> damage_dimensions;

		/// Item
		bool item_enabled;
		std::vector<Sprite> item_sprites;
		Point<int16_t> item_dimensions;

		/// Full-fill sub-panels (运行时 UserInfo.img 的独立背景板)
		Sprite panel_bg_pet;      // backgrnd2 275x399
		Sprite panel_bg_mount;    // backgrnd5 275x361
		Sprite panel_bg_wish;     // backgrnd3 240x162
		Sprite panel_bg_item;     // backgrnd4 240x162
		Sprite panel_bg_medal;    // backgrnd7 275x404
		Sprite panel_bg_exclude;  // backgrnd6 159x147
		Point<int16_t> panel_offset; // 子窗相对主窗的偏移（右侧浮层 = (width,0)，整窗板 = (0,0)）

		CharInfoData full_data;
		bool has_full_data = false;

		// 子窗数据文本（构造期建 Text，update_full 时 change_text）
		std::vector<Text> pet_name_texts;
		std::vector<Text> pet_level_texts;
		Text mount_info_text;
		Text mount_exinfo_text;
		std::vector<Text> wish_item_texts;
		std::vector<Text> item_slot_texts;
		Text book_info_text;
		Text medal_info_text;
		std::vector<Text> medal_quest_texts;

		// 2026-11-18：道具子窗（他人装备）——图标 + 名称 + 悬浮 tooltip 命中矩形。
		struct ItemSlotView
		{
			int32_t itemid = 0;
			Texture icon;
			Text name;
			Rectangle<int16_t> hit;   // 相对窗口左上角
		};
		std::vector<ItemSlotView> item_views;      // 道具子窗 3 槽（与 backgrnd4 空槽对应）
		std::vector<ItemSlotView> wish_views;      // 想买的 3 槽（与 backgrnd3 空槽对应）
		int16_t hovered_slot = -1;                 // 当前悬浮槽（道具=0..n，想买的=100+n）

	public:
		// 宠物排除道具列表：PET_EXCEPTION_LIST 由 PetExceptionListHandler 按 cid 填入（看他人时服务端下发）。
		// 静态暂存使 handler 无需持有 UI 实例即可写入；UICharInfo 绘制子窗时按 target_cid 读取。
		static void store_excluded_items(int32_t cid, const std::vector<int32_t>& itemids);
		static std::vector<int32_t> excluded_items(int32_t cid);

	private:
		static std::unordered_map<int32_t, std::vector<int32_t>>& excluded_store();
		void draw_exclude_panel(float inter) const;
	};
}

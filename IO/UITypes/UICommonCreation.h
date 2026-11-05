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

#include "../UIElement.h"

#include "../Components/Charset.h"
#include "../Components/Textfield.h"

#include "../../Template/BoolPair.h"

#include "../../Character/Look/CharLook.h"

namespace ms
{
	class UICommonCreation : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CLASSCREATION;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UICommonCreation(const std::string& classType);
		~UICommonCreation() override;

		void draw(float inter) const override;
		void update() override;

		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void send_naming_result(bool nameused);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

		std::vector<Sprite> sprites_background;
		std::vector<Sprite> sprites_lookboard;

	private:
		void init_modern(nl::node Login, nl::node Common);
		void init_classic(nl::node Login, nl::node Common);
		void randomize_look();
		void load_char_look();
		void load_classic_char_look();
		void load_classic_gender_pool(bool female, nl::node gender_node);
		void enable_char_look();
		void disable_char_look();
		void set_classic_yes_no(uint8_t step);
		void check_names();

		const std::string& get_equipname(EquipSlot::Id slot) const;

		Point<int16_t> getGenderTopPos();
		Point<int16_t> getBoardMidPos();
		Point<int16_t> getBoardBottomPos();
		Point<int16_t> getAvatarSelPos(size_t index);
		Point<int16_t> getGenderSelectMalePos();
		Point<int16_t> getGenderSelectFemalePos();
		int16_t getCharLookY();
		Point<int16_t> getCharLookLeftPos();
		Point<int16_t> getCharLookRightPos();
		Point<int16_t> getBtYesPos(uint8_t step);
		Point<int16_t> getBtNoPos(uint8_t step);
		Point<int16_t> getHairSelectPos(size_t index);
		Color::Name getNewCharNameColor();
		Rectangle<int16_t> getNewCharNameRect();
		size_t getVerticalCount() const;
		int16_t getVerticalAdj() const;
		Point<int16_t> getNamePos(size_t index) const;
		Point<int16_t> getNameboardPos() const;
		Point<int16_t> getTextfieldPos() const;
		int16_t getMaxWidth() const;

		enum Buttons : uint16_t
		{
			BtStart,
			BtPreview,
			BtYes,
			BtNo,
			BtSkinLeft,
			BtSkinRight,
			BtFaceLeft,
			BtFaceRight,
			BtHairLeft,
			BtHairRight,
			BtTopLeft,
			BtTopRight,
			BtBottomLeft,
			BtBottomRight,
			BtCapeLeft,
			BtCapeRight,
			BtShoesLeft,
			BtShoesRight,
			BtWeaponLeft,
			BtWeaponRight,
			BtGenderMale,
			BtGenderFemale,
			BtDice,
			BtHairSelect0,
			BtHairSelect1,
			BtHairSelect2,
			BtHairSelect3,
			BtHairSelect4,
			BtHairSelect5,
			BtHairSelect6,
			BtHairSelect7,
			BtViewAll,
			LENGTH
		};

		std::map<std::string, std::string> class_map =
		{
			{ "000",	"adventurer"	},
			{ "1000",	"cygnusKnight"	},
			{ "2000",	"aran"			}
		};

		Text version;
		Point<int16_t> version_pos;

		// 原生 classic 建角页左侧栏（创建角色步骤框 / 选择的:世界信息框 / 世界名·频道文本）。
		Sprite step_create;
		Sprite selworld_box;
		Text world_text;
		Text channel_text;
		Texture nameboard;
		std::vector<Sprite> sprites_char_look;
		std::vector<Sprite> sprites_gender_select;

		Point<int16_t> avatarOrigin;
		Point<int16_t> charNameOrigin;
		Point<int16_t> genderOrigin;
		Point<int16_t> newAvatarPos;
		int16_t vCanvasCount;
		int16_t genderTopHeight;
		int16_t boardMidHeight;

		nl::node BtYesPos;
		nl::node BtNoPos;

		size_t hairSelectSize;
		Randomizer randomizer;

		bool classic_mode;
		bool female;
		bool genderSelected;
		bool charLookSelected;
		bool charNameSelected;
		bool newCharNameDisabled;
		bool choosableGender;
		std::string classType;
		std::string className;

		Point<int16_t> classic_yes_gender;
		Point<int16_t> classic_no_gender;
		Point<int16_t> classic_yes_avatar;
		Point<int16_t> classic_no_avatar;
		Point<int16_t> classic_yes_name;
		Point<int16_t> classic_no_name;
		Texture classic_gender_male;
		Texture classic_gender_female;
		Sprite classic_dice;

		Charset createLimitNum;
		Point<int16_t> createLimitPos;

		CharLook newCharLook;
		Point<int16_t> newCharNamePos;
		Textfield newCharName;

		size_t faceIndex;
		Text faceName;
		BoolPair<std::vector<int64_t>> faces;

		size_t hairStyleIndex;
		Text hairStyleName;
		BoolPair<std::vector<int64_t>> hairStyles;

		bool hairColorEnabled;
		size_t hairColorIndex;
		BoolPair<std::unordered_map<int64_t, std::vector<int64_t>>> hairColors;

		size_t skinColorIndex;
		Text skinColorName;
		std::vector<int64_t> skinColors;

		size_t topIndex;
		Text topName;
		BoolPair<std::vector<int64_t>> tops;

		bool capeEnabled;
		size_t capeIndex;
		Text capeName;
		BoolPair<std::vector<int64_t>> capes;

		bool bottomEnabled;
		size_t bottomIndex;
		Text bottomName;
		BoolPair<std::vector<int64_t>> bottoms;

		size_t shoeIndex;
		Text shoeName;
		BoolPair<std::vector<int64_t>> shoes;

		size_t weaponIndex;
		Text weaponName;
		BoolPair<std::vector<int64_t>> weapons;
	};
}
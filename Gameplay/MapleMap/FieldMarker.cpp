//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — FieldMarker
//////////////////////////////////////////////////////////////////////////////////
#include "FieldMarker.h"

#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	FieldMarker::FieldMarker(int32_t o, Kind k, Point<int16_t> pos, const std::string& text, int32_t skill_id, int32_t life_ms)
		: MapObject(o, pos),
		kind(k),
		label(Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::YELLOW, Text::Background::NAMETAG, text)),
		base_label(text),
		lifetime_ms(life_ms)
	{
		if (skill_id > 0)
			try_load_anim(skill_id);
		if (!has_anim)
		{
			if (k == Kind::DOOR)
				try_load_door();
			else if (k == Kind::MIST)
				try_load_mist();
			else if (k == Kind::DRAGON)
				try_load_dragon();
			else if (k == Kind::KITE)
				try_load_kite();
		}
	}

	void FieldMarker::try_load_door()
	{
#ifdef USE_NX
		// Prefer mystic-door style effect if present; else BasicEff ItemUp as visible marker.
		nl::node node = nl::nx::Skill["0100.img"]["skill"]["1001005"]["effect"]["0"];
		if (!node)
			node = nl::nx::Effect["BasicEff.img"]["ItemUp"]["0"];
		if (node)
		{
			anim = Animation(node);
			has_anim = true;
		}
#endif
	}

	void FieldMarker::try_load_mist()
	{
#ifdef USE_NX
		nl::node node = nl::nx::Effect["BasicEff.img"]["ItemUp"]["0"];
		if (!node)
			node = nl::nx::Effect["ItemEff.img"]["1102039"]["effect"]["0"];
		if (node)
		{
			anim = Animation(node);
			has_anim = true;
		}
#endif
	}

	void FieldMarker::try_load_dragon()
	{
#ifdef USE_NX
		// Evan dragon placeholder — skill effect or BasicEff
		nl::node node = nl::nx::Skill["2200.img"]["skill"]["22000000"]["effect"]["0"];
		if (!node)
			node = nl::nx::Effect["BasicEff.img"]["LevelUp"]["0"];
		if (node)
		{
			anim = Animation(node);
			has_anim = true;
		}
#endif
	}

	void FieldMarker::try_load_kite()
	{
#ifdef USE_NX
		nl::node node = nl::nx::Effect["BasicEff.img"]["ItemUp"]["0"];
		if (node)
		{
			anim = Animation(node);
			has_anim = true;
		}
#endif
	}

	void FieldMarker::try_load_anim(int32_t skill_id)
	{
#ifdef USE_NX
		// Item / cash effect ids are typically 1xxxxxx–5xxxxxx (not skill book)
		if (skill_id >= 1000000 && skill_id < 10000000)
		{
			std::string iid = std::to_string(skill_id);
			nl::node node = nl::nx::Effect["ItemEff.img"][iid]["effect"]["0"];
			if (!node)
				node = nl::nx::Effect["ItemEff.img"][iid]["0"];
			if (!node)
				node = nl::nx::Effect["CashEff.img"][iid]["effect"]["0"];
			if (!node)
				node = nl::nx::Effect["CashEff.img"][iid]["0"];
			if (node)
			{
				anim = Animation(node);
				has_anim = true;
				return;
			}
		}

		std::string strid = skill_id < 10000000
			? string_format::extend_id(skill_id, 7)
			: std::to_string(skill_id);
		nl::node src = nl::nx::Skill[string_format::skill_namespace(skill_id) + ".img"]["skill"][strid];
		nl::node node = src["summon"]["0"];
		if (!node)
			node = src["summoned"]["0"];
		if (!node)
			node = src["effect"]["0"];
		if (!node)
			node = src["special"]["0"];
		if (!node)
			node = src["mob"]["0"];
		if (!node)
			node = src["ball"]["0"];
		if (node)
		{
			anim = Animation(node);
			has_anim = true;
		}
#else
		(void)skill_id;
#endif
	}

	void FieldMarker::draw(double viewx, double viewy, float alpha) const
	{
		if (!active)
			return;

		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);
		if (has_anim)
			anim.draw(absp, alpha);
		label.draw(absp + Point<int16_t>(0, has_anim ? -40 : -20));
	}

	int8_t FieldMarker::update(const Physics&)
	{
		if (has_anim)
			anim.update();
		if (lifetime_ms > 0)
		{
			lifetime_ms -= 16;
			if (lifetime_ms <= 0)
				return -1;
		}
		return DRAW_LAYER;
	}

	int8_t FieldMarker::get_layer() const
	{
		return DRAW_LAYER;
	}

	void FieldMarker::set_label(const std::string& text)
	{
		base_label = text;
		if (hp_cur >= 0)
			label.change_text(base_label + " HP" + std::to_string(hp_cur));
		else
			label.change_text(text);
	}

	void FieldMarker::apply_damage(int32_t damage)
	{
		if (damage <= 0)
			return;
		if (hp_cur < 0)
			hp_cur = 100000; // unknown pool until first hit
		hp_cur -= damage;
		if (hp_cur < 0)
			hp_cur = 0;
		label.change_text(base_label + " HP" + std::to_string(hp_cur));
	}

	FieldMarker::Kind FieldMarker::get_kind() const
	{
		return kind;
	}
}

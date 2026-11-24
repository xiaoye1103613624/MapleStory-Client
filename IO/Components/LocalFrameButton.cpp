//////////////////////////////////////////////////////////////////////////////////
//	LocalFrameButton — Default/Notify/Appear local PNG sequences.
//////////////////////////////////////////////////////////////////////////////////
#include "LocalFrameButton.h"

#include "../../Constants.h"
#include "../../Util/Misc.h"
#include "../../Util/Resource/ResourceLocator.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>

namespace ms
{
	namespace
	{
		std::string lower_copy(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

		bool load_state_frames(const std::string& icon_key, const char* state,
			size_t atlas_base, size_t atlas_offset,
			std::vector<LocalPngTexture>& out)
		{
			namespace fs = std::filesystem;
			out.clear();

			const std::string exe_dir = res::ResourceLocator::executable_dir();
			std::vector<std::string> roots = {
				"UI/SideIcons/frames/" + icon_key + "/",
				"UI\\SideIcons\\frames\\" + icon_key + "\\",
			};
			if (!exe_dir.empty())
			{
				roots.push_back(exe_dir + "\\UI\\SideIcons\\frames\\" + icon_key + "\\");
			}
			// Dev fallbacks: synced ICON pack folders by common keys.
			roots.push_back(
				"docs/features/side-menu-unified-entry-2026-11/resources/frames/" + icon_key + "/");

			auto try_dir = [&](const std::string& dir) -> bool {
				std::error_code ec;
				if (!fs::is_directory(fs::u8path(dir), ec))
					return false;
				std::vector<std::pair<int, std::string>> files;
				for (auto& ent : fs::directory_iterator(fs::u8path(dir), ec))
				{
					if (!ent.is_regular_file())
						continue;
					const std::string name = ent.path().filename().string();
					// Accept: Default.0.png | GalaxyStar.Default.0.png
					std::string stem = name;
					const auto dotpng = lower_copy(stem);
					if (dotpng.size() < 5 || dotpng.substr(dotpng.size() - 4) != ".png")
						continue;
					stem = stem.substr(0, stem.size() - 4);
					int idx = -1;
					const std::string prefix1 = std::string(state) + ".";
					const std::string prefix2 = icon_key + "." + state + ".";
					if (stem.compare(0, prefix2.size(), prefix2) == 0)
					{
						try { idx = std::stoi(stem.substr(prefix2.size())); }
						catch (...) { continue; }
					}
					else if (stem.compare(0, prefix1.size(), prefix1) == 0)
					{
						try { idx = std::stoi(stem.substr(prefix1.size())); }
						catch (...) { continue; }
					}
					else
						continue;
					files.emplace_back(idx, ent.path().string());
				}
				if (files.empty())
					return false;
				std::sort(files.begin(), files.end(),
					[](const auto& a, const auto& b) { return a.first < b.first; });
				out.reserve(files.size());
				for (size_t i = 0; i < files.size(); ++i)
				{
					LocalPngTexture tex;
					const size_t atlas = atlas_base + atlas_offset + i;
					if (!tex.load(atlas, { files[i].second }))
						continue;
					out.push_back(std::move(tex));
				}
				return !out.empty();
			};

			for (const auto& r : roots)
			{
				if (try_dir(r))
					return true;
			}

			// Flat legacy SideIcons/{book,bulb,star}.png as single-frame Default.
			// 新 side_menu 树的 iconKey 多数是 WZ 风格 key，本地无序列帧；
			// 映射到 live UI/SideIcons/ 下已有的 PNG，保证横排成员有可见且可区分的图。
			const std::string legacy_map[][2] = {
				{ "galaxystar", "star" },
				{ "eventlist", "book" },
				{ "daily", "bulb" },
				{ "attendance", "checkin" },      // 每日签到
				{ "dailygift", "star" },          // 每日礼物
				{ "achieve", "colorprism" },      // 勋章
				{ "content", "book" },            // 通用
				{ "collection", "colorprism" },   // 收集
				{ "maplehelper", "bulb" },
				{ "19thevent", "damagerank" },
				{ "13thballoon", "colorprism" },
				{ "maplestyle", "colorprism" },
				{ "maplealarm", "bulb" },
				{ "itemcollection", "book" },
				{ "absolute", "damagerank" },      // 魔女森林，与 GalaxyStar(star) 区分
				{ "xmas", "damagerank" },
				{ "anvil", "colorprism" },        // 融合外观占位
			};
			const std::string lk = lower_copy(icon_key);
			std::string stem = icon_key;
			for (const auto& m : legacy_map)
			{
				if (lk == m[0])
				{
					stem = m[1];
					break;
				}
			}
			if (std::string(state) == "Default" || std::string(state) == "Notify")
			{
				const std::string file = (std::string(state) == "Notify")
					? (stem + "_over.png")
					: (stem + ".png");
				LocalPngTexture tex;
				std::vector<std::string> paths = {
					"UI/SideIcons/" + file,
					"UI\\SideIcons\\" + file,
				};
				if (!exe_dir.empty())
					paths.push_back(exe_dir + "\\UI\\SideIcons\\" + file);
				if (tex.load(atlas_base + atlas_offset, paths))
				{
					out.push_back(std::move(tex));
					return true;
				}
			}
			return false;
		}
	}

	LocalFrameButton::LocalFrameButton(std::vector<LocalPngTexture> defaults_in,
		std::vector<LocalPngTexture> notifys_in,
		std::vector<LocalPngTexture> appears_in,
		Point<int16_t> position)
		: defaults(std::move(defaults_in))
		, notifys(std::move(notifys_in))
		, appears(std::move(appears_in))
		, pos(position)
	{
		state = Button::State::NORMAL;
		active = true;
		appear_done = appears.empty();
	}

	bool LocalFrameButton::try_load(const std::string& icon_key, size_t atlas_base,
		std::vector<LocalPngTexture>& out_default,
		std::vector<LocalPngTexture>& out_notify,
		std::vector<LocalPngTexture>& out_appear)
	{
		if (icon_key.empty())
			return false;
		const bool ok_d = load_state_frames(icon_key, "Default", atlas_base, 0, out_default);
		load_state_frames(icon_key, "Notify", atlas_base, 24, out_notify);
		load_state_frames(icon_key, "Appear", atlas_base, 48, out_appear);
		if (!ok_d)
			return false;
		if (out_notify.empty())
			out_notify = out_default;
		return true;
	}

	const LocalPngTexture* LocalFrameButton::current_frame() const
	{
		const std::vector<LocalPngTexture>* seq = &defaults;
		if (!appear_done && !appears.empty())
			seq = &appears;
		else if ((state == Button::State::MOUSEOVER || state == Button::State::PRESSED)
			&& !notifys.empty())
			seq = &notifys;
		if (seq->empty())
			return nullptr;
		const size_t i = static_cast<size_t>(frame) % seq->size();
		return &(*seq)[i];
	}

	void LocalFrameButton::draw(Point<int16_t> parentpos) const
	{
		if (!active)
			return;
		if (const LocalPngTexture* f = current_frame())
			f->draw(parentpos + pos);
	}

	void LocalFrameButton::update()
	{
		anim_ms += Constants::TIMESTEP;
		if (anim_ms < FRAME_MS)
			return;
		anim_ms = 0;
		++frame;

		if (!appear_done && !appears.empty())
		{
			if (frame >= static_cast<int>(appears.size()))
			{
				appear_done = true;
				frame = 0;
			}
		}
		else
		{
			const auto& seq = (state == Button::State::MOUSEOVER || state == Button::State::PRESSED)
				? notifys : defaults;
			if (!seq.empty())
				frame %= static_cast<int>(seq.size());
		}
	}

	void LocalFrameButton::set_position(Point<int16_t> position)
	{
		pos = position;
	}

	Rectangle<int16_t> LocalFrameButton::bounds(Point<int16_t> parentpos) const
	{
		const LocalPngTexture* f = current_frame();
		const int16_t w = f ? f->width() : 32;
		const int16_t h = f ? f->height() : 32;
		Point<int16_t> lt = parentpos + pos;
		return Rectangle<int16_t>(lt, lt + Point<int16_t>(w, h));
	}

	int16_t LocalFrameButton::width() const
	{
		if (const LocalPngTexture* f = current_frame())
			return f->width();
		return 32;
	}

	Point<int16_t> LocalFrameButton::origin() const
	{
		return Point<int16_t>(0, 0);
	}
}

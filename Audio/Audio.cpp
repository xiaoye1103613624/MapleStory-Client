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
#include "Audio.h"

#include "../Configuration.h"
#include "../IO/Window.h"

#include <fstream>
#include <iostream>

// BASS is only used when the backend is enabled (Windows by default, or when
// USE_BASS=1 is provided on other platforms). Otherwise a silent fallback is
// compiled in so the client still builds and runs without audio.
#ifdef USE_BASS
#include <bass.h>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#endif

#ifdef USE_NX
#include <nlnx/audio.hpp>
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	/// Append one line to _audio.log next to the executable (Release has no console).
	static void audio_log(const std::string& line)
	{
		std::ofstream out("_audio.log", std::ios::app);

		if (out)
			out << line << std::endl;

		std::cerr << line << std::endl;
	}

#ifdef USE_NX
	/// Resolve BGM across Sound*.nx roots (BeiDou/v83 keeps classic tracks in Sound.nx;
	/// newer packs may put Newtro tracks in Sound002 / Sound001 / Sound2).
	static nl::audio resolve_bgm_audio(const std::string& path)
	{
		nl::audio ad = nl::nx::Sound002.resolve(path);

		if (ad.data())
			return ad;

		ad = nl::nx::Sound.resolve(path);

		if (ad.data())
			return ad;

		ad = nl::nx::Sound001.resolve(path);

		if (ad.data())
			return ad;

		return nl::nx::Sound2.resolve(path);
	}
#endif

#ifdef USE_BASS
	/// WZ/NX audio blobs start with an 82-byte UOL/GUID header; BASS needs the
	/// payload after that. length must be bytes available after the offset.
	static QWORD bass_audio_payload_length(uint32_t total)
	{
		return total > 82 ? static_cast<QWORD>(total - 82) : 0;
	}

	static void* bass_window_handle()
	{
#ifdef _WIN32
		SDL_Window* wnd = Window::get().get_handle();

		if (!wnd)
			return nullptr;

		return SDL_GetPointerProperty(
			SDL_GetWindowProperties(wnd),
			SDL_PROP_WINDOW_WIN32_HWND_POINTER,
			nullptr);
#else
		return nullptr;
#endif
	}

	static HSTREAM create_bgm_stream(const void* data, uint32_t total_length, bool loop)
	{
		const QWORD payload = bass_audio_payload_length(total_length);

		if (!data || payload == 0)
			return 0;

		DWORD flags = loop ? BASS_SAMPLE_LOOP : 0;
		HSTREAM stream = BASS_StreamCreateFile(true, data, 82, payload, flags | BASS_SAMPLE_FLOAT);

		if (!stream)
			stream = BASS_StreamCreateFile(true, data, 82, payload, flags);

		return stream;
	}
#endif

	Sound::Sound(Name name)
	{
		id = soundids[name];
	}

	Sound::Sound(int32_t itemid)
	{
		auto fitemid = format_id(itemid);

		if (itemids.find(fitemid) != itemids.end())
		{
			id = itemids.at(fitemid);
		}
		else
		{
			auto pid = (10000 * (itemid / 10000));
			auto fpid = format_id(pid);

			if (itemids.find(fpid) != itemids.end())
				id = itemids.at(fpid);
			else
				id = itemids.at("02000000");
		}
	}

	Sound::Sound(nl::node src)
	{
		id = add_sound(src);
	}

	Sound::Sound()
	{
		id = 0;
	}

	void Sound::play() const
	{
		if (id > 0)
			play(id);
	}

	Error Sound::init()
	{
#ifdef USE_BASS
		void* hwnd = bass_window_handle();

		if (!BASS_Init(-1, 44100, 0, static_cast<HWND>(hwnd), nullptr))
		{
			audio_log(std::string("[Sound][ERROR] BASS_Init failed err=") + std::to_string(BASS_ErrorGetCode()));
			return Error::Code::AUDIO;
		}

		audio_log(std::string("[Sound] BASS_Init ok hwnd=") + (hwnd ? "set" : "null"));
#else
		// No audio backend: preload the sound table (so ids resolve) but play nothing.
#endif

		nl::node uisrc = nl::nx::Sound["UI.img"];

		add_sound(Sound::Name::BUTTONCLICK, uisrc["BtMouseClick"]);
		add_sound(Sound::Name::BUTTONOVER, uisrc["BtMouseOver"]);
		add_sound(Sound::Name::CHARSELECT, uisrc["CharSelect"]);
		add_sound(Sound::Name::DLGNOTICE, uisrc["DlgNotice"]);
		add_sound(Sound::Name::MENUDOWN, uisrc["MenuDown"]);
		add_sound(Sound::Name::MENUUP, uisrc["MenuUp"]);
		add_sound(Sound::Name::RACESELECT, uisrc["RaceSelect"]);
		add_sound(Sound::Name::SCROLLUP, uisrc["ScrollUp"]);
		add_sound(Sound::Name::SELECTMAP, uisrc["SelectMap"]);
		add_sound(Sound::Name::TAB, uisrc["Tab"]);
		add_sound(Sound::Name::WORLDSELECT, uisrc["WorldSelect"]);
		add_sound(Sound::Name::DRAGSTART, uisrc["DragStart"]);
		add_sound(Sound::Name::DRAGEND, uisrc["DragEnd"]);
		add_sound(Sound::Name::WORLDMAPOPEN, uisrc["WorldmapOpen"]);
		add_sound(Sound::Name::WORLDMAPCLOSE, uisrc["WorldmapClose"]);

		nl::node gamesrc = nl::nx::Sound["Game.img"];

		add_sound(Sound::Name::GAMESTART, gamesrc["GameIn"]);
		add_sound(Sound::Name::JUMP, gamesrc["Jump"]);
		add_sound(Sound::Name::DROP, gamesrc["DropItem"]);
		add_sound(Sound::Name::PICKUP, gamesrc["PickUpItem"]);
		add_sound(Sound::Name::PORTAL, gamesrc["Portal"]);
		add_sound(Sound::Name::LEVELUP, gamesrc["LevelUp"]);
		add_sound(Sound::Name::TOMBSTONE, gamesrc["Tombstone"]);

		nl::node itemsrc = nl::nx::Sound["Item.img"];

		for (auto node : itemsrc)
			add_sound(node.name(), node["Use"]);

		uint8_t volume = Setting<SFXVolume>::get().load();

		if (!set_sfxvolume(volume))
			return Error::Code::AUDIO;

		return Error::Code::NONE;
	}

	void Sound::close()
	{
#ifdef USE_BASS
		BASS_Free();
#endif
	}

	bool Sound::set_sfxvolume(uint8_t vol)
	{
#ifdef USE_BASS
		return BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, vol * 100) == TRUE;
#else
		(void)vol;
		return true;
#endif
	}

	void Sound::play(size_t id)
	{
		if (!samples.count(id))
			return;

#ifdef USE_BASS
		HCHANNEL channel = BASS_SampleGetChannel((HSAMPLE)samples.at(id), false);

		if (channel)
			BASS_ChannelPlay(channel, true);
#else
		(void)id;
#endif
	}

	size_t Sound::add_sound(nl::node src)
	{
		nl::audio ad = src;

		auto data = reinterpret_cast<const void*>(ad.data());

		if (data)
		{
			size_t id = ad.id();

			if (samples.find(id) != samples.end())
				return id;

#ifdef USE_BASS
			DWORD payload = static_cast<DWORD>(bass_audio_payload_length(ad.length()));

			if (payload == 0)
				return 0;

			HSAMPLE sample = BASS_SampleLoad(true, data, 82, payload, 4, BASS_SAMPLE_OVER_POS);

			if (!sample)
			{
				audio_log(std::string("[Sound][WARN] SampleLoad failed id=") + std::to_string(id)
					+ " len=" + std::to_string(ad.length())
					+ " err=" + std::to_string(BASS_ErrorGetCode()));
				return 0;
			}

			samples[id] = sample;
#else
			samples[id] = id; // placeholder so play() sees the id
#endif

			return id;
		}
		else
		{
			return 0;
		}
	}

	void Sound::add_sound(Name name, nl::node src)
	{
		size_t id = add_sound(src);

		if (id)
			soundids[name] = id;
	}

	void Sound::add_sound(std::string itemid, nl::node src)
	{
		size_t id = add_sound(src);

		if (id)
			itemids[itemid] = id;
	}

	std::string Sound::format_id(int32_t itemid)
	{
		std::string strid = std::to_string(itemid);
		strid.insert(0, 8 - strid.size(), '0');

		return strid;
	}

	std::unordered_map<size_t, uint64_t> Sound::samples;
	EnumMap<Sound::Name, size_t> Sound::soundids;
	std::unordered_map<std::string, size_t> Sound::itemids;

	Music::Music(std::string p)
	{
		path = p;
	}

	void Music::play() const
	{
		static std::string bgmpath = "";

		if (path == bgmpath)
			return;

#ifdef USE_BASS
		static HSTREAM stream = 0;

		nl::audio ad = resolve_bgm_audio(path);
		auto data = reinterpret_cast<const void*>(ad.data());

		if (!data)
		{
			audio_log(std::string("[Music][WARN] BGM not found: ") + path
				+ " (tried Sound002/Sound/Sound001/Sound2)");
			return;
		}

		if (stream)
		{
			BASS_ChannelStop(stream);
			BASS_StreamFree(stream);
			stream = 0;
		}

		stream = create_bgm_stream(data, ad.length(), true);

		if (!stream)
		{
			audio_log(std::string("[Music][ERROR] StreamCreate failed path=") + path
				+ " len=" + std::to_string(ad.length())
				+ " err=" + std::to_string(BASS_ErrorGetCode()));
			return;
		}

		// Re-apply volume in case a FOCUS_LOST muted streams before play, or
		// SDL omitted the matching FOCUS_GAINED after startup.
		uint8_t volume = Setting<BGMVolume>::get().load();
		set_bgmvolume(volume);
		BASS_Start();

		if (!BASS_ChannelPlay(stream, true))
		{
			audio_log(std::string("[Music][ERROR] ChannelPlay failed path=") + path
				+ " err=" + std::to_string(BASS_ErrorGetCode()));
			BASS_StreamFree(stream);
			stream = 0;
			return;
		}

		bgmpath = path;
		audio_log(std::string("[Music] playing ") + path
			+ " len=" + std::to_string(ad.length())
			+ " vol=" + std::to_string(static_cast<int>(volume))
			+ " handle=" + std::to_string(stream));
#else
		bgmpath = path;
#endif
	}

	void Music::play_once() const
	{
		static std::string bgmpath = "";

		if (path == bgmpath)
			return;

#ifdef USE_BASS
		static HSTREAM stream = 0;

		nl::audio ad = resolve_bgm_audio(path);
		auto data = reinterpret_cast<const void*>(ad.data());

		if (!data)
		{
			audio_log(std::string("[Music][WARN] BGM not found: ") + path
				+ " (tried Sound002/Sound/Sound001/Sound2)");
			return;
		}

		if (stream)
		{
			BASS_ChannelStop(stream);
			BASS_StreamFree(stream);
			stream = 0;
		}

		stream = create_bgm_stream(data, ad.length(), false);

		if (!stream)
		{
			audio_log(std::string("[Music][ERROR] StreamCreate(once) failed path=") + path
				+ " err=" + std::to_string(BASS_ErrorGetCode()));
			return;
		}

		uint8_t volume = Setting<BGMVolume>::get().load();
		set_bgmvolume(volume);
		BASS_Start();

		if (!BASS_ChannelPlay(stream, true))
		{
			audio_log(std::string("[Music][ERROR] ChannelPlay(once) failed path=") + path
				+ " err=" + std::to_string(BASS_ErrorGetCode()));
			BASS_StreamFree(stream);
			stream = 0;
			return;
		}

		bgmpath = path;
		audio_log(std::string("[Music] playing once ") + path
			+ " vol=" + std::to_string(static_cast<int>(volume)));
#else
		bgmpath = path;
#endif
	}

	Error Music::init()
	{
		uint8_t volume = Setting<BGMVolume>::get().load();

		if (!set_bgmvolume(volume))
			return Error::Code::AUDIO;

		audio_log(std::string("[Music] init vol=") + std::to_string(static_cast<int>(volume)));
		return Error::Code::NONE;
	}

	bool Music::set_bgmvolume(uint8_t vol)
	{
#ifdef USE_BASS
		return BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol * 100) == TRUE;
#else
		(void)vol;
		return true;
#endif
	}
}

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
#include "Gameplay/Stage.h"
#include "Data/SetItemData.h"
#include "IO/LoginBackground.h"
#include "IO/UI.h"
#include "IO/Window.h"
#include "Net/Session.h"
#include "Util/HardwareInfo.h"
#include "Util/ScreenResolution.h"
#include "Configuration.h"
#include "Constants.h"
#include "MapleStory.h"

#include "Util/BootSplash.h"
#include "Util/Patch/BootTimer.h"
#include "Util/Patch/PatchUpdater.h"
#include "Util/Resource/ResourceProbe.h"
#include "Util/Resource/ResourceService.h"
#include "Graphics/GraphicsGL.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#ifdef __ANDROID__
#include <unistd.h>
#endif

namespace ms
{
	// Command line helper for the headless resource diagnostics below.
	bool has_argument(int argc, char** argv, const char* wanted)
	{
		for (int i = 1; i < argc; ++i)
			if (argv[i] && std::strcmp(argv[i], wanted) == 0)
				return true;

		return false;
	}

	// [DBG-INIT] 启动面包屑：Windows/Android 均落盘 _init_debug.log（cwd）。
	// Release 下 LOG() 为 no-op，堆损坏排查时靠此文件定位最后成功阶段。
	static void dbginit(const std::string& stage)
	{
		std::ofstream f("_init_debug.log", std::ios::app);
		if (!f)
			return;
		const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count();
		f << ms << ' ' << stage << '\n';
		f.flush();
	}

	Error init()
	{
		dbginit("init:start");
		const auto boot_t0 = std::chrono::steady_clock::now();

		// 确保配置先加载，宽高会同步到 Constants，Window 创建时才能取到正确分辨率。
		Configuration::get();
#ifdef __ANDROID__
		{
			// [DBG-INIT] 记录 cwd 与实际解析出的 server.ip，判断配置是否生效。
			char cwdbuf[512] = { 0 };
			if (::getcwd(cwdbuf, sizeof(cwdbuf)))
				dbginit(std::string("cwd=") + cwdbuf);
			dbginit(std::string("config: server.ip=") + Setting<ServerIP>::get().load()
				+ " server.port=" + Setting<ServerPort>::get().load());
		}
#endif
		dbginit("config:ok");

		// Show the window before heavy I/O so startup is not a 20s blank desktop.
		{
			ms::patch::BootTimer _bt("window");
			if (Error error = Window::get().init()) {
				dbginit(std::string("window:FAIL ") + error.get_message());
				return error;
			}
		}
		dbginit("window:ok");
		ms::perf::mark("BOOT", "after_window");
		// Pre-WZ: procedural spinner (1B/2A). Post-WZ: Logo.img/Nexon, then login.
		BootSplash::get().pump("Loading...");
		dbginit("splash1:ok");
		ms::perf::mark("BOOT", "after_splash1");

		// 差异资源同步必须在 ResourceService 之前：按最终 img/wz/nx 文件更新本地。
		{
			ms::patch::BootTimer _bt("patch");
			if (Error error = ms::patch::PatchUpdater::sync()) {
				dbginit(std::string("patch:FAIL ") + error.get_message());
				return error;
			}
		}
		dbginit("patch:ok");
		ms::perf::mark("BOOT", "after_patch");
		BootSplash::get().pump("Loading data...");

		// 数据源（img / wz / nx）在运行时按 data.format 选择，统一经由资源门面装载。
		// img：仅装登录根；其余根在登录成功后 ensure_game_roots()。
		{
			ms::patch::BootTimer _bt("resource");
			if (Error error = ms::res::ResourceService::get().init()) {
				dbginit(std::string("resource:FAIL ") + error.get_message());
				return error;
			}
			LOG(LOG_INFO, "[BOOT] resource diagnostics: "
				<< ms::res::ResourceService::get().diagnostics(true));
		}
		dbginit("resource:ok");
		ms::perf::mark("BOOT", "after_resource");
		// img：登录根就绪后立即在后台预热选角根（Character/Item/… +
		// BodyDrawInfo）。它与片头/登录界面并行，等用户点完连接时通常
		// 已装载完——此前这一步在登录响应后同步执行，会冻结 Wait 弹窗
		// 半分钟级（表现=「连接加载条卡住不动，过一会才动」）。
		ms::res::ResourceService::get().prewarm_game_roots();
		dbginit("prewarm:ok");
		BootSplash::get().try_load_nexon();
		BootSplash::get().pump("Connecting...");
		dbginit("splash_connect:ok");

		if (Error error = Session::get().init()) {
			dbginit(std::string("session:FAIL ") + error.get_message());
			return error;
		}
		dbginit("session:ok");
		ms::perf::mark("BOOT", "after_session");
		BootSplash::get().pump("Connecting...");

		if (Error error = Sound::init()) {
			dbginit(std::string("sound:FAIL ") + error.get_message());
			return error;
		}
		dbginit("sound:ok");
		ms::perf::mark("BOOT", "after_sound");
		BootSplash::get().pump("Preparing...");

		if (Error error = Music::init()) {
			dbginit(std::string("music:FAIL ") + error.get_message());
			return error;
		}
		dbginit("music:ok");
		ms::perf::mark("BOOT", "after_music");
		BootSplash::get().pump("Preparing...");

		Char::init();
		dbginit("char:ok");
		DamageNumber::init();
		dbginit("damagenumber:ok");
		MapPortals::init();
		dbginit("mapportals:ok");
		// ⑤ 套装花名册（2026-11-17 升级）：主源 = live WZ Etc/SetItemInfo.img；
		//    WZ 不可用（lazy 根未装载/缺节点）时降级 Data/SetItem.txt；
		//    两者都失败则按「无套装」运行，不影响正常游戏。
		//    Etc 是 lazy 根，此处经 nlnx 门面访问（与 QuestData 读 Quest/ 同范式）。
		if (!SetItemData::load_from_wz())
			SetItemData::load("");
		dbginit("setitem:ok");
		Stage::get().init();
		dbginit("stage:ok");
		// Seed the locally selected damage skin before any damage number is drawn.
		// The login DAMAGE_SKIN(247) packet remains authoritative when non-zero.
		Stage::get().get_player().set_damage_skin(Configuration::get().get_damage_skin());
		BootSplash::get().pump("Preparing...");

		// 登录背景（地图 + 书框）必须先于 UI 初始化：
		// UI::init() 会创建 UIStateLogin/UILogin，而 UILogin 在构造时会用
		// LoginBackground::available() 判断"背景是否已由 LoginBackground 接管"。
		// 顺序颠倒时 available() 仍为 false，UILogin 会自己再画一张
		// Login.img/Common/frame（围绕屏幕中心定位），与 LoginBackground 在 (0,0)
		// 画的那张叠加 → 出现双书页。
		//
		// 启动动画在 BootSplash 已播完；强制跳过 UILogo，资源完后直接进登录页。
		{
			ms::patch::BootTimer _bt("ui");
			Configuration::get().set_start_shown(true);
			LoginBackground::get().init();
			dbginit("loginbg:ok");
			BootSplash::get().pump("Preparing...");
			// 原生启动动画：资源就绪后、进登录页前播放 Logo.img/Nexon 片头。
			// 放在最后一步是为了"片头 → 登录页"直接衔接，中间不再插回 spinner。
			// 开关 boot.logo（默认开），任意按键/点击可跳过。
			BootSplash::get().play_logo();
			BootSplash::get().clear();
			UI::get().init();
			dbginit("ui:ok");
		}

		// UILogin plays BGM during construction; sync mute state to real focus
		// so a startup FOCUS_LOST without GAINED cannot leave volume at 0 forever.
		Window::get().sync_audio_focus();

		{
			const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - boot_t0).count();
			LOG(LOG_INFO, "[BOOT] init_total=" << ms << "ms");
		}
		dbginit("init:done");
		return Error::NONE;
	}

	void update()
	{
		Window::get().check_events();
		Window::get().update();
		Stage::get().update();
		UI::get().update();
		Session::get().read();
	}

	void draw(float alpha)
	{
		Window::get().begin();
		Stage::get().draw(alpha);
		// alpha 是 accumulator/timestep，即「本帧还差多少时间才走到下一逻辑帧」的插值
		// 系数（每帧在 0..1 之间抖动）。Stage 拿它插值描画位置是对的；UI 侧不接受插值语义，
		// 但此前有 14 个文件里的 59 处（外加基类 UIElement::draw_sprites）把它当不透明度传给
		// sprite.draw(pos, inter)，于是界面层每帧按一个抖动的半透明度描画。这里一次性按不透明绘制。
		UI::get().draw(1.0f);
		Window::get().end();
	}

	bool running()
	{
		return Session::get().is_connected()
			&& UI::get().not_quitted()
			&& Window::get().not_closed();
	}

	void loop()
	{
		Timer::get().start();

		int64_t timestep = Constants::TIMESTEP * 1000;
		int64_t accumulator = timestep;

		int64_t period = 0;
		int32_t samples = 0;

		bool show_fps = Configuration::get().get_show_fps();

		// 长跑内存/性能采样节拍：每 5s 采一次（见循环体内）。
		auto mem_sample_tp = std::chrono::steady_clock::now();

		while (running())
		{
			// xy-config 热重载通道（含 ui.language -> Locale::reload）。此前
			// Configuration::reload_if_changed() 全仓零调用点，「改配置即生效」只写在注释里；
			// 未命中 mtime 变化时它只做一次 stat，所以每帧调用不引入可测开销。
			Configuration::get().reload_if_changed();

			int64_t elapsed = Timer::get().stop();

			// 上限保护：单帧慢（装图/atlas 冷上传/卡顿）时，无界的补帧
			// 循环会滚雪球——elapsed 越大补帧越多、每帧越慢，最终占满
			// CPU、永远走不到 draw()（黑屏）且难以泵事件（未响应）。
			// 限制单帧最多补 MAX_FRAME_UPDATES 次，多余时间直接丢弃。
			constexpr int64_t MAX_FRAME_UPDATES = 4;
			accumulator += elapsed;
			if (accumulator > timestep * MAX_FRAME_UPDATES)
				accumulator = timestep * MAX_FRAME_UPDATES;

			// Update game with constant timestep as many times as possible.
			for (; accumulator >= timestep; accumulator -= timestep)
				update();

			// Draw the game. Interpolate to account for remaining time.
			float alpha = static_cast<float>(accumulator) / timestep;
			draw(alpha);

			if (show_fps)
			{
				if (samples < 100)
				{
					period += elapsed;
					samples++;
				}
				else if (period)
				{
					int64_t fps = (samples * 1000000) / period;

					LOG(LOG_INFO, "FPS: " << fps);

					period = 0;
					samples = 0;
				}
			}

			// 长跑内存治理：每 5s 采样一次 Private Bytes / 图集占用 / 常驻根诊断 /
			// FPS（debug.perf_log 开启时落盘 boot_perf.log），用于验证内存稳定
			// 不溢出、波动收敛；并触发 quads 批缓冲空闲收缩（trim）。
			const auto mem_now = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(
				mem_now - mem_sample_tp).count() >= 5000)
			{
				mem_sample_tp = mem_now;
				ms::res::ResourceService::get().sample_memory();
				GraphicsGL::get().trim();
			}
		}

		dbginit(std::string("loop:exit connected=")
			+ (Session::get().is_connected() ? "1" : "0")
			+ " ui=" + (UI::get().not_quitted() ? "1" : "0")
			+ " win=" + (Window::get().not_closed() ? "1" : "0"));
		ms::perf::mark("BOOT", "loop_exit");
		Sound::close();
		dbginit("sound:closed");
	}

	void start()
	{
		try
		{
			// Initialize and check for errors
			if (Error error = init())
			{
				const char* message = error.get_message();
				const char* args = error.get_args();
				bool can_retry = error.can_retry();

				if (args && args[0])
					LOG(LOG_ERROR, message << args);
				else
					LOG(LOG_ERROR, message);

				dbginit(std::string("start:init FAILED ") + message + (args ? args : ""));

				if (can_retry)
					LOG(LOG_INFO, "Enter 'retry' to try again.");

				// Windows 子系统无控制台：cin 会立刻 EOF，禁止阻塞/空转。
				std::string command;
				if (std::cin.good())
					std::cin >> command;

				if (can_retry && command == "retry")
					start();
			}
			else
			{
				dbginit("loop:enter");
				loop();
				dbginit("loop:done");
			}
		}
		catch (const std::exception& ex)
		{
			LOG(LOG_ERROR, "MapleStory::start uncaught: " << ex.what());
			dbginit(std::string("start:UNCAUGHT ") + ex.what());
		}
		dbginit("start:return");
	}
}

#ifdef _WIN32
#  ifdef _DEBUG
int main(int argc, char** argv)
#  else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#  endif
#elif defined(__ANDROID__)
#include <SDL3/SDL.h>
// SDLActivity's nativeRunMain() locates the entry point with
// dlsym(handle, "SDL_main"), i.e. by the *unmangled C symbol*. Without
// extern "C" the compiler exports it as _Z8SDL_mainiPPc and the app
// aborts right after library load with
// "nativeRunMain(): Couldn't find function SDL_main in library".
extern "C" int SDL_main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif
{
#ifdef _WIN32
	// MSVC and MinGW expose the process command line through __argc / __argv,
	// which is also available from the Windows subsystem entry point.
	const int arg_count = __argc;
	char** arg_values = __argv;
#else
	const int arg_count = argc;
	char** arg_values = argv;
#endif

#ifdef __ANDROID__
	// Run with the app's external-storage directory as the working directory so
	// relative paths (xy-config.yaml, Settings, Data) resolve to the 9 GB payload
	// that the deploy script pushes there. SDL is already initialised when this
	// native entry is called by the SDL Android activity.
	if (const char* ext = SDL_GetAndroidExternalStoragePath())
		::chdir(ext);

	// The Configuration singleton may already have been constructed during
	// static initialisation, i.e. while the working directory was still "/",
	// so it parsed nothing and every setting kept its built-in default
	// (notably server.ip=127.0.0.1, which on the emulator is the device
	// itself). Reload now that the working directory points at the payload.
	ms::Configuration::get().load();
#endif

	// Headless resource diagnostics.
	//
	// The game path starts with Session::get().init(), which needs a handshake
	// on the login port; without a server the process exits before any data has
	// been loaded. These switches load and inspect the data layer only, so
	// layout problems stay reproducible in environments without a login server.
	// Results go to Util/Resource/ResourceProbe.h's report file because this is
	// a Windows subsystem application with no console.
	if (ms::has_argument(arg_count, arg_values, "--resource-selftest"))
		return ms::ResourceProbe::run_selftest();

	if (ms::has_argument(arg_count, arg_values, "--resource-probe"))
		return ms::ResourceProbe::run_probe(ms::has_argument(arg_count, arg_values, "--verbose"));

	{
		std::ofstream f("_init_debug.log", std::ios::trunc);
		f << "WinMain enter\n";
	}
	ms::HardwareInfo();
	ms::ScreenResolution();
	ms::start();
	{
		std::ofstream f("_init_debug.log", std::ios::app);
		f << "WinMain exit\n";
	}

	return 0;
}
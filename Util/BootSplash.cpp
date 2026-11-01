//////////////////////////////////////////////////////////////////////////////////
//	Boot splash: procedural spinner until WZ/IMG ready, then Logo.img/Nexon reel.
//
//	原生行为对齐（BeiDou V17.7 客户端：ijl15.dll 的 RemoveLogos 开关说明该片头
//	确实存在于客户端启动流程里，只是默认被跳过；本实现把它做成 boot.logo 开关、
//	默认开启）：
//	  - 资源：UI/Logo.img/Nexon，共 137 帧（第 0 帧 800x600，其余 720x480）
//	  - 画面：纯黑背景 + Logo 按屏幕居中（origin 由 Frame 自己处理）
//	  - 帧长：wz 节点不带 delay，经典客户端节奏写死在代码里；这里用
//	    boot.logo.frame_ms（默认 40ms，整段约 5.5s）
//	  - 跳过：任意按键 / 鼠标按下（Window::get_input_events）
//////////////////////////////////////////////////////////////////////////////////
#include "BootSplash.h"

#include "../Configuration.h"
#include "../Constants.h"
#include "../IO/Window.h"
#include "../Graphics/Animation.h"
#include "../Graphics/GraphicsGL.h"
#include "../Util/Misc.h"

#include <algorithm>
#include <cmath>

#include <SDL3/SDL.h>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// Logo.img/Nexon 节点；资源未就绪或缺失时返回空节点。
	static nl::node logo_source()
	{
#ifdef USE_NX
		return nl::nx::UI["Logo.img"]["Nexon"];
#else
		return nl::node();
#endif
	}

	BootSplash& BootSplash::get()
	{
		static BootSplash instance;
		return instance;
	}

	BootSplash::BootSplash()
		: logo_frames(0)
		, logo_attempted(false)
		, logo_played(false)
		, clock_started(false)
		, pending_ms(0)
		, spin_phase(0.0f)
	{
	}

	void BootSplash::clear()
	{
		// 帧纹理按需构建（见 draw_logo），这里只需复位状态标记。
		logo_frames = 0;
		logo_attempted = false;
	}

	void BootSplash::advance_clock()
	{
		const auto now = std::chrono::steady_clock::now();

		if (!clock_started)
		{
			clock_started = true;
			last_tick = now;
			pending_ms = Constants::TIMESTEP;
			return;
		}

		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count();
		last_tick = now;

		// Cap so a stalled hitch does not skip the whole reel.
		// 不用 std::clamp：部分配置未开 /std:c++17。
		int64_t capped = elapsed;
		if (capped < 1)
			capped = 1;
		else if (capped > 80)
			capped = 80;
		pending_ms = static_cast<uint16_t>(capped);
	}

	bool BootSplash::try_load_nexon()
	{
		if (logo_attempted)
			return logo_frames > 0;

		logo_attempted = true;

		nl::node Nexon = logo_source();

		// Nexon 是"容器"节点（其子节点 0..136 才是 Canvas），数据层对容器一律报
		// type::none，所以存在性必须用 operator bool 判断，不能看 data_type。
		if (!Nexon)
		{
			LOG(LOG_WARN, "[BOOT] Logo.img/Nexon missing; keep procedural splash");
			return false;
		}

		// 帧名是 "0".."136"；与 Animation 的收集规则一致，按序号顺序取位图帧。
		std::vector<int16_t> ids;

		for (auto sub : Nexon)
		{
			if (sub.data_type() != nl::node::type::bitmap)
				continue;

			int16_t fid = string_conversion::or_default<int16_t>(sub.name(), -1);

			if (fid >= 0)
				ids.push_back(fid);
		}

		std::sort(ids.begin(), ids.end());
		logo_frames = ids.size();

		if (logo_frames == 0)
		{
			LOG(LOG_WARN, "[BOOT] Logo.img/Nexon has no bitmap frames; keep procedural splash");
			return false;
		}

		LOG(LOG_INFO, "[BOOT] Logo.img/Nexon ready frames=" << logo_frames);
		return true;
	}

	void BootSplash::draw_logo(std::size_t index, const char* message)
	{
		GraphicsGL& gl = GraphicsGL::get();
		const int16_t vw = Constants::Constants::get().get_viewwidth();
		const int16_t vh = Constants::Constants::get().get_viewheight();

		// 纯黑底：非全屏窗口下 720x480 的帧不会留出桌面配色。
		gl.drawscreenfill(0.0f, 0.0f, 0.0f, 1.0f);

		nl::node Nexon = logo_source();
		nl::node frame_src = Nexon[std::to_string(index)];

		if (frame_src.data_type() == nl::node::type::bitmap)
		{
			// 逐帧构建：137 帧全量常驻约 190MB 显存，这里一次只留当前帧。
			Frame frame(frame_src);

			Point<int16_t> dim = frame.get_dimensions();

			if (dim.x() > 0 && dim.y() > 0)
			{
				// 原生片头为满屏动画：帧直接拉伸铺满视口（800x600 帧=视口本身，
				// 720x480 帧非等比拉到满屏，同旧客户端把 720x480 拉到 800x600）。
				// 注意 DrawArgument(pos, Point) 的第二参数是"平铺延伸"不是缩放，
				// 必须用 (pos, xscale, yscale) 重载（用户实测反馈"动画不是全屏"）。
				const float xscale = (vw > 0) ? static_cast<float>(vw) / dim.x() : 1.0f;
				const float yscale = (vh > 0) ? static_cast<float>(vh) / dim.y() : 1.0f;

				// 帧带 WZ origin（片头帧多为中心锚点且在 sanitize_origin 的合法
				// 范围内、不会被清零）：get_rectangle 以 center 为缩放轴且先减
				// origin，pos=center 时矩形左上 = pos - xscale*origin。取
				// pos = (xscale*origin.x, yscale*origin.y) 恰好把矩形推回
				// (0,0)-(vw,vh)，与 origin 取值无关。
				// 用 Frame::get_origin()（Texture 净化后的值），保证与实际绘制
				// 所用的 origin 完全一致。
				Point<int16_t> forigin = frame.get_origin();
				Point<int16_t> pos(
					static_cast<int16_t>(xscale * forigin.x()),
					static_cast<int16_t>(yscale * forigin.y())
				);

				frame.draw(DrawArgument(pos, xscale, yscale));
			}
		}

		if (message && message[0])
		{
			Text label(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, message);
			label.draw(DrawArgument(vw / 2, static_cast<int16_t>(vh - 36)));
		}
	}

	void BootSplash::draw_procedural(const char* message)
	{
		GraphicsGL& gl = GraphicsGL::get();
		const int16_t vw = Constants::Constants::get().get_viewwidth();
		const int16_t vh = Constants::Constants::get().get_viewheight();
		const int16_t cx = vw / 2;
		const int16_t cy = vh / 2;

		gl.drawscreenfill(0.05f, 0.05f, 0.07f, 1.0f);

		spin_phase += pending_ms * 0.012f;
		if (spin_phase > 6.2831853f)
			spin_phase -= 6.2831853f;

		constexpr int kArms = 12;
		constexpr float kRadius = 28.0f;

		for (int i = 0; i < kArms; ++i)
		{
			const float t = spin_phase + (6.2831853f * i) / kArms;
			const float fade = 0.25f + 0.75f * (static_cast<float>(i) / kArms);
			const int16_t x0 = static_cast<int16_t>(cx + std::cos(t) * (kRadius - 6.0f));
			const int16_t y0 = static_cast<int16_t>(cy + std::sin(t) * (kRadius - 6.0f) - 24);
			const int16_t x1 = static_cast<int16_t>(cx + std::cos(t) * (kRadius + 8.0f));
			const int16_t y1 = static_cast<int16_t>(cy + std::sin(t) * (kRadius + 8.0f) - 24);

			const int16_t left = std::min(x0, x1);
			const int16_t top = std::min(y0, y1);
			const int16_t w = static_cast<int16_t>(std::max(3, std::abs(x1 - x0)));
			const int16_t h = static_cast<int16_t>(std::max(3, std::abs(y1 - y0)));

			gl.drawrectangle(left, top, w, h, 0.55f, 0.75f, 1.0f, fade);
		}

		// Thin progress shimmer under the spinner (indeterminate).
		const int16_t bar_w = 180;
		const int16_t bar_h = 4;
		const int16_t bar_x = cx - bar_w / 2;
		const int16_t bar_y = cy + 20;
		gl.drawrectangle(bar_x, bar_y, bar_w, bar_h, 0.18f, 0.20f, 0.24f, 1.0f);

		const float pulse = 0.5f + 0.5f * std::sin(spin_phase * 2.0f);
		const int16_t fill_w = static_cast<int16_t>(40 + pulse * (bar_w - 40));
		const int16_t fill_x = bar_x + static_cast<int16_t>((bar_w - fill_w) * (0.5f + 0.5f * std::sin(spin_phase)));
		gl.drawrectangle(fill_x, bar_y, fill_w, bar_h, 0.45f, 0.70f, 1.0f, 0.9f);

		if (message && message[0])
		{
			Text label(Text::Font::A18M, Text::Alignment::CENTER, Color::Name::WHITE, message);
			label.draw(DrawArgument(cx, static_cast<int16_t>(cy + 48)));
		}
	}

	bool BootSplash::play_logo()
	{
		if (logo_played)
			return false;

		logo_played = true;

		if (!Configuration::get().get_boot_logo())
		{
			LOG(LOG_INFO, "[BOOT] logo reel disabled (boot.logo=false)");
			return false;
		}

		if (!try_load_nexon())
			return false;

		const uint16_t frame_ms = Configuration::get().get_boot_logo_frame_ms();
		const uint32_t baseline = Window::get().get_input_events();
		const auto started = std::chrono::steady_clock::now();

		bool skipped = false;
		std::size_t shown = 0;

		for (std::size_t i = 0; i < logo_frames; ++i)
		{
			Window::get().check_events();

			if (!Window::get().not_closed())
				break;

			// 任意按键 / 点击 = 跳过（经典客户端同样是"按一下就过"）。
			if (Window::get().get_input_events() != baseline)
			{
				skipped = true;
				break;
			}

			const auto frame_start = std::chrono::steady_clock::now();

			Window::get().begin();
			draw_logo(i, nullptr);
			Window::get().end();

			shown++;

			// 解码/上传本身耗时已占用预算时不再等，避免整段越播越慢。
			const auto spent = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - frame_start).count();
			const int64_t remain = static_cast<int64_t>(frame_ms) - spent;

			if (remain > 0)
				SDL_Delay(static_cast<uint32_t>(remain));
		}

		const auto total = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - started).count();

		LOG(LOG_INFO, "[BOOT] logo reel played frames=" << shown << "/" << logo_frames
			<< " total=" << total << "ms" << (skipped ? " (skipped)" : ""));

		return shown > 0;
	}

	void BootSplash::pump(const char* message)
	{
		if (message)
			last_message = message;

		advance_clock();

		Window::get().check_events();
		Window::get().begin();

		if (logo_frames > 0)
			draw_logo(0, last_message.c_str());
		else
			draw_procedural(last_message.c_str());

		Window::get().end();
	}
}

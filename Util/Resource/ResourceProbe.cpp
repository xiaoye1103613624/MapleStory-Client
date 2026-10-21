//////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton					//
//																			//
//	Fyms085 resource diagnostics additions Copyright (C) 2026				//
//																			//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.										//
//																			//
//	This program is distributed in the hope that it will be useful,			//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.						//
//																			//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
//////////////////////////////////////////////////////////////////////////////
#include "ResourceProbe.h"

// MapleStory.h carries the backend switches (e.g. USE_NX) the data headers key
// off of, so it has to come first.
#include "../../MapleStory.h"

#include "../../Configuration.h"
#include "../../Graphics/Texture.h"
#include "Keyring.h"
#include "../NxFiles.h"
#include "../WzFiles.h"
#include "ResourceLocator.h"
#include "ResourceService.h"

#include <nlnx/bitmap.hpp>
#include <nlnx/node.hpp>
#include <nlnx/wz.hpp>

#include <chrono>
#include <exception>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

// [DBG-AV] 临时诊断：捕获访问违规/栈溢出，记录故障地址与调用栈（含符号），
// 用于定位 img 解析期的越界写入。联调通过后与本文件其它 [DBG-*] 一并清理。
// Windows-only: VEH / SEH translators do not exist on other platforms.
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "DbgHelp.lib")

namespace {
	LONG WINAPI dbg_av_veh(EXCEPTION_POINTERS* ep) {
		auto& er = *ep->ExceptionRecord;
		if (er.ExceptionCode != EXCEPTION_ACCESS_VIOLATION &&
			er.ExceptionCode != EXCEPTION_STACK_OVERFLOW &&
			er.ExceptionCode != EXCEPTION_IN_PAGE_ERROR &&
			er.ExceptionCode != EXCEPTION_INT_DIVIDE_BY_ZERO &&
			er.ExceptionCode != EXCEPTION_FLT_DIVIDE_BY_ZERO &&
			er.ExceptionCode != 0xE06D7363) // C++ exception (shouldn't reach VEH under /EHa first)
			return EXCEPTION_CONTINUE_SEARCH;

		std::ofstream f("E:/img_av.log", std::ios::app);
		f << "[AV] code=0x" << std::hex << er.ExceptionCode;
		if (er.ExceptionCode == EXCEPTION_ACCESS_VIOLATION ||
			er.ExceptionCode == EXCEPTION_IN_PAGE_ERROR) {
			f << " access=" << (er.ExceptionInformation[0] ? "write" : "read")
				<< " addr=0x" << er.ExceptionInformation[1];
		}
		f << " ip=0x" << ep->ContextRecord->Rip << std::dec << "\n";

		HANDLE h = GetCurrentProcess();
		char exepath[MAX_PATH] = { 0 };
		GetModuleFileNameA(nullptr, exepath, MAX_PATH);
		std::string dir = exepath;
		auto sp = dir.find_last_of("\\/");
		if (sp != std::string::npos) dir = dir.substr(0, sp);
		SymInitialize(h, dir.c_str(), TRUE);

		void* frames[48];
		USHORT n = CaptureStackBackTrace(0, 48, frames, nullptr);
		char buf[sizeof(SYMBOL_INFO) + 256] = { 0 };
		for (USHORT i = 0; i < n; ++i) {
			SYMBOL_INFO* si = reinterpret_cast<SYMBOL_INFO*>(buf);
			si->SizeOfStruct = sizeof(SYMBOL_INFO);
			si->MaxNameLen = 256;
			DWORD64 disp = 0;
			if (SymFromAddr(h, reinterpret_cast<DWORD64>(frames[i]), &disp, si))
				f << "  #" << i << " " << si->Name << "+0x" << std::hex << disp << std::dec << "\n";
			else
				f << "  #" << i << " 0x" << std::hex << frames[i] << std::dec << "\n";
		}
		f.flush();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// [DBG-SEH] 临时诊断：把 SEH 结构化异常翻译成带异常码的 C++ 异常，
	// 便于在 catch(...) 中识别其类型（如 EXCEPTION_ILLEGAL_INSTRUCTION）。
	// 配合下文的逐类型 catch，定位 post-load 抛出的"未知"异常本质。
	struct SehException : public std::exception {
		DWORD code;
		mutable std::string msg;
		explicit SehException(DWORD c) : code(c) {}
		const char* what() const override {
			char buf[64];
			_snprintf_s(buf, sizeof(buf), _TRUNCATE,
				"SEH code=0x%08X", static_cast<unsigned>(code));
			msg = buf;
			return msg.c_str();
		}
	};

	void CALLBACK dbg_se_translator(unsigned int code, EXCEPTION_POINTERS*) {
		throw SehException(static_cast<DWORD>(code));
	}
}
#endif // _WIN32


namespace fs = std::filesystem;

namespace ms
{
	namespace ResourceProbe
	{
		// The client is a Windows subsystem application: there is no console and
		// LOG() is compiled away in Release, so every finding has to reach disk.
		class Report
		{
		public:
			explicit Report(const std::string& path) : stream_(path, std::ios::out | std::ios::trunc), path_(path) {}

			bool good() const
			{
				return stream_.good();
			}

			const std::string& path() const
			{
				return path_;
			}

			void line(const std::string& text)
			{
				stream_ << text << '\n';
				stream_.flush();
			}

		private:
			std::ofstream stream_;
			std::string path_;
		};

		const char* code_name(Error::Code code)
		{
			switch (code)
			{
			case Error::Code::NONE:
				return "NONE";
			case Error::Code::CONNECTION:
				return "CONNECTION";
			case Error::Code::NLNX:
				return "NLNX";
			case Error::Code::WZ:
				return "WZ";
			case Error::Code::MISSING_FILE:
				return "MISSING_FILE";
			case Error::Code::WRONG_UI_FILE:
				return "WRONG_UI_FILE";
			case Error::Code::GLFW:
				return "GLFW";
			case Error::Code::GLEW:
				return "GLEW";
			case Error::Code::FREETYPE:
				return "FREETYPE";
			case Error::Code::VERTEX_SHADER:
				return "VERTEX_SHADER";
			case Error::Code::FRAGMENT_SHADER:
				return "FRAGMENT_SHADER";
			case Error::Code::SHADER_PROGRAM_LINK:
				return "SHADER_PROGRAM_LINK";
			case Error::Code::SHADER_PROGRAM_VALID:
				return "SHADER_PROGRAM_VALID";
			case Error::Code::SHADER_VARS:
				return "SHADER_VARS";
			case Error::Code::WINDOW:
				return "WINDOW";
			case Error::Code::AUDIO:
				return "AUDIO";
			case Error::Code::MISSING_ICON:
				return "MISSING_ICON";
			case Error::Code::FONT_PATH:
				return "FONT_PATH";
			case Error::Code::DATA_FORMAT:
				return "DATA_FORMAT";
			default:
				return "UNKNOWN";
			}
		}

		std::string timestamp()
		{
			const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			std::tm fields{};

#ifdef _WIN32
			localtime_s(&fields, &now);
#else
			localtime_r(&now, &fields);
#endif

			std::ostringstream out;
			out << std::put_time(&fields, "%Y-%m-%d %H:%M:%S");

			return out.str();
		}

		int run_probe(bool verbose, const std::string& log_path)
		{
#ifdef _WIN32
			// [DBG-AV] 注册访问违规诊断处理器（仅记录，不拦截）。
			AddVectoredExceptionHandler(1, dbg_av_veh);
			// [DBG-SEH] 注册 SEH→C++ 翻译器，使结构化异常也能被类型识别。
			_set_se_translator(dbg_se_translator);
#endif

			Report report(log_path);

			if (!report.good())
				return 1;

			std::error_code ec;
			const fs::path cwd = fs::current_path(ec);

			const std::string format = Configuration::get().get_data_format();
			std::string data_dir = Configuration::get().get_data_dir();

			if (data_dir.empty())
				data_dir = "Data";

			report.line("== resource probe ==");
			report.line("timestamp          : " + timestamp());
			report.line("build              : " __DATE__ " " __TIME__);
			report.line("report             : " + fs::absolute(report.path(), ec).string());
			report.line("working directory  : " + cwd.string());
			report.line("DataFormat         : " + format);
			report.line("DataDir            : " + data_dir);

			// How much of each layout is actually where the loaders look for it.
			// Today every backend resolves relative to the working directory,
			// which is exactly the assumption this tool has to make visible.
			std::vector<std::string> wz_hits;
			std::vector<std::string> nx_hits;

			for (auto filename : WzFiles::filenames)
				if (fs::exists(fs::path(cwd) / filename, ec))
					wz_hits.emplace_back(filename);

			for (auto filename : NxFiles::filenames)
				if (fs::exists(fs::path(cwd) / filename, ec))
					nx_hits.emplace_back(filename);

			std::ostringstream head;
			head << "wz roots found     : " << wz_hits.size() << "/" << static_cast<size_t>(WzFiles::NUM_FILES);
			report.line(head.str());

			std::ostringstream nxhead;
			nxhead << "nx roots found     : " << nx_hits.size() << "/" << static_cast<size_t>(NxFiles::NUM_FILES);
			report.line(nxhead.str());

			const fs::path img_base = fs::absolute(data_dir, ec);
			const bool img_dir = fs::is_directory(img_base, ec);
			report.line("img directory      : " + img_base.string() + (img_dir ? " (present)" : " (absent)"));

			if (img_dir)
			{
				size_t subdirs = 0;

				for (auto it = fs::directory_iterator(img_base, ec); it != fs::directory_iterator(); it.increment(ec))
					if (it->is_directory(ec))
						++subdirs;

				std::ostringstream children;
				children << "img subdirectories : " << subdirs;
				report.line(children.str());
			}

			if (verbose)
			{
				report.line("-- wz candidates --");

				for (auto filename : WzFiles::filenames)
					report.line(std::string("  ") + (fs::exists(fs::path(cwd) / filename, ec) ? "[x] " : "[ ] ") + filename);

				report.line("-- nx candidates --");

				for (auto filename : NxFiles::filenames)
					report.line(std::string("  ") + (fs::exists(fs::path(cwd) / filename, ec) ? "[x] " : "[ ] ") + filename);
			}

			const auto started = std::chrono::steady_clock::now();

			// 统一经资源门面装载：后端选择、根绑定都由门面负责。
			Error error = Error::Code::NONE;
			try {
				error = ms::res::ResourceService::get().init();
			} catch (const std::exception& e) {
				report.line(std::string("[DBG-PROBE] init threw: ") + e.what());
			} catch (...) {
				report.line("[DBG-PROBE] init threw: unknown");
			}
			const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - started).count();

			// 自适应定位器视角：列出会按优先级尝试的候选目录，
			// 以及在没有显式格式要求时它会挑选哪个后端 / 目录。
			// 注意：locate() 只是只读自省（遍历目录、判断文件存在），
			// 不影响下面的真实装载。
			report.line("-- locator --");

			const std::vector<std::string> candidates = ms::res::ResourceLocator::candidates();
			std::ostringstream clist;
			clist << "candidates         : " << candidates.size();
			report.line(clist.str());

			if (verbose)
			{
				for (size_t i = 0; i < candidates.size(); i++)
					report.line(std::string("  [") + std::to_string(i) + "] " + candidates[i]);
			}

			const ms::res::Layout plan = ms::res::ResourceLocator::locate(ms::res::Format::Unknown);
			std::ostringstream planline;
			planline << "locator picks      : "
				<< (plan.found ? ms::res::to_string(plan.format) : "none")
				<< " (score=" << plan.score << ", base=" << plan.base << ")";
			report.line(planline.str());

			report.line("-- load --");

			std::ostringstream timing;
			timing << "elapsed            : " << elapsed << " ms";
			report.line(timing.str());

			// NONE means "loaded fine"; only a real error carries a retry hint.
			std::string result_line = std::string("result             : ") + code_name(error.get_code());

			if (error)
				result_line += error.can_retry() ? " (retryable)" : " (fatal)";

			report.line(result_line);

			if (const char* args = error.get_args(); args && args[0])
				report.line(std::string("detail             : ") + error.get_message() + args);
			else if (error)
				report.line(std::string("detail             : ") + error.get_message());

			// How many children the UI root actually exposes. A zero here means
			// the backend did not bind the UI root at all, which is a completely
			// different failure than "the archive is missing this one bitmap".
			// Guard the count: size() dereferences the node record, so an unbound
			// (null) node has to be handled before asking for its children.
			const size_t ui_children = nl::nx::UI ? nl::nx::UI.size() : 0;

			std::ostringstream uichildren;
			uichildren << "ui root children   : " << ui_children
				<< " (root " << (nl::nx::UI ? "bound" : "NOT bound") << ")";
			report.line(uichildren.str());

			// 密钥治理（D4）诊断：报告密钥来源模式与每归档试探次数（验证 ≤1 次试探）。
			{
				ms::res::KeyMode kmode = ms::Configuration::get().get_keymode();
				const char* kmode_str = (kmode == ms::res::KeyMode::Builtin) ? "builtin"
					: (kmode == ms::res::KeyMode::External) ? "external" : "auto";
				std::string kfile = ms::Configuration::get().get_keyfile();

				report.line("-- key --");
				report.line(std::string("keymode            : ") + kmode_str);
				report.line(std::string("keyfile            : ") + (kfile.empty() ? "(disabled)" : kfile));
				report.line("resolver trials per archive (after warming):");
				report.line(nl::key_diagnostics());
			}

			// [DBG-SB] 临时诊断：检查游戏内窗口节点存在性（按钮点击无反应时确认资源缺失）。
		{
			report.line("-- window nodes --");

			for (const char* name : { "Skill", "Item", "Equip", "Stat", "KeyConfig", "Channel", "MiniMap" })
			{
				nl::node modern = nl::nx::UI["UIWindow2.img"][name];
				nl::node classic = nl::nx::UI["UIWindow.img"][name];
				std::ostringstream os;
				os << "UIWindow[" << name << "] : UIWindow2=" << (modern ? "yes" : "no")
					<< " UIWindow=" << (classic ? "yes" : "no");

				if (classic)
				{
					// 有背景节点才算真正可用（与 UiAssetResolve::usable 语义一致）。
					os << " backgrnd=" << (classic["backgrnd"] ? "yes" : "no")
						<< " children=" << static_cast<int>(classic.size());
				}

				report.line(os.str());
			}

			// [DBG-SB] 临时诊断：复刻 UISkillBook 的背景解析路径，确认每个候选节点的位图有效性。
			// （技能窗口构造成功却不渲染，怀疑 main["backgrnd"] 缺失/无效而 fallback 只看存在性。）
			{
				report.line("-- skill window detail --");

				auto probe_tex = [&report](const char* label, nl::node n)
				{
					if (!n)
					{
						report.line(std::string(label) + " : (node missing)");
						return;
					}
					Texture t(n);
					std::ostringstream os;
					os << label << " : valid=" << (t.is_valid() ? 1 : 0)
						<< " dim=(" << t.get_dimensions().x() << "," << t.get_dimensions().y() << ")"
						<< " exists=" << (n ? 1 : 0);
					report.line(os.str());
				};

				nl::node Skill = nl::nx::UI["UIWindow.img"]["Skill"];
				nl::node main = Skill["main"];
				report.line(std::string("Skill           : exists=") + (Skill ? "yes" : "no") + " children=" + std::to_string(Skill ? static_cast<int>(Skill.size()) : 0));
				report.line(std::string("Skill/main      : exists=") + (main ? "yes" : "no") + " children=" + std::to_string(main ? static_cast<int>(main.size()) : 0));
				report.line(std::string("Skill/main/backgrnd   : exists=") + (Skill["main"]["backgrnd"] ? "yes" : "no"));
				report.line(std::string("Skill/backgrnd        : exists=") + (Skill["backgrnd"] ? "yes" : "no"));
				report.line(std::string("Skill/main/backgrnd2  : exists=") + (Skill["main"]["backgrnd2"] ? "yes" : "no"));
				report.line(std::string("Skill/main/backgrnd3  : exists=") + (Skill["main"]["backgrnd3"] ? "yes" : "no"));
				report.line(std::string("Skill/backgrnd2       : exists=") + (Skill["backgrnd2"] ? "yes" : "no"));
				report.line(std::string("Skill/backgrnd3       : exists=") + (Skill["backgrnd3"] ? "yes" : "no"));

				// 复刻 UISkillBook 的解析：main = Skill["main"] 若可用，否则 Skill
				nl::node resolved_main = main ? main : Skill;
				report.line(std::string("resolved main usable : ") + (resolved_main ? "yes" : "no"));

				nl::node ui_backgrnd = Skill["main"]["backgrnd"] ? Skill["main"]["backgrnd"] : Skill["backgrnd"];
				report.line(std::string("ui_backgrnd (as coded) chosen from : ")
					+ (Skill["main"]["backgrnd"] ? "main/backgrnd" : "Skill/backgrnd"));
				probe_tex("ui_backgrnd (as coded)", ui_backgrnd);
				probe_tex("Skill/backgrnd (fallback)", Skill["backgrnd"]);
				probe_tex("Skill/main/backgrnd", Skill["main"]["backgrnd"]);
			}
		}

		// [DBG-PROBE] 临时注释版本检查/服务诊断以定位崩溃点。
			report.line("[DBG-PROBE] reached post-key section");

			try {
			// Version check discovery: both the NX and the WZ backend accept a UI
			// archive without this bitmap, then fail later with a confusing
			// symptom, so probe it explicitly.
			constexpr const char* POSTCHAOS_BITMAP = "Login.img/WorldSelect/BtChannel/layer:bg";

			const auto ui_type = nl::nx::UI.resolve(POSTCHAOS_BITMAP).data_type();

			std::ostringstream version;
			version << "ui version check   : " << POSTCHAOS_BITMAP << " -> ";

			switch (ui_type)
			{
			case nl::node::type::bitmap:
				report.line(version.str() + "bitmap (ok)");
				break;
			case nl::node::type::none:
				report.line(version.str() + "none (missing)");
				break;
			default:
				report.line(version.str() + "unexpected type");
				break;
			}

			// 门面视角的装载结果：格式、后端、已绑定/缺失的根与能力标记。
			// 与上面的"根文件扫描"互为印证——前者看磁盘上有什么，后者看实际绑定了什么。
			report.line("service            : " + ms::res::ResourceService::get().diagnostics(verbose));
			report.line("base               : " + ms::res::ResourceService::get().base());
			} catch (const std::exception& e) {
				report.line(std::string("[DBG-PROBE] post-load threw std::exception: ") + e.what());
#ifdef _WIN32
			} catch (const SehException& se) {
				report.line(std::string("[DBG-PROBE] post-load threw ") + se.what());
#endif
			} catch (const char* s) {
				report.line(std::string("[DBG-PROBE] post-load threw const char*: ") + (s ? s : "(null)"));
			} catch (const std::string& s) {
				report.line(std::string("[DBG-PROBE] post-load threw std::string: ") + s);
			} catch (int i) {
				report.line(std::string("[DBG-PROBE] post-load threw int: ") + std::to_string(i));
			} catch (...) {
				std::string info = "[DBG-PROBE] post-load threw: truly-unknown (non-std/SEH)";
				try {
					throw;
				} catch (const std::exception& e2) {
					info = std::string("[DBG-PROBE] post-load threw (rethrow std): ") + e2.what();
				} catch (...) {
				}
				report.line(info);
			}

			report.line(error ? "probe result       : FAILED" : "probe result       : OK");

			return error ? 1 : 0;
		}

		int run_selftest(const std::string& log_path)
		{
			Report report(log_path);

			if (!report.good())
				return 1;

			int failures = 0;

			auto check = [&](bool ok, const char* expression, int line)
			{
				if (!ok)
					++failures;

				std::ostringstream entry;
				entry << (ok ? "[PASS] " : "[FAIL] ") << expression << " (line " << line << ")";
				report.line(entry.str());
			};

#define EXPECT(cond) check((cond), #cond, __LINE__)

			report.line("== resource selftest ==");

			// Both layouts must describe the very same set of roots. A mismatch
			// means one backend silently serves empty nodes where the other has
			// data, which shows up as "missing graphics depending on DataFormat".
			EXPECT(WzFiles::NUM_FILES == NxFiles::NUM_FILES);

			bool names_aligned = WzFiles::NUM_FILES == NxFiles::NUM_FILES;

			for (uint8_t i = 0; names_aligned && i < WzFiles::NUM_FILES; ++i)
			{
				const fs::path wz_name = WzFiles::filenames[i];
				const fs::path nx_name = NxFiles::filenames[i];

				names_aligned = wz_name.stem() == nx_name.stem()
					&& wz_name.extension() == ".wz"
					&& nx_name.extension() == ".nx";
			}

			EXPECT(names_aligned);

			// Retry contract: the start loop only offers "retry" when the code
			// says so, so a layout problem must never be classified fatal.
			EXPECT(Error(Error::Code::DATA_FORMAT).can_retry());
			EXPECT(Error(Error::Code::MISSING_FILE).can_retry());
			EXPECT(Error(Error::Code::WRONG_UI_FILE).can_retry());
			EXPECT(!Error(Error::Code::NLNX).can_retry());
			EXPECT(!Error(Error::Code::WZ).can_retry());
			EXPECT(!static_cast<bool>(Error(Error::Code::NONE)));
			EXPECT(static_cast<bool>(Error(Error::Code::MISSING_FILE)));

			// The dispatcher must not silently accept an empty format value:
			// an empty string used to fall through to the WZ backend and was
			// reported as "WZ data is broken".
			EXPECT(!Configuration::get().get_data_format().empty());

#undef EXPECT

			std::ostringstream tail;
			tail << "-- " << failures << " failure(s) --";
			report.line(tail.str());

			return failures;
		}
	}
}

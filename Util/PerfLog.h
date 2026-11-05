////////////////////////////////////////////////////////////////////////////////
//	PerfLog —— Release 可用的毫秒级分段计时文件日志（Wave 0.1 测量仪器）。
//
//	背景：Release 构建里 LOG() 是 no-op（MapleStory.h 的 _DEBUG 门控，已坐实），
//	[BOOT]/[SetField] 分段计时从来没有真数字落盘，perf-budget 基线因此一直空。
//	本类走与 LOG 无关的文件通道：
//	  - 开关：debug.perf_log（xy-config.yaml / Settings）。调试期默认 true，
//	    基线采集完成后按门禁改回默认 false 并降为观测注释。
//	  - 落点：exe 工作目录 boot_perf.log（追加）。每行含相对进程起点的毫秒，
//	    跨线程安全（低频写 + mutex；进图类事件每秒 <10 条，不构成热点）。
//	  - 上限：5000 行封顶（同 dbgatlas 200 条思路，本器面向会话级，更宽）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>

namespace ms
{
	namespace perf
	{
		// 记录一行 "[PERF] t=<ms> <tag> <msg>"。开关关闭时一次 bool 读取即返回。
		void mark(const char* tag, const std::string& msg);
	}
}

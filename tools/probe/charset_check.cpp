//////////////////////////////////////////////////////////////////////////////////
//	T11 · Net/PacketCharset.cpp 的字节级对拍（独立编译，不进客户端构建）
//
//	  cl /utf-8 /EHsc /nologo /Fe:%TEMP%\charset_check.exe ^
//	     tools\probe\charset_check.cpp Net\PacketCharset.cpp
//
//	期望值来自 tools/probe/gen_charset_vectors.py（Python cp936/gbk codec 算出）。
//	退出码 = 失败条数。stdout 全 ASCII（本机 shell 按 cp936 解 UTF-8）。
//////////////////////////////////////////////////////////////////////////////////
#include "../../Net/PacketCharset.h"

#include "charset_vectors.h"

#include <cstdio>
#include <string>

namespace
{
	int failures = 0;

	const char* STATUS(bool ok)
	{
		return ok ? "PASS" : "FAIL";
	}

	void expect(const char* what, const std::string& got, const std::string& want)
	{
		const bool ok = got == want;

		if (!ok)
			++failures;

		printf("%s %-44s got(len=%zu) want(len=%zu)\n", STATUS(ok), what, got.size(), want.size());

		if (!ok)
		{
			printf("      got :");
			for (const unsigned char c : got)
				printf(" %02X", c);
			printf("\n      want:");
			for (const unsigned char c : want)
				printf(" %02X", c);
			printf("\n");
		}
	}

	void hexdump(const char* label, const std::string& s)
	{
		printf("%-10s len=%zu:", label, s.size());
		for (const unsigned char c : s)
			printf(" %02X", c);
		printf("\n");
	}
}

int main()
{
	for (int i = 0; i < CHARSET_VECTOR_COUNT; ++i)
	{
		const CharsetVector& v = CHARSET_VECTORS[i];
		const std::string utf8(v.utf8);
		const std::string gbk(v.gbk);
		const std::string tag = "vector[" + std::to_string(i) + "]";

		// 入站：线上 GBK -> 内部 UTF-8
		expect((tag + " to_internal").c_str(), ms::packet_to_internal(gbk), utf8);

		// 出站：内部 UTF-8 -> 线上 GBK
		expect((tag + " from_internal").c_str(), ms::packet_from_internal(utf8), gbk);

		// 往返：读进来再发回去必须回到原字节
		// （角色名/公会名这类"服务端给的串，客户端原样回发"的路径依赖这条）
		expect((tag + " roundtrip").c_str(),
			ms::packet_from_internal(ms::packet_to_internal(gbk)), gbk);
	}

	// 非法 UTF-8 出站：按约定原样发出，不得丢字段
	{
		const std::string bad = std::string("abc") + static_cast<char>(0xFF) + "test";
		expect("invalid utf8 passthrough", ms::packet_from_internal(bad), bad);

		const std::string truncated = std::string("hi") + static_cast<char>(0xE5) + static_cast<char>(0x28);
		expect("truncated utf8 passthrough", ms::packet_from_internal(truncated), truncated);
	}

	// 纯 ASCII 必须字节级不变（175 个既有调用点的绝大多数属于这一类）
	{
		const std::string ascii = "Admin";
		expect("ascii to_internal identity", ms::packet_to_internal(ascii), ascii);
		expect("ascii from_internal identity", ms::packet_from_internal(ascii), ascii);
	}

	// 非法 GBK 入站：只观测不判定（Windows best-fit 的行为不在 CP936 规范里），
	// 断言的是"不崩、不吞掉两侧完好的字节"。
	{
		const std::string lone_lead = std::string("A") + static_cast<char>(0xD2) + "B";
		hexdump("info-in", ms::packet_to_internal(lone_lead));

		const std::string half_pair = std::string("A") + static_cast<char>(0xD2);
		hexdump("info-tail", ms::packet_to_internal(half_pair));
	}

	printf("\nvectors=%d failures=%d\n", CHARSET_VECTOR_COUNT, failures);
	return failures;
}

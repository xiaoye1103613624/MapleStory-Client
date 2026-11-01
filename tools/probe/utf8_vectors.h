// 自动生成，勿手改：python tools/probe/gen_utf8_vectors.py
// 被测对象：Graphics/Utf8.h 的 utf8_next()
// Python 3.14.4 / 样本 26 条（oracle 14 + rule 12）
#pragma once

#include <cstddef>
#include <cstdint>

struct Utf8Vector
{
	const char* label;
	const uint8_t* bytes;
	size_t nbytes;
	const uint32_t* cps;
	const uint32_t* consumed;
	size_t nchars;
};

	static const uint8_t v0_bytes[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F };
	static const uint32_t v0_cps[] = { 72, 101, 108, 108, 111 };
	static const uint32_t v0_consumed[] = { 1, 1, 1, 1, 1 };
	static const uint8_t v1_bytes[] = { 0x41, 0x64, 0x6D, 0x69, 0x6E, 0x20, 0xE6, 0xAD, 0xA3, 0xE5, 0x9C, 0xA8, 0xE7, 0x9B, 0x91, 0xE6, 0x8E, 0xA7, 0x20, 0x31 };
	static const uint32_t v1_cps[] = { 65, 100, 109, 105, 110, 32, 27491, 22312, 30417, 25511, 32, 49 };
	static const uint32_t v1_consumed[] = { 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1, 1 };
	static const uint8_t v2_bytes[] = { 0xE5, 0xB7, 0xB2, 0xE8, 0xA2, 0xAB, 0xE7, 0x9B, 0x91, 0xE6, 0x8E, 0xA7 };
	static const uint32_t v2_cps[] = { 24050, 34987, 30417, 25511 };
	static const uint32_t v2_consumed[] = { 3, 3, 3, 3 };
	static const uint8_t v3_bytes[] = { 0xE5, 0xAF, 0xB9, 0xE6, 0x89, 0x80, 0xE6, 0x9C, 0x89, 0xE4, 0xBA, 0xBA };
	static const uint32_t v3_cps[] = { 23545, 25152, 26377, 20154 };
	static const uint32_t v3_consumed[] = { 3, 3, 3, 3 };
	static const uint8_t v4_bytes[] = { 0xE8, 0x98, 0x91, 0xE8, 0x8F, 0x87, 0xE7, 0x8E, 0x8B, 0xE5, 0x9B, 0xBD };
	static const uint32_t v4_cps[] = { 34321, 33735, 29579, 22269 };
	static const uint32_t v4_consumed[] = { 3, 3, 3, 3 };
	static const uint8_t v5_bytes[] = { 0xE6, 0x96, 0xB0, 0xE6, 0x89, 0x8B, 0x2F, 0xE6, 0x8E, 0x92, 0xE8, 0xA1, 0x8C, 0xE4, 0xB8, 0x8D, 0xE5, 0x8F, 0xAF, 0xE7, 0x94, 0xA8 };
	static const uint32_t v5_cps[] = { 26032, 25163, 47, 25490, 34892, 19981, 21487, 29992 };
	static const uint32_t v5_consumed[] = { 3, 3, 1, 3, 3, 3, 3, 3 };
	static const uint8_t v6_bytes[] = { 0x61, 0xC3, 0xA9, 0xE2, 0x82, 0xAC };
	static const uint32_t v6_cps[] = { 97, 233, 8364 };
	static const uint32_t v6_consumed[] = { 1, 2, 3 };
	static const uint8_t v7_bytes[] = { 0xE3, 0x80, 0x90, 0xE6, 0xB5, 0x8B, 0xE8, 0xAF, 0x95, 0xE3, 0x80, 0x91, 0xC2, 0xB7, 0xE2, 0x80, 0x94, 0xE2, 0x80, 0x9C, 0xE2, 0x80, 0x9D };
	static const uint32_t v7_cps[] = { 12304, 27979, 35797, 12305, 183, 8212, 8220, 8221 };
	static const uint32_t v7_consumed[] = { 3, 3, 3, 3, 2, 3, 3, 3 };
	static const uint8_t v8_bytes[] = { 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80 };
	static const uint32_t v8_cps[] = { 54620, 44544 };
	static const uint32_t v8_consumed[] = { 3, 3 };
	static const uint8_t v9_bytes[] = { 0xF0, 0x9F, 0x8E, 0x83 };
	static const uint32_t v9_cps[] = { 127875 };
	static const uint32_t v9_consumed[] = { 4 };
	static const uint8_t v10_bytes[] = { 0xC2, 0x80, 0xDF, 0xBF, 0xE0, 0xA0, 0x80, 0xEF, 0xBF, 0xBF };
	static const uint32_t v10_cps[] = { 128, 2047, 2048, 65535 };
	static const uint32_t v10_consumed[] = { 2, 2, 3, 3 };
	static const uint8_t v11_bytes[] = { 0xF0, 0x90, 0x80, 0x80, 0xF4, 0x8F, 0xBF, 0xBF };
	static const uint32_t v11_cps[] = { 65536, 1114111 };
	static const uint32_t v11_consumed[] = { 4, 4 };
	static const uint8_t v12_bytes[] = { 0x4C, 0x69, 0x6E, 0x65, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x20, 0x73, 0x70, 0x61, 0x63, 0x65, 0x73, 0x20, 0x20, 0x69, 0x6E, 0x73, 0x69, 0x64, 0x65 };
	static const uint32_t v12_cps[] = { 76, 105, 110, 101, 32, 119, 105, 116, 104, 32, 32, 115, 112, 97, 99, 101, 115, 32, 32, 105, 110, 115, 105, 100, 101 };
	static const uint32_t v12_consumed[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	static const uint8_t v13_bytes[] = { 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, 0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD };
	static const uint32_t v13_cps[] = { 20320, 22909, 20320, 22909, 20320, 22909, 20320, 22909, 20320, 22909, 20320, 22909, 20320, 22909, 20320, 22909, 20320, 22909, 20320, 22909 };
	static const uint32_t v13_consumed[] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
	static const uint8_t v14_bytes[] = { 0x80 };
	static const uint32_t v14_cps[] = { 0 };
	static const uint32_t v14_consumed[] = { 1 };
	static const uint8_t v15_bytes[] = { 0x41, 0x80 };
	static const uint32_t v15_cps[] = { 65, 0 };
	static const uint32_t v15_consumed[] = { 1, 1 };
	static const uint8_t v16_bytes[] = { 0xE2 };
	static const uint32_t v16_cps[] = { 0 };
	static const uint32_t v16_consumed[] = { 1 };
	static const uint8_t v17_bytes[] = { 0xE2, 0x98 };
	static const uint32_t v17_cps[] = { 0, 0 };
	static const uint32_t v17_consumed[] = { 1, 1 };
	static const uint8_t v18_bytes[] = { 0xF0, 0x9F, 0x8E };
	static const uint32_t v18_cps[] = { 0, 0, 0 };
	static const uint32_t v18_consumed[] = { 1, 1, 1 };
	static const uint8_t v19_bytes[] = { 0x00, 0x41 };
	static const uint32_t v19_cps[] = { 0, 65 };
	static const uint32_t v19_consumed[] = { 1, 1 };
	static const uint8_t v20_bytes[] = { 0x09, 0x41 };
	static const uint32_t v20_cps[] = { 0, 65 };
	static const uint32_t v20_consumed[] = { 1, 1 };
	static const uint8_t v21_bytes[] = { 0x7F };
	static const uint32_t v21_cps[] = { 127 };
	static const uint32_t v21_consumed[] = { 1 };
	static const uint8_t v22_bytes[] = { 0x20, 0xE2, 0x98 };
	static const uint32_t v22_cps[] = { 32, 0, 0 };
	static const uint32_t v22_consumed[] = { 1, 1, 1 };
	static const uint8_t v23_bytes[] = { 0xE2, 0x98, 0x41 };
	static const uint32_t v23_cps[] = { 9729 };
	static const uint32_t v23_consumed[] = { 3 };
	static const uint8_t v24_bytes[] = { 0xF8, 0x88, 0x80, 0x80, 0x80 };
	static const uint32_t v24_cps[] = { 32768, 0 };
	static const uint32_t v24_consumed[] = { 4, 1 };
	static const uint8_t v25_bytes[] = { 0xC0, 0x80 };
	static const uint32_t v25_cps[] = { 0 };
	static const uint32_t v25_consumed[] = { 2 };

static const Utf8Vector UTF8_VECTORS[] =
{
	{ "str:48656c6c6f", v0_bytes, 5, v0_cps, v0_consumed, 5 },
	{ "str:41646d696e20e6ada3e59ca8e79b91e68ea72031", v1_bytes, 20, v1_cps, v1_consumed, 12 },
	{ "str:e5b7b2e8a2abe79b91e68ea7", v2_bytes, 12, v2_cps, v2_consumed, 4 },
	{ "str:e5afb9e68980e69c89e4baba", v3_bytes, 12, v3_cps, v3_consumed, 4 },
	{ "str:e89891e88f87e78e8be59bbd", v4_bytes, 12, v4_cps, v4_consumed, 4 },
	{ "str:e696b0e6898b2fe68e92e8a18ce4b88de58fafe794a8", v5_bytes, 22, v5_cps, v5_consumed, 8 },
	{ "str:61c3a9e282ac", v6_bytes, 6, v6_cps, v6_consumed, 3 },
	{ "str:e38090e6b58be8af95e38091c2b7e28094e2809ce2809d", v7_bytes, 23, v7_cps, v7_consumed, 8 },
	{ "str:ed959ceab880", v8_bytes, 6, v8_cps, v8_consumed, 2 },
	{ "str:f09f8e83", v9_bytes, 4, v9_cps, v9_consumed, 1 },
	{ "str:c280dfbfe0a080efbfbf", v10_bytes, 10, v10_cps, v10_consumed, 4 },
	{ "str:f0908080f48fbfbf", v11_bytes, 8, v11_cps, v11_consumed, 2 },
	{ "str:4c696e65207769746820207370616365732020696e73696465", v12_bytes, 25, v12_cps, v12_consumed, 25 },
	{ "str:e4bda0e5a5bde4bda0e5a5bde4bda0e5a5bde4bda0e5a5bde4bda0e5a5bde4bda0e5a5bde4bda0e5a5bde4bda0e5a5bde4bda0e5a5bde4bda0e5a5bd", v13_bytes, 60, v13_cps, v13_consumed, 20 },
	{ "rule:lone-continuation", v14_bytes, 1, v14_cps, v14_consumed, 1 },
	{ "rule:truncated-after-ascii", v15_bytes, 2, v15_cps, v15_consumed, 2 },
	{ "rule:truncated-at-eof", v16_bytes, 1, v16_cps, v16_consumed, 1 },
	{ "rule:truncated-mid", v17_bytes, 2, v17_cps, v17_consumed, 2 },
	{ "rule:truncated-4byte", v18_bytes, 3, v18_cps, v18_consumed, 3 },
	{ "rule:control-nul", v19_bytes, 2, v19_cps, v19_consumed, 2 },
	{ "rule:control-tab", v20_bytes, 2, v20_cps, v20_consumed, 2 },
	{ "rule:del-7f", v21_bytes, 1, v21_cps, v21_consumed, 1 },
	{ "rule:space-then-truncated", v22_bytes, 3, v22_cps, v22_consumed, 3 },
	{ "rule:lax:non-continuation-tail", v23_bytes, 3, v23_cps, v23_consumed, 1 },
	{ "rule:lax:lead-f8-as-4byte", v24_bytes, 5, v24_cps, v24_consumed, 2 },
	{ "rule:lax:overlong-nul", v25_bytes, 2, v25_cps, v25_consumed, 1 },

};

static const size_t UTF8_VECTOR_COUNT = 26;

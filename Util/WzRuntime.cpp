////////////////////////////////////////////////////////////////////////////////
// Direct WZ runtime adapter for the NoLifeNx nl::node API.
//
// The parser is the AGPL NoLifeWzToNx parser already shipped with this source
// tree. Runtime mode stops after metadata parsing: WZ archives remain mapped,
// audio points into the archive, and Canvas payloads are decoded on demand.
////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX
#define LZ4_DISABLE_DEPRECATE_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <Windows.h>
#endif

#include <zlib.h>
#include <lz4.h>
#include <lz4hc.h>
#include <squish.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <codecvt>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <numeric>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <nlnx/audio.hpp>
#include <nlnx/file_impl.hpp>
#include <nlnx/node_impl.hpp>
#include <nlnx/nx.hpp>
#include <nlnx/wz.hpp>

#include "../Configuration.h"
#include "Resource/Keyring.h"  // D4：ms::res::KeyResolver / KeyMode 定义（本 TU 直接用到，须显式包含）。

// The converter's structures intentionally mirror NoLifeNx structures. Rename
// its three public value types while embedding it in the client translation
// unit so both APIs can coexist.
#define NL_WZ_PARSER_EMBEDDED
#define node wz_parser_node
#define bitmap wz_parser_bitmap
#define audio wz_parser_audio
#include "../tools/vendor/NoLifeWzToNx-master/NoLifeWzToNx.cpp"
#undef audio
#undef bitmap
#undef node
#undef NL_WZ_PARSER_EMBEDDED

namespace nl {
    static_assert(sizeof(wz_parser_node) == sizeof(node::data),
        "WZ and NX node records must remain binary-compatible");

    struct wz_file::data {
        struct redirect {
            wz_file * file = nullptr;
            uint32_t node = 0;
        };

        std::unique_ptr<wztonx> parser;
        _file_data view;
        std::vector<uint32_t> parents;
        std::vector<redirect> redirects;
        std::vector<uint8_t> resolve_state;
        std::string archive_name;  // 归档标识（如文件名），用于 per-archive 密钥缓存与诊断。
    };

    namespace {
        // 密钥解析器（D4）：单例，按配置 data.keymode / data.keyfile 构造。
        ms::res::KeyResolver * s_keyresolver = nullptr;
    }

    // 惰性构造并复用全局 KeyResolver 实例（首次解码时触发，读取 Configuration）。
    ms::res::KeyResolver & get_keyresolver() {
        if (!s_keyresolver) {
            ms::res::KeyMode mode = ms::Configuration::get().get_keymode();
            std::string path = ms::Configuration::get().get_keyfile();
            s_keyresolver = new ms::res::KeyResolver(mode, path);
        }
        return *s_keyresolver;
    }

    // 密钥诊断（D4）：返回每归档的试探次数，用于验证"每归档 ≤1 次试探"。
    std::string key_diagnostics() {
        return get_keyresolver().diagnostics();
    }

    // [SAFETY] SEH 兜底：畸形 .img 可能触发底层解析器的越界访问（AV）。
    // 用仅含 POD 参数的函数承载 __try/__except，避免 C2712（含 C++ 对象的
    // 函数禁用 __try）。返回 false 表示发生了访问违规。
    static void seh_call_parse_file(void * p) {
        static_cast<wztonx *>(p)->parse_file();
    }
    static void seh_call_parse_single(void * p) {
        static_cast<imgdirtonx *>(p)->parse_single();
    }
    static void seh_call_parse_only(void * p) {
        static_cast<imgdirtonx *>(p)->parse_only_stored();
    }
    // SEH is MSVC/Windows only; other compilers just run the callee directly.
#ifdef _WIN32
    static bool seh_try_invoke(void (*fn)(void *), void * ctx) {
        __try {
            fn(ctx);
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }
#else
    static bool seh_try_invoke(void (*fn)(void *), void * ctx) {
        fn(ctx);
        return true;
    }
#endif

    namespace {
        std::unordered_map<std::string, wz_file *> archives;

        std::vector<std::string> split_wz_path(std::string const & path) {
            std::vector<std::string> parts;
            std::istringstream stream(path);
            std::string part;
            while (std::getline(stream, part, '/'))
                if (!part.empty())
                    parts.push_back(std::move(part));
            return parts;
        }
    }

    wz_file::wz_file() = default;

    wz_file::wz_file(std::string const & filename) {
        open(filename);
    }

    wz_file::~wz_file() = default;

    void wz_file::close() {
        m_data.reset();
    }

    void wz_file::finalize(std::unique_ptr<data> next) {
        next->view.node_table = reinterpret_cast<node::data const *>(
            next->parser->nodes.data());
        next->view.wz = this;
        next->parents.assign(next->parser->nodes.size(), UINT32_MAX);
        next->redirects.resize(next->parser->nodes.size());
        next->resolve_state.assign(next->parser->nodes.size(), 0);
        for (uint32_t parent = 0; parent < next->parser->nodes.size(); ++parent) {
            auto const & entry = next->parser->nodes[parent];
            for (uint32_t i = 0; i < entry.num; ++i) {
                auto const child = entry.children + i;
                if (child < next->parents.size())
                    next->parents[child] = parent;
            }
        }
        m_data = std::move(next);
    }

    void wz_file::open(std::string const & filename) {
        close();
        auto next = std::make_unique<data>();
        next->archive_name = std::filesystem::path(filename).filename().generic_string();
        next->parser = std::make_unique<wztonx>(std::filesystem::u8path(filename), true, false);
        next->parser->parse_file();
        finalize(std::move(next));
    }

    // ---- 直接 IMG 后端（零转换） ----
    // 复用 wz_file 的 _file_data 布局与全部 bitmap/audio 解码方法：img 本质是一个
    // img-node 字节流，与 WZ 的 Canvas/Audio 编码同源，解析出的 nodes/strings/
    // bitmaps/audios 结构二进制兼容（见 WzRuntime.cpp 顶部 static_assert）。

    // 把一组 (相对路径, .img 字节) 按 "/" 拆成嵌套目录树。中间目录标记为
    // is_dir，叶子携带原始 .img 字节。open_merge / open_tree 共用，保证
    // "Obj/login.img" 这类带层级路径被正确还原为 Map["Obj"]["login.img"]，
    // 而非扁平地把整条 relpath 当作顶层节点名（那会导致分级访问全部失效）。
    static tree_node build_img_tree(
        std::vector<std::pair<std::string, std::vector<char>>> && bufs)
    {
        tree_node root_tree;
        for (auto & rp : bufs) {
            std::string rel = rp.first;
            std::vector<std::string> segs;
            size_t start = 0;
            while (true) {
                size_t pos = rel.find_first_of("/\\", start);
                if (pos == std::string::npos) {
                    segs.push_back(rel.substr(start));
                    break;
                }
                segs.push_back(rel.substr(start, pos - start));
                start = pos + 1;
            }

            tree_node * cur = &root_tree;
            for (size_t i = 0; i + 1 < segs.size(); ++i) {
                cur = &cur->children[segs[i]];
                cur->is_dir = true;
            }
            tree_node & leaf = cur->children[segs.back()];
            leaf.is_dir = false;
            leaf.buf = std::move(rp.second);
        }
        return root_tree;
    }

    void img_file::open_merge(std::vector<std::pair<std::string, std::vector<char>>> && bufs) {
        auto next = std::make_unique<data>();
        next->archive_name = "img";
        auto conv = std::make_unique<imgdirtonx>();
        conv->client = true;
        conv->hc = false;
        // [FIX] 改用嵌套 tree 模式（与 open_tree 一致），按 "/" 重建目录层级，
        // 使 Map["Obj"]["login.img"] 这类分级访问可用；扁平模式会把整条
        // relpath 当顶层节点名，导致 Map/Obj/*、Map/Tile/*、Map/Back/* 全部失效。
        conv->tree = build_img_tree(std::move(bufs));
        // [SAFETY] SEH 兜底：畸形 .img 可能触发解析器越界访问，捕获后抛出，
        // 由 ImgSource::ensure 捕获并跳过该根，避免整体装载崩溃。
        if (!seh_try_invoke(&seh_call_parse_only, conv.get()))
            throw std::runtime_error("img parse access violation");
        next->parser = std::move(conv);
        finalize(std::move(next));
    }

    void img_file::open_single(std::vector<char> && buf) {
        auto next = std::make_unique<data>();
        next->archive_name = "img";
        auto conv = std::make_unique<imgdirtonx>();
        conv->client = true;
        conv->hc = false;
        conv->single_buf = std::move(buf);
        if (!seh_try_invoke(&seh_call_parse_single, conv.get()))
            throw std::runtime_error("img parse access violation");
        next->parser = std::move(conv);
        finalize(std::move(next));
    }

    void img_file::open_tree(std::vector<std::pair<std::string, std::vector<char>>> && bufs) {
        auto next = std::make_unique<data>();
        next->archive_name = "img";
        auto conv = std::make_unique<imgdirtonx>();
        conv->client = true;
        conv->hc = false;

        // 复用 build_img_tree 重建嵌套目录树（与 open_merge 同逻辑）。
        // tree_node 由 conv 持久持有，避免解析后 bitmap/audio 解码时 base 指针悬垂。
        conv->tree = build_img_tree(std::move(bufs));
        if (!seh_try_invoke(&seh_call_parse_only, conv.get()))
            throw std::runtime_error("img parse access violation");
        next->parser = std::move(conv);
        finalize(std::move(next));
    }

    node wz_file::root() const {
        if (!m_data || !m_data->view.node_table)
            return {};
        return {m_data->view.node_table, &m_data->view};
    }

    wz_file::operator node() const {
        return root();
    }

    std::string const & wz_file::get_string(uint32_t id) const {
        static std::string const empty;
        if (!m_data || id >= m_data->parser->strings.size())
            return empty;
        return m_data->parser->strings[id];
    }

    uint32_t wz_file::find_child(uint32_t parent, char const * name,
        uint16_t length) const {
        if (!m_data || parent >= m_data->parser->nodes.size())
            return 0;
        return m_data->parser->get_child_full(parent, std::string{name, length});
    }

    void const * wz_file::get_audio(uint32_t id) const {
        if (!m_data || id >= m_data->parser->audios.size())
            return nullptr;
        auto const & entry = m_data->parser->audios[id];
        // [FIX] 与位图同理：音频偏移相对它自己所属 .img 的 entry.base，
        // 而非解析结束后残留的 parser.in.base。
        return entry.base + entry.data;
    }

    uint32_t wz_file::resolve_path(uint32_t start,
        std::vector<std::string> const & parts, size_t first) const {
        if (!m_data || start >= m_data->parser->nodes.size())
            return 0;
        auto current = start;
        for (size_t i = first; i < parts.size(); ++i) {
            current = m_data->parser->get_child_full(current, parts[i]);
            if (current == 0)
                return 0;
        }
        return current;
    }

    bool wz_file::resolve_link(uint32_t node_id) {
        if (!m_data || node_id >= m_data->parser->nodes.size())
            return false;
        auto & state = m_data->resolve_state[node_id];
        if (state == 2)
            return m_data->redirects[node_id].file != nullptr;
        if (state == 1)
            return false;
        state = 1;

        auto & parser = *m_data->parser;
        char const * link_names[] = {"_outlink", "_inlink", "source"};
        for (auto const * link_name : link_names) {
            auto const link_node = parser.get_child_full(node_id, link_name);
            if (link_node == 0)
                continue;
            auto const & link = parser.nodes[link_node];
            if (static_cast<uint16_t>(link.data_type) != 3 ||
                link.data.string >= parser.strings.size())
                continue;

            auto const parts = split_wz_path(parser.strings[link.data.string]);
            if (parts.empty())
                continue;

            wz_file * target_file = this;
            uint32_t target_node = 0;
            if (std::strcmp(link_name, "_outlink") == 0) {
                auto const archive = archives.find(parts.front());
                if (archive != archives.end()) {
                    target_file = archive->second;
                    target_node = target_file->resolve_path(0, parts, 1);
                }
            } else if (std::strcmp(link_name, "_inlink") == 0) {
                auto base = node_id;
                // [SAFETY] 父链可能因解析异常而成环，或 base 越界读到垃圾，
                // 原 while 会永不终止并让整个装载卡死。这里加上限与边界检查
                // （与 resolve_bitmap 的 depth<64 一致），最多沿父链上溯 64 跳。
                for (size_t hops = 0; base != UINT32_MAX && hops < 64u; ++hops) {
                    if (base >= m_data->parents.size())
                        break;
                    target_node = resolve_path(base, parts);
                    if (target_node != 0)
                        break;
                    base = m_data->parents[base];
                }
            } else {
                target_node = resolve_path(0, parts);
            }

            if (target_node != 0) {
                target_file->resolve_link(target_node);
                m_data->redirects[node_id] = {target_file, target_node};
                state = 2;
                return true;
            }
        }

        state = 2;
        return false;
    }

    void wz_file::resolve_links() {
        if (!m_data)
            return;
        for (uint32_t i = 0; i < m_data->parser->nodes.size(); ++i) {
            if (static_cast<uint16_t>(m_data->parser->nodes[i].data_type) == 5)
                resolve_link(i);
        }
    }

    bool wz_file::resolve_bitmap(uint32_t node_id, wz_file * & owner,
        uint32_t & bitmap_id, uint16_t & width, uint16_t & height) {
        auto * file = this;
        auto current = node_id;
        for (size_t depth = 0; depth < 64; ++depth) {
            if (!file->m_data || current >= file->m_data->parser->nodes.size())
                return false;
            file->resolve_link(current);
            auto const redirect = file->m_data->redirects[current];
            if (redirect.file) {
                file = redirect.file;
                current = redirect.node;
                continue;
            }
            auto const & entry = file->m_data->parser->nodes[current];
            if (static_cast<uint16_t>(entry.data_type) != 5)
                return false;
            owner = file;
            bitmap_id = entry.data.wz_parser_bitmap.id;
            width = entry.data.wz_parser_bitmap.width;
            height = entry.data.wz_parser_bitmap.height;
            return true;
        }
        return false;
    }

    void wz_file::decode_bitmap(uint32_t id, std::vector<char> & destination) {
        destination.clear();
        if (!m_data || id >= m_data->parser->bitmaps.size())
            return;

        auto & parser = *m_data->parser;
        auto const & entry = parser.bitmaps[id];
        auto & input_file = parser.in;

        // [FIX] 位图偏移 entry.data 相对的是它自己所属的 .img 缓冲区
        // (entry.base)。合并解析多个 .img 时 parser.in.base 会被逐个覆盖，
        // 解析结束后只保留最后处理的 .img；直接用它 seek 会算出越界地址并
        // 触发 AV（进程在无交互会话里表现为挂起）。这里先把 reader 基址改绑
        // 到 entry.base 再 seek，与 vendor write_bitmaps 的用法一致。
        input_file.base = entry.base;
        input_file.end = nullptr;
        input_file.seek(static_cast<size_t>(entry.data));

        auto const width = input_file.read_cint();
        auto const height = input_file.read_cint();
        if (width <= 0 || height <= 0 || width > 0xFFFF || height > 0xFFFF)
            throw std::runtime_error("Invalid WZ Canvas dimensions");

        auto const format1 = input_file.read_cint();
        auto const format2 = static_cast<unsigned>(input_file.read<uint8_t>());
        if (input_file.read<uint32_t>() != 0)
            throw std::runtime_error("Invalid WZ Canvas header");

        auto compressed_length = input_file.read<uint32_t>();
        auto const stored_length = compressed_length;
        if (input_file.read<uint8_t>() != 0)
            throw std::runtime_error("Invalid WZ Canvas payload header");

        auto const pixel_count = static_cast<size_t>(width) * static_cast<size_t>(height);
        auto const output_size = pixel_count * 4u;

        size_t source_width = static_cast<size_t>(width);
        size_t source_height = static_cast<size_t>(height);
        if (format2 == 4) {
            if ((width % 16) != 0 || (height % 16) != 0)
                throw std::runtime_error("Invalid WZ Canvas scaled dimensions");
            source_width /= 16;
            source_height /= 16;
        } else if (format2 != 0) {
            throw std::runtime_error("Unsupported WZ Canvas secondary format");
        }

        auto const source_pixels = source_width * source_height;
        size_t expected_decompressed = 0;
        switch (format1) {
        case 1:
        case 257:
        case 513:
            expected_decompressed = source_pixels * 2u;
            break;
        case 2:
            expected_decompressed = source_pixels * 4u;
            break;
        case 1026:
            expected_decompressed = static_cast<size_t>(squish::GetStorageRequirements(
                static_cast<int>(source_width), static_cast<int>(source_height), squish::kDxt3));
            break;
        case 2050:
            expected_decompressed = static_cast<size_t>(squish::GetStorageRequirements(
                static_cast<int>(source_width), static_cast<int>(source_height), squish::kDxt5));
            break;
        default:
            throw std::runtime_error("Unsupported WZ Canvas pixel format");
        }

        auto const buffer_size = std::max({output_size, expected_decompressed,
            static_cast<size_t>(compressed_length)});
        std::vector<uint8_t> input(buffer_size);
        std::vector<uint8_t> output(buffer_size);
        auto const * original = reinterpret_cast<uint8_t const *>(input_file.offset);
        auto const * archive_key = entry.key;
        int decompressed = 0;

        auto inflate_payload = [&](int & result_code, int & written) {
            z_stream stream = {};
            stream.next_in = input.data();
            stream.avail_in = compressed_length;
            if (inflateInit(&stream) != Z_OK)
                return false;
            stream.next_out = output.data();
            // Decode only the pixels declared by the Canvas metadata. Some
            // v085 archives contain a valid image followed by stale/padded
            // deflate output; the renderer must not consume that tail.
            stream.avail_out = static_cast<uInt>(expected_decompressed);
            auto const result = inflate(&stream, Z_FINISH);
            decompressed = static_cast<int>(stream.total_out);
            result_code = result;
            written = decompressed;
            inflateEnd(&stream);
            // WZ Canvas streams commonly fill the destination buffer exactly;
            // zlib reports Z_BUF_ERROR rather than Z_STREAM_END in that case.
            return (result == Z_STREAM_END || result == Z_BUF_ERROR || result == Z_DATA_ERROR) &&
                static_cast<size_t>(decompressed) == expected_decompressed;
        };

        auto decrypt_payload = [&](uint8_t const * key) {
            size_t source = 0;
            size_t target = 0;
            while (source + sizeof(uint32_t) <= compressed_length) {
                auto const block = *reinterpret_cast<uint32_t const *>(original + source);
                source += sizeof(uint32_t);
                // List.wz-protected Canvas payloads may terminate their chunk
                // table with a zero-sized block and leave padding afterwards.
                if (block == 0)
                    break;
                if (source + block > compressed_length || target + block > input.size())
                    return false;
                for (uint32_t i = 0; i < block; ++i)
                    input[target + i] = static_cast<uint8_t>(original[source + i] ^ key[i]);
                source += block;
                target += block;
            }
            compressed_length = static_cast<uint32_t>(target);
            return target != 0;
        };

        auto const has_zlib_header = [](uint8_t const * bytes, size_t length) {
            if (!bytes || length < 2)
                return false;
            auto const cmf = static_cast<unsigned>(bytes[0]);
            auto const flg = static_cast<unsigned>(bytes[1]);
            return (cmf & 0x0Fu) == 8u && (cmf >> 4u) <= 7u &&
                ((cmf << 8u) + flg) % 31u == 0u;
        };

        std::copy(original, original + compressed_length, input.begin());
        int direct_result = 0;
        int direct_written = 0;
        int decrypted_result = 0;
        int decrypted_written = 0;
        bool decoded = false;
        bool const direct_zlib = has_zlib_header(input.data(), compressed_length);
        if (direct_zlib)
            decoded = inflate_payload(direct_result, direct_written);

        // One source sprite in this v085 set is truncated by 256 bytes. Keep
        // every valid decoded pixel and make only the missing tail transparent.
        if (!decoded && direct_zlib && direct_result == Z_BUF_ERROR && direct_written > 0 &&
            static_cast<size_t>(direct_written) < expected_decompressed &&
            expected_decompressed - static_cast<size_t>(direct_written) <= 256u) {
            std::fill(output.begin() + direct_written,
                output.begin() + expected_decompressed, static_cast<uint8_t>(0));
            decompressed = static_cast<int>(expected_decompressed);
            decoded = true;
        }

        bool decrypted = false;
        int decrypted_key = -1;
        if (!decoded) {
            // 密钥治理（D4）：经 KeyResolver 取得候选 key 序列（按 KeyMode 排序），
            // 并复用 per-archive 缓存的首次命中 key，使同一归档的后续解码直接命中（≤1 次试探）。
            std::vector<uint8_t const *> candidate_keys;
            if (const uint8_t * cached = get_keyresolver().cached(m_data->archive_name))
                candidate_keys.push_back(cached);
            for (const uint8_t * k : get_keyresolver().candidates(archive_key))
                candidate_keys.push_back(k);

            const uint8_t * winner_key = nullptr;
            uint32_t attempts = 0;
            for (size_t key_index = 0; key_index < candidate_keys.size() && !decoded; ++key_index) {
                auto const * candidate_key = candidate_keys[key_index];
                if (!candidate_key)
                    continue;
                bool duplicate = false;
                for (size_t earlier = 0; earlier < key_index; ++earlier)
                    if (candidate_keys[earlier] == candidate_key)
                        duplicate = true;
                if (duplicate)
                    continue;

                ++attempts;
                compressed_length = stored_length;
                if (!decrypt_payload(candidate_key))
                    continue;
                decrypted = true;
                decrypted_key = static_cast<int>(key_index);
                if (has_zlib_header(input.data(), compressed_length)) {
                    if (inflate_payload(decrypted_result, decrypted_written)) {
                        winner_key = candidate_key;
                        decoded = true;
                    }
                }
            }
            // 首次成功即缓存命中 key，后续该归档解码直接命中（≤1 次试探）。
            if (winner_key)
                get_keyresolver().cache(m_data->archive_name, winner_key);
            get_keyresolver().note_trial(m_data->archive_name, attempts);
        }

        if (!decoded) {
            std::ostringstream message;
            message << "Unable to inflate WZ Canvas payload"
                << " width=" << width << " height=" << height
                << " format1=" << format1 << " format2=" << format2
                << " expected=" << expected_decompressed
                << " stored=" << stored_length
                << " direct_result=" << direct_result << " direct_written=" << direct_written
                << " decrypted=" << (decrypted ? 1 : 0)
                << " decrypted_key=" << decrypted_key
                << " decrypted_result=" << decrypted_result
                << " decrypted_written=" << decrypted_written << " prefix=";
            auto const prefix_length = std::min<size_t>(8, stored_length);
            for (size_t i = 0; i < prefix_length; ++i)
                message << std::hex << std::setfill('0') << std::setw(2)
                    << static_cast<unsigned>(original[i]);
            throw std::runtime_error(message.str());
        }

        input.swap(output);

        struct color4444 {
            uint8_t b : 4;
            uint8_t g : 4;
            uint8_t r : 4;
            uint8_t a : 4;
        };
        struct color8888 {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        };
        struct color565 {
            uint16_t b : 5;
            uint16_t g : 6;
            uint16_t r : 5;
        };
        static_assert(sizeof(color4444) == 2, "Unexpected 4444 bit packing");
        static_assert(sizeof(color8888) == 4, "Unexpected 8888 packing");
        static_assert(sizeof(color565) == 2, "Unexpected 565 bit packing");

        auto const converted_pixels = source_pixels;
        if (static_cast<size_t>(decompressed) != expected_decompressed) {
            std::ostringstream message;
            message << "WZ Canvas decompressed size mismatch"
                << " width=" << width << " height=" << height
                << " format1=" << format1 << " format2=" << format2
                << " actual=" << decompressed << " expected=" << expected_decompressed;
            throw std::runtime_error(message.str());
        }

        auto const * pixels4444 = reinterpret_cast<color4444 const *>(input.data());
        auto const * pixels565 = reinterpret_cast<color565 const *>(input.data());
        auto * pixelsout = reinterpret_cast<color8888 *>(output.data());
        switch (format1) {
        case 1:
        case 257:
            for (size_t i = 0; i < converted_pixels; ++i) {
                auto const p = pixels4444[i];
                pixelsout[i] = {table4[p.b], table4[p.g], table4[p.r], table4[p.a]};
            }
            input.swap(output);
            break;
        case 2:
            break;
        case 513:
            for (size_t i = 0; i < converted_pixels; ++i) {
                auto const p = pixels565[i];
                pixelsout[i] = {table5[p.b], table6[p.g], table5[p.r], 255};
            }
            input.swap(output);
            break;
        case 1026:
            squish::DecompressImage(output.data(), static_cast<int>(source_width),
                static_cast<int>(source_height), input.data(), squish::kDxt3);
            input.swap(output);
            break;
        case 2050:
            squish::DecompressImage(output.data(), static_cast<int>(source_width),
                static_cast<int>(source_height), input.data(), squish::kDxt5);
            input.swap(output);
            break;
        default:
            break;
        }

        if (format2 == 4) {
            output.resize(output_size);
            scale<16>(input, output, width, height);
            input.swap(output);
        }

        destination.assign(reinterpret_cast<char const *>(input.data()),
            reinterpret_cast<char const *>(input.data() + output_size));
    }

    namespace wz {
        namespace {
            std::vector<std::unique_ptr<wz_file>> files;

            // 可选的装载根目录前缀：非空时所有归档都在该目录下解析，
            // 为空时保持既有行为（相对当前工作目录）。
            std::string base_prefix;

            // 给归档名加上根目录前缀。
            std::string with_base(char const * filename) {
                if (base_prefix.empty())
                    return filename;

                std::string joined = base_prefix;
                auto const last = joined.back();

                if (last != '/' && last != '\\')
                    joined += '/';

                joined += filename;

                return joined;
            }

            node add_file(char const * filename) {
                auto const path = with_base(filename);
                files.emplace_back(std::make_unique<wz_file>(path));
                auto name = std::filesystem::path(path).stem().string();
                archives[name] = files.back().get();
                return files.back()->root();
            }
        }

        void load_all() {
            load_all(std::string());
        }

        void load_all(std::string const & base_dir) {
            if (!files.empty())
                return;

            // 装载期间生效，结束后清空，避免影响后续调用。
            base_prefix = base_dir;

            // Single + split volumes, matching the 28-root NX/WZ layout used by
            // this client. Each archive is mapped onto the corresponding nl::nx root.
            nx::Base = add_file("Base.wz");
            nx::Character = add_file("Character.wz");
            nx::Effect = add_file("Effect.wz");
            nx::Etc = add_file("Etc.wz");
            nx::Item = add_file("Item.wz");
            nx::Map = add_file("Map.wz");
            nx::Map001 = add_file("Map001.wz");
            nx::Map002 = add_file("Map002.wz");
            nx::Map2 = add_file("Map2.wz");
            nx::Mob = add_file("Mob.wz");
            nx::Mob001 = add_file("Mob001.wz");
            nx::Mob002 = add_file("Mob002.wz");
            nx::Mob2 = add_file("Mob2.wz");
            nx::Morph = add_file("Morph.wz");
            nx::Npc = add_file("Npc.wz");
            nx::Quest = add_file("Quest.wz");
            nx::Reactor = add_file("Reactor.wz");
            nx::Skill = add_file("Skill.wz");
            nx::Skill001 = add_file("Skill001.wz");
            nx::Skill002 = add_file("Skill002.wz");
            nx::Skill003 = add_file("Skill003.wz");
            nx::Sound = add_file("Sound.wz");
            nx::Sound001 = add_file("Sound001.wz");
            nx::Sound002 = add_file("Sound002.wz");
            nx::Sound2 = add_file("Sound2.wz");
            nx::String = add_file("String.wz");
            nx::TamingMob = add_file("TamingMob.wz");
            nx::UI = add_file("UI.wz");

            static char const * names[] = {
                "Base", "Character", "Effect", "Etc", "Item", "Map", "Map001", "Map002", "Map2",
                "Mob", "Mob001", "Mob002", "Mob2", "Morph", "Npc", "Quest", "Reactor",
                "Skill", "Skill001", "Skill002", "Skill003", "Sound", "Sound001", "Sound002", "Sound2",
                "String", "TamingMob", "UI"
            };
            for (size_t i = 0; i < files.size(); ++i) {
                std::cout << "Resolving WZ links " << names[i] << "......." << std::flush;
                files[i]->resolve_links();
                std::cout << "Done!" << std::endl;
            }

            base_prefix.clear();
        }
    }
}

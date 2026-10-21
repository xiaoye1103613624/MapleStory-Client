////////////////////////////////////////////////////////////////////////////////
// Direct WZ backend for the nl::node interface.
// Copyright (C) 2026 Fyms085 compatible client contributors.
// Licensed under GNU Affero General Public License v3 or later.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "nxfwd.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nl {
    // 直接 WZ 后端：在内存中从 .wz 归档字节解析 nl::node（不经过 .nx 落盘）。
    // img_file 复用其 _file_data 布局与全部 bitmap/audio 解码方法（见下）。
    class wz_file {
    protected:
        // 完整定义见 WzRuntime.cpp（需要 vendor 的 wztonx 类型，故不能在此内联）。
        struct data;
        std::unique_ptr<data> m_data;
        // 把已解析的 parser 绑定到 view（node_table + wz 回指），并构建
        // parents/redirects/resolve_state 索引。open 与 img_file 的 open_*
        // 共用，避免重复逻辑（非静态：内部用 this 作为 view.wz 回指）。
        void finalize(std::unique_ptr<data> next);
    public:
        wz_file();
        explicit wz_file(std::string const & filename);
        ~wz_file();

        wz_file(wz_file const &) = delete;
        wz_file & operator=(wz_file const &) = delete;

        void open(std::string const & filename);
        node root() const;
        operator node() const;

        std::string const & get_string(uint32_t id) const;
        uint32_t find_child(uint32_t parent, char const * name, uint16_t length) const;
        void const * get_audio(uint32_t id) const;
        void decode_bitmap(uint32_t id, std::vector<char> & output);
        void resolve_links();
        bool resolve_bitmap(uint32_t node_id, wz_file * & owner,
            uint32_t & bitmap_id, uint16_t & width, uint16_t & height);

    private:
        void close();
        bool resolve_link(uint32_t node_id);
        uint32_t resolve_path(uint32_t start, std::vector<std::string> const & parts,
            size_t first = 0) const;
    };

    // 直接 IMG 后端：把经典独立 .img 文件（含 img-node 字节流）在内存中解析为
    // nl::node，复用了 wz_file 的 _file_data 布局与全部 bitmap/audio 解码方法
    // （img 本质是一个 img-node 流，与 WZ 的 Canvas/Audio 编码同源）。与旧
    // ImgFiles 实现不同，这里不落盘 .nxcache、不写中间 .nx 文件（零转换）。
    class img_file : public wz_file {
    public:
        img_file() = default;

        // 合并若干 .img 字节（带相对路径）为一个根节点。
        // 用于 Base/Character/.../UI 等"目录下所有 .img 成为根子节点"的普通根。
        void open_merge(std::vector<std::pair<std::string, std::vector<char>>> && bufs);

        // 单个 .img 字节作为根节点（用于顶层散落 .img，如 smap.img）。
        void open_single(std::vector<char> && buf);

        // 目录树（相对路径 + 字节）作为一个根节点，保持嵌套结构。
        // 用于 Map002 的 Map/Map<prefix>/<id>.img 三层组织。
        void open_tree(std::vector<std::pair<std::string, std::vector<char>>> && bufs);
    };

    namespace wz {
        // Loads Base.wz plus the other fifteen standard v085 WZ archives and
        // assigns their roots to nl::nx::*.
        void load_all();

        // Same as load_all(), but every archive is resolved under @base_dir
        // instead of the current working directory. An empty base_dir behaves
        // exactly like load_all().
        void load_all(std::string const & base_dir);
    }

    // Converts a list of standalone .img files into a single <root_name>.nx file
    // (one root node whose children are the individual .img subtrees). Used by
    // the IMG data backend. Each .img is a classic container: a 13-byte magic
    // followed by a standard WZ img-node stream.
    void convert_img_files_to_nx(const std::vector<std::string>& img_paths, const std::string& root_name, const std::string& base_dir);
    // Variant that maps each .img to an explicit relative path (used for Map002,
    // where maps are reorganised as Map/Map<prefix>/<id>.img).
    void convert_img_files_to_nx(const std::vector<std::pair<std::string, std::string>>& relpaths_and_paths, const std::string& root_name);

    // Converts a single standalone .img file into its own <root_name>.nx file,
    // where the .img content becomes the NX file root directly (no wrapping
    // "x.img" child). Used for WZ archives that are individual nl::nx roots,
    // e.g. Map001.img -> nl::nx::Map001.
    void convert_single_img_to_nx(const std::string& img_path, const std::string& root_name);

    // 密钥诊断（D4）：返回每归档的试探次数，用于验证"每归档 ≤1 次试探"。
    std::string key_diagnostics();
}

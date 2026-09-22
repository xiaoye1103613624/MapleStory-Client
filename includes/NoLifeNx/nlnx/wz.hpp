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
    class wz_file {
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
        struct data;
        std::unique_ptr<data> m_data;
        bool resolve_link(uint32_t node_id);
        uint32_t resolve_path(uint32_t start, std::vector<std::string> const & parts,
            size_t first = 0) const;
    };

    namespace wz {
        // Loads Base.wz plus the other fifteen standard v085 WZ archives and
        // assigns their roots to nl::nx::*.
        void load_all();
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
}

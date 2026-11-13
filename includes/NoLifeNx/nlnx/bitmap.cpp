//////////////////////////////////////////////////////////////////////////////
// NoLifeNx - Part of the NoLifeStory project                               //
// Copyright © 2013 Peter Atashian                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////

#include "bitmap.hpp"
#include "wz.hpp"
#include <lz4.h>
#include <climits>
#include <cstring>
#include <vector>

namespace nl {
    bitmap::bitmap(void const * d, uint16_t w, uint16_t h) :
        m_data(d), m_width(w), m_height(h) {}
    bitmap::bitmap(wz_file * f, uint32_t i, uint16_t w, uint16_t h) :
        m_wz(f), m_wz_index(i), m_width(w), m_height(h) {}
    bool bitmap::operator<(bitmap const & o) const {
        if (m_wz != o.m_wz)
            return m_wz < o.m_wz;
        return m_wz ? m_wz_index < o.m_wz_index : m_data < o.m_data;
    }
    bool bitmap::operator==(bitmap const & o) const {
        return m_data == o.m_data && m_wz == o.m_wz && m_wz_index == o.m_wz_index;
    }
    bitmap::operator bool() const {
        return m_data || m_wz;
    }
    std::vector<char> bitmap_buf;
    void const * bitmap::data() const {
        if (m_wz) {
            m_wz->decode_bitmap(m_wz_index, bitmap_buf);
            return bitmap_buf.empty() ? nullptr : bitmap_buf.data();
        }
        if (!m_data)
            return nullptr;

        auto const l = length();
        if (l == 0 || l > static_cast<uint32_t>(INT_MAX))
            return nullptr;

        if (l + 0x20 > bitmap_buf.size())
            bitmap_buf.resize(l + 0x20);

        // .nx layout is [uint32 compressed length][LZ4 block].  Read the
        // length prefix and use the bounded LZ4_decompress_safe so a truncated
        // / corrupt cache (e.g. an interrupted conversion) can never read past
        // the mapped file end.  LZ4_decompress_fast took no source size, which
        // caused an access violation inside liblz4.dll on such data.
        uint32_t compressed = 0;
        std::memcpy(&compressed, m_data, sizeof(compressed));

        uint64_t bound = static_cast<uint64_t>(l) + l / 255u + 16u;
        if (compressed == 0 || compressed > bound
            || compressed > static_cast<uint32_t>(INT_MAX))
        {
            std::memset(bitmap_buf.data(), 0, l);
            return bitmap_buf.data();
        }

        int const written = ::LZ4_decompress_safe(
            4 + reinterpret_cast<char const *>(m_data),
            bitmap_buf.data(),
            static_cast<int>(compressed),
            static_cast<int>(l));

        if (written != static_cast<int>(l))
            std::memset(bitmap_buf.data(), 0, l);

        return bitmap_buf.data();
    }
    uint16_t bitmap::width() const {
        return m_width;
    }
    uint16_t bitmap::height() const {
        return m_height;
    }
    uint32_t bitmap::length() const {
        return 4u * m_width * m_height;
    }
    size_t bitmap::id() const {
        if (m_wz)
            return reinterpret_cast<size_t>(m_wz) ^ (static_cast<size_t>(m_wz_index) << 1);
        return reinterpret_cast<size_t>(m_data);
    }
}

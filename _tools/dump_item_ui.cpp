// Dump MapleStory UIWindow.img/Item canvases to PNG via sdlMS wzlib.
#include "wz/Files.h"
#include "wz/Key.h"
#include "wz/Wz.h"
#include "wz/Node.h"
#include "wz/Property.h"
#include "wz/Types.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static std::u16string a2u(const char *s) {
  std::u16string out;
  for (const char *p = s; *p; ++p)
    out.push_back(static_cast<char16_t>(static_cast<unsigned char>(*p)));
  return out;
}

static bool write_png_rgba(const fs::path &path, int w, int h, const std::vector<uint8_t> &rgba) {
  // Minimal uncompressed PNG via raw BMP fallback (Pillow-friendly), then convert externally if needed.
  // Write 32-bit BGRA BMP which Python/Pillow and Photoshop both open.
  const fs::path bmp = path.extension() == ".png" ? path.replace_extension(".bmp") : path;
  const uint32_t row_stride = static_cast<uint32_t>(w * 4);
  const uint32_t pixel_bytes = row_stride * static_cast<uint32_t>(h);
  const uint32_t file_size = 54 + pixel_bytes;

  std::vector<uint8_t> hdr(54, 0);
  hdr[0] = 'B'; hdr[1] = 'M';
  *reinterpret_cast<uint32_t *>(&hdr[2]) = file_size;
  *reinterpret_cast<uint32_t *>(&hdr[10]) = 54;
  *reinterpret_cast<uint32_t *>(&hdr[14]) = 40;
  *reinterpret_cast<int32_t *>(&hdr[18]) = w;
  *reinterpret_cast<int32_t *>(&hdr[22]) = h; // bottom-up
  *reinterpret_cast<uint16_t *>(&hdr[26]) = 1;
  *reinterpret_cast<uint16_t *>(&hdr[28]) = 32;
  *reinterpret_cast<uint32_t *>(&hdr[34]) = pixel_bytes;

  std::ofstream out(bmp, std::ios::binary);
  if (!out)
    return false;
  out.write(reinterpret_cast<const char *>(hdr.data()), 54);

  // rgba is top-down RGBA; BMP wants bottom-up BGRA
  std::vector<uint8_t> row(row_stride);
  for (int y = h - 1; y >= 0; --y) {
    const uint8_t *src = rgba.data() + static_cast<size_t>(y) * row_stride;
    for (int x = 0; x < w; ++x) {
      row[x * 4 + 0] = src[x * 4 + 2]; // B
      row[x * 4 + 1] = src[x * 4 + 1]; // G
      row[x * 4 + 2] = src[x * 4 + 0]; // R
      row[x * 4 + 3] = src[x * 4 + 3]; // A
    }
    out.write(reinterpret_cast<const char *>(row.data()), row_stride);
  }
  return true;
}

static std::vector<uint8_t> canvas_to_rgba(wz::Property<wz::WzCanvas> *canvas) {
  auto raw = canvas->get_raw_data();
  const auto c = canvas->get();
  const int w = c.width;
  const int h = c.height;
  const int format = c.format + c.format2;
  std::vector<uint8_t> rgba(static_cast<size_t>(w) * h * 4, 0);

  if (format == 1) {
    // ARGB4444
    for (int i = 0; i < w * h; ++i) {
      uint16_t px = static_cast<uint16_t>(raw[i * 2] | (raw[i * 2 + 1] << 8));
      uint8_t a = static_cast<uint8_t>(((px >> 12) & 0xF) * 17);
      uint8_t r = static_cast<uint8_t>(((px >> 8) & 0xF) * 17);
      uint8_t g = static_cast<uint8_t>(((px >> 4) & 0xF) * 17);
      uint8_t b = static_cast<uint8_t>((px & 0xF) * 17);
      rgba[i * 4 + 0] = r;
      rgba[i * 4 + 1] = g;
      rgba[i * 4 + 2] = b;
      rgba[i * 4 + 3] = a;
    }
  } else if (format == 2) {
    // ARGB8888 (byte order typically B,G,R,A in Maple raw — match sdlMS ARGB8888 texture)
    for (int i = 0; i < w * h; ++i) {
      // MapleStory format 2 is typically BGRA in memory for SDL_PIXELFORMAT_ARGB8888 on little-endian
      uint8_t b = raw[i * 4 + 0];
      uint8_t g = raw[i * 4 + 1];
      uint8_t r = raw[i * 4 + 2];
      uint8_t a = raw[i * 4 + 3];
      rgba[i * 4 + 0] = r;
      rgba[i * 4 + 1] = g;
      rgba[i * 4 + 2] = b;
      rgba[i * 4 + 3] = a;
    }
  }
  return rgba;
}

static bool dump_canvas(wz::Node *node, const fs::path &out_path) {
  if (!node || node->type != wz::Type::Canvas)
    return false;
  auto *canvas = static_cast<wz::Property<wz::WzCanvas> *>(node);
  auto rgba = canvas_to_rgba(canvas);
  auto c = canvas->get();
  printf("dump %s %dx%d format=%d -> %s\n", out_path.string().c_str(), c.width, c.height,
         c.format + c.format2, out_path.string().c_str());
  return write_png_rgba(out_path, c.width, c.height, rgba);
}

static wz::Node *find_path(wz::Node *root, const char *path) {
  if (!root)
    return nullptr;
  return root->try_find(a2u(path));
}

int main(int argc, char **argv) {
  std::vector<uint8_t> key = {0x13, 0, 0, 0, 0x08, 0, 0, 0, 0x06, 0, 0, 0, 0xB4, 0, 0, 0,
                              0x1B, 0, 0, 0, 0x0F, 0, 0, 0, 0x33, 0, 0, 0, 0x52, 0, 0, 0};
  std::array<uint8_t, 4> iv{0x4d, 0x23, 0xc7, 0x2b};
  wz::init_key(iv, key);

  const char *wz_path = argc > 1 ? argv[1] : R"(E:\project\sdlMS\build\Data2\UI.wz)";
  const char *out_dir = argc > 2 ? argv[2]
                                 : R"(E:\project\MapleStory-Client\docs\features\inventory-ui-ps\source)";

  fs::create_directories(out_dir);

  auto *f = wz::Files::from_classic_wz(wz_path);
  if (!f) {
    printf("load failed: %s\n", wz_path);
    return 1;
  }

  struct Item {
    const char *rel;
    const char *file;
  };
  const Item items[] = {
      {"UIWindow.img/Item/FullBackgrnd", "FullBackgrnd"},
      {"UIWindow.img/Item/backgrnd", "backgrnd"},
      {"UIWindow.img/Item/disabled", "disabled"},
      {"UIWindow.img/Item/activeIcon", "activeIcon"},
      {"UIWindow.img/Item/Tab/enabled/0", "Tab_enabled_0"},
      {"UIWindow.img/Item/Tab/enabled/1", "Tab_enabled_1"},
      {"UIWindow.img/Item/Tab/enabled/2", "Tab_enabled_2"},
      {"UIWindow.img/Item/Tab/enabled/3", "Tab_enabled_3"},
      {"UIWindow.img/Item/Tab/enabled/4", "Tab_enabled_4"},
      {"UIWindow.img/Item/Tab/disabled/0", "Tab_disabled_0"},
      {"UIWindow.img/Item/Tab/disabled/1", "Tab_disabled_1"},
      {"UIWindow.img/Item/Tab/disabled/2", "Tab_disabled_2"},
      {"UIWindow.img/Item/Tab/disabled/3", "Tab_disabled_3"},
      {"UIWindow.img/Item/Tab/disabled/4", "Tab_disabled_4"},
      {"UIWindow.img/Item/BtCoin/normal/0", "BtCoin"},
      {"UIWindow.img/Item/BtGather/normal/0", "BtGather"},
      {"UIWindow.img/Item/BtSort/normal/0", "BtSort"},
      {"UIWindow.img/Item/BtSmall/normal/0", "BtSmall"},
      {"UIWindow.img/Item/BtFull/normal/0", "BtFull"},
      {"UIWindow.img/Item/BtCashshop/normal/0", "BtCashshop"},
      {"UIWindow.img/Basic.img/BtClose3/normal/0", "BtClose3"},
      {"Basic.img/BtClose3/normal/0", "BtClose3_basic"},
      {"Basic.img/BtClose2/normal/0", "BtClose2"},
  };

  int ok = 0;
  for (auto &it : items) {
    auto *n = find_path(f, it.rel);
    if (!n) {
      printf("missing: %s\n", it.rel);
      continue;
    }
    fs::path out = fs::path(out_dir) / (std::string(it.file) + ".bmp");
    if (dump_canvas(n, out))
      ++ok;
    else
      printf("dump failed: %s\n", it.rel);
  }

  printf("done, exported %d canvases to %s\n", ok, out_dir);
  return ok > 0 ? 0 : 2;
}

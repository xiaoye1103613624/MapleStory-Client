#include <nlnx/wz.hpp>
#include <nlnx/nx.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

int main(int argc, char** argv) {
    std::string base = argc > 1 ? argv[1] : R"(E:\MXD\BeiDou-Client-xy\Data)";
    std::string uidir = base + "/UI";
    std::ifstream f(uidir + "/Basic.img", std::ios::binary);
    if (!f) { std::cerr << "no Basic.img\n"; return 1; }
    std::vector<char> buf((std::istreambuf_iterator<char>(f)), {});
    std::vector<std::pair<std::string, std::vector<char>>> merge;
    merge.emplace_back("Basic.img", std::move(buf));
    {
      std::ifstream lf(uidir + "/Login.img", std::ios::binary);
      if (lf) {
        std::vector<char> lb((std::istreambuf_iterator<char>(lf)), {});
        merge.emplace_back("Login.img", std::move(lb));
      }
    }
    nl::img_file img;
    img.open_merge(std::move(merge));
    nl::node root = img.root();
    auto dump = [](nl::node n, int depth, int maxd) {
      if (depth > maxd) return;
      for (auto c : n) {
        std::string pad(depth * 2, ' ');
        std::cout << pad << c.name() << " type=" << (int)c.data_type();
        if (c.data_type() == nl::node::type::bitmap)
          std::cout << " " << c.width() << "x" << c.height();
        std::cout << "\n";
        if (depth < maxd) {
          // recurse manually below
        }
      }
    };
    std::cout << "=== Basic.img/Cursor (depth2) ===\n";
    nl::node cur = root["Basic.img"]["Cursor"];
    std::cout << "Cursor valid=" << (bool)cur << " children=" << cur.size() << "\n";
    for (auto c : cur) {
      std::cout << "  " << c.name() << " type=" << (int)c.data_type() << " size=" << c.size();
      if (c.data_type() == nl::node::type::bitmap)
        std::cout << " " << c.width() << "x" << c.height();
      std::cout << "\n";
      int n = 0;
      for (auto f2 : c) {
        if (n++ > 8) { std::cout << "    ...\n"; break; }
        std::cout << "    " << f2.name() << " type=" << (int)f2.data_type();
        if (f2.data_type() == nl::node::type::bitmap)
          std::cout << " " << f2.width() << "x" << f2.height();
        auto ori = f2["origin"];
        if (ori) std::cout << " origin";
        std::cout << "\n";
      }
    }
    std::cout << "=== Basic.img/ItemNo ===\n";
    nl::node ino = root["Basic.img"]["ItemNo"];
    std::cout << "ItemNo valid=" << (bool)ino << "\n";
    for (auto c : ino) {
      std::cout << "  " << c.name();
      if (c.data_type() == nl::node::type::bitmap)
        std::cout << " " << c.width() << "x" << c.height();
      std::cout << "\n";
    }
    std::cout << "=== Login Notice Loading ===\n";
    nl::node load = root["Login.img"]["Notice"]["Loading"];
    std::cout << "Loading valid=" << (bool)load << "\n";
    for (auto c : load) {
      std::cout << "  " << c.name() << " type=" << (int)c.data_type();
      if (c.data_type() == nl::node::type::bitmap)
        std::cout << " " << c.width() << "x" << c.height();
      auto ori = c["origin"];
      std::cout << "\n";
      if (c.name() == std::string("backgrnd") || c.name() == std::string("bar") || c.name() == std::string("BtCancel")) {
        for (auto s : c) {
          std::cout << "    " << s.name() << " type=" << (int)s.data_type();
          if (s.data_type() == nl::node::type::bitmap)
            std::cout << " " << s.width() << "x" << s.height();
          std::cout << "\n";
          for (auto s2 : s) {
            std::cout << "      " << s2.name() << " type=" << (int)s2.data_type();
            if (s2.data_type() == nl::node::type::bitmap)
              std::cout << " " << s2.width() << "x" << s2.height();
            std::cout << "\n";
          }
        }
      }
    }
    return 0;
}

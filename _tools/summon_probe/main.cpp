// 波3 取证探针：Skill.img summon 节点语义（动画 vs mob id 引用）
// 镜像客户端 ImgSource 的装载路径：扫 Data/Skill/*.img 直接 open_merge。
// 只读，不做任何写操作。
#include <memory>
#include <nlnx/nxfwd.hpp>
#include <nlnx/node.hpp>
#include <nlnx/bitmap.hpp>
#include <nlnx/wz.hpp>
#include <nlnx/nx.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <vector>
#include <utility>
#include <string>
#include <cctype>

static void dump_node(nl::node n, int depth, int maxd)
{
	if (depth > maxd)
		return;

	for (auto c : n)
	{
		std::cout << std::string(depth * 2, ' ') << c.name()
			<< " type=" << (int)c.data_type();

		if (c.data_type() == nl::node::type::bitmap)
		{
			nl::bitmap b = c;
			std::cout << " " << b.width() << "x" << b.height();
		}
		else if (c.data_type() == nl::node::type::real)
			std::cout << " val=" << (int32_t)c;
		else if (c.data_type() == nl::node::type::string)
			std::cout << " str=" << c.get_string();

		std::cout << "\n";
		dump_node(c, depth + 1, maxd);
	}
}

static std::vector<std::pair<std::string, std::vector<char>>> gather(const std::string& dir)
{
	std::vector<std::pair<std::string, std::vector<char>>> out;
	std::error_code ec;

	for (auto it = std::filesystem::recursive_directory_iterator(dir,
		std::filesystem::directory_options::skip_permission_denied, ec);
		it != std::filesystem::recursive_directory_iterator(); ++it)
	{
		std::error_code lec;
		if (!it->is_regular_file(lec) || lec)
			continue;

		std::string ext = it->path().extension().string();
		for (auto& ch : ext)
			ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

		if (ext != ".img")
			continue;

		std::ifstream f(it->path(), std::ios::binary);
		if (!f)
			continue;

		std::vector<char> all((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());
		if (all.empty())
			continue;

		std::string rel = it->path().lexically_relative(dir).generic_string();
		out.emplace_back(std::move(rel), std::move(all));
	}
	return out;
}

int main(int argc, char** argv)
{
	std::string data = argc > 1 ? argv[1] : "E:/MXD/BeiDou-Client-xy/Data";

	std::cout << "scanning " << data << "/Skill ...\n";
	auto bufs = gather(data + "/Skill");
	std::cout << "found " << bufs.size() << " .img files\n";
	if (bufs.empty())
		return 1;

	auto f = std::make_unique<nl::img_file>();
	f->open_merge(std::move(bufs));
	nl::node root = f->root();
	std::cout << "Skill root children: " << root.size() << "\n";

	const int probe[] = { 3111002, 3211002, 2321003, 3221005, 2121005, 1321007, 5211001, 2311009 };

	for (int sid : probe)
	{
		std::string ns = std::to_string(sid / 10000);
		while (ns.size() < 3)
			ns.insert(ns.begin(), '0');

		nl::node sk = root[ns + ".img"]["skill"][std::to_string(sid)];

		if (!sk)
		{
			std::cout << "=== " << sid << " NOT FOUND in " << ns << ".img\n";
			continue;
		}

		std::cout << "=== " << sid << " (" << ns << ".img) children: ";
		for (auto c : sk)
			std::cout << c.name() << " ";
		std::cout << "\n";

		bool printed = false;

		for (auto key : { "summon", "summoned", "mob" })
		{
			nl::node sub = sk[key];

			if (sub && sub.size() > 0)
			{
				std::cout << "  >> " << key << " size=" << sub.size() << "\n";
				dump_node(sub, 2, 3);
				printed = true;
				break;
			}
		}

		if (!printed)
			std::cout << "  (no summon/summoned/mob node)\n";
	}

	std::cout << "\n=== Sound/Reactor.img probe ===\n";
	auto sbufs = gather(data + "/Sound");
	if (!sbufs.empty())
	{
		auto sf = std::make_unique<nl::img_file>();
		sf->open_merge(std::move(sbufs));
		nl::node react = sf->root()["Reactor.img"];
		int n = 0;
		for (auto c : react)
		{
			std::cout << "rid " << c.name() << " children: ";
			for (auto k : c)
				std::cout << k.name() << " ";
			std::cout << "\n";
			if (++n >= 3)
				break;
		}
	}

	std::cout << "\n=== MapHelper.img/portal ===\n";
	{
		auto mbufs = gather(data + "/Map");
		if (!mbufs.empty())
		{
			auto mf = std::make_unique<nl::img_file>();
			mf->open_merge(std::move(mbufs));
			nl::node helper = mf->root()["MapHelper.img"]["portal"];

			if (helper)
			{
				for (auto g : helper)
				{
					std::cout << "group " << g.name() << " children: ";
					for (auto c : g)
						std::cout << c.name() << "(" << (int)c.data_type() << ") ";
					std::cout << "\n";
				}
			}
			else
				std::cout << "portal node MISSING\n";

			// mist textures
			nl::node me = mf->root()["MapHelper.img"]["mark"];
			if (me)
			{
				std::cout << "mark children (first 30): ";
				int n = 0;
				for (auto c : me) { std::cout << c.name() << " "; if (++n >= 30) break; }
				std::cout << "\n";
			}
		}
	}

	std::cout << "\n=== Effect/BasicEff mist/door keys ===\n";
	{
		auto ebufs = gather(data + "/Effect");
		if (!ebufs.empty())
		{
			auto ef = std::make_unique<nl::img_file>();
			ef->open_merge(std::move(ebufs));
			nl::node be = ef->root()["BasicEff.img"];

			for (auto c : be)
			{
				std::string nm = c.name();
				if (nm.find("ist") != std::string::npos || nm.find("oor") != std::string::npos
					|| nm.find("Mob") != std::string::npos || nm.find("spec") != std::string::npos)
				{
					std::cout << "BasicEff/" << nm << " children: ";
					int n = 0;
					for (auto k : c) { std::cout << k.name() << " "; if (++n >= 12) break; }
					std::cout << "\n";
				}
			}
		}
	}

	std::cout << "\n=== Sound/Reactor.img node detail ===\n";
	{
		auto sbufs2 = gather(data + "/Sound");
		if (!sbufs2.empty())
		{
			auto sf2 = std::make_unique<nl::img_file>();
			sf2->open_merge(std::move(sbufs2));
			nl::node r2 = sf2->root()["Reactor.img"];
			int n2 = 0;
			for (auto c : r2)
			{
				std::cout << "rid " << c.name() << ":\n";
				for (auto k : c)
					std::cout << "  " << k.name() << " type=" << (int)k.data_type()
						<< " size=" << k.size() << "\n";
				if (++n2 >= 2)
					break;
			}
		}
	}

	std::cout << "\n=== Reactor rid/0 inner + all BasicEff keys ===\n";
	{
		auto sbufs3 = gather(data + "/Sound");
		if (!sbufs3.empty())
		{
			auto sf3 = std::make_unique<nl::img_file>();
			sf3->open_merge(std::move(sbufs3));
			nl::node r3 = sf3->root()["Reactor.img"]["1020000"];
			for (auto k : r3)
			{
				std::cout << "state " << k.name() << " type=" << (int)k.data_type()
					<< " size=" << k.size() << " children: ";
				for (auto g : k)
					std::cout << g.name() << "(t" << (int)g.data_type() << ") ";
				std::cout << "\n";
			}
		}
		auto ebufs2 = gather(data + "/Effect");
		if (!ebufs2.empty())
		{
			auto ef2 = std::make_unique<nl::img_file>();
			ef2->open_merge(std::move(ebufs2));
			nl::node be2 = ef2->root()["BasicEff.img"];
			std::cout << "BasicEff keys: ";
			for (auto c : be2)
				std::cout << c.name() << " ";
			std::cout << "\n";
		}
	}

	std::cout << "\n=== 3111002 summon/info deep ===\n";
	{
		auto bufs2 = gather(data + "/Skill");
		auto f2 = std::make_unique<nl::img_file>();
		f2->open_merge(std::move(bufs2));
		nl::node sk2 = f2->root()["311.img"]["skill"]["3111002"]["summon"];
		nl::node info = sk2["info"];
		std::cout << "info children: ";
		for (auto c : info) {
			std::cout << c.name() << "=";
			if (c.data_type() == nl::node::type::string) std::cout << c.get_string() << " ";
			else { std::cout << (int32_t)c << " "; }
		}
		std::cout << "\n";
		// stand/die origins for anchoring
		for (auto key : {"stand","hit","die","summoned"}) {
			nl::node st = sk2[key];
			for (auto fr : st) {
				std::cout << key << "/" << fr.name() << " origin="
					<< fr["origin"].get_vector().first << "," << fr["origin"].get_vector().second
					<< " delay=" << (int32_t)fr["delay"] << "\n";
				break;
			}
		}
	}

	std::cout << "\n=== portal/game deep (ph/psh/pv) ===\n";
	{
		auto mb = gather(data + "/Map");
		auto mf = std::make_unique<nl::img_file>();
		mf->open_merge(std::move(mb));
		nl::node pg = mf->root()["MapHelper.img"]["portal"]["game"];
		for (auto g : pg)
		{
			std::cout << "anim " << g.name() << " type=" << (int)g.data_type() << ":\n";
			for (auto c : g)
			{
				std::cout << "  " << c.name() << " type=" << (int)c.data_type() << " size=" << c.size() << "\n";
				int n = 0;
				for (auto k : c)
				{
					std::cout << "    " << k.name() << " t" << (int)k.data_type();
					if (k.data_type() == nl::node::type::bitmap) { nl::bitmap b = k; std::cout << " " << b.width() << "x" << b.height(); }
					std::cout << "\n";
					if (++n >= 10) { std::cout << "    ...\n"; break; }
				}
			}
		}
	}

	std::cout << "\n=== real map portal node keys ===\n";
	{
		auto mb = gather(data + "/Map");
		auto mf = std::make_unique<nl::img_file>();
		mf->open_merge(std::move(mb));
		const char* maps[] = {"Map/Map1/100000000.img", "Map/Map0/100000000.img",
		                      "Map/Map2/990000000.img", "Map/MapS_i/100000000.img"};
		for (const char* path : maps)
		{
			nl::node m;
			std::string pth(path);
			size_t pos = 0;
			m = mf->root();
			while (true)
			{
				size_t slash = pth.find('/');
				std::string seg = slash == std::string::npos ? pth : pth.substr(0, slash);
				m = m[seg];
				if (slash == std::string::npos) break;
				pth = pth.substr(slash + 1);
			}
			if (!m) { std::cout << path << " MISSING\n"; continue; }
			nl::node ports = m["portal"];
			if (!ports) { std::cout << path << " no portal\n"; continue; }
			int cnt = 0;
			for (auto pn : ports)
			{
				std::cout << path << " portal " << pn.name() << " keys: ";
				for (auto k : pn)
				{
					std::cout << k.name() << "=";
					if (k.data_type() == nl::node::type::string) std::cout << "\"" << k.get_string() << "\" ";
					else std::cout << (int32_t)k << " ";
				}
				std::cout << "\n";
				if (++cnt >= 3) break;
			}
			if (cnt) break;
		}
	}

	return 0;
}

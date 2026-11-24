//////////////////////////////////////////////////////////////////////////////////
// TintContext.cpp — draw-time per-character tint routing
//////////////////////////////////////////////////////////////////////////////////
#include "TintContext.h"
#include "../Graphics/Texture.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	thread_local std::vector<int32_t> TintContext::stack;

	TintContext& TintContext::get()
	{
		static TintContext inst;
		return inst;
	}

	void TintContext::register_bitmap(size_t bitmap_id, int32_t tint_key)
	{
		std::lock_guard<std::mutex> lock(mutex);
		bitmap_to_key[bitmap_id] = tint_key;
	}

#ifdef USE_NX
	void TintContext::register_tree(nl::node root, int32_t tint_key)
	{
		if (!root)
			return;
		// A node that carries bitmap data: its Texture's bitmap id is the draw-time lookup key.
		Texture tex(root);
		if (tex.is_valid())
			register_bitmap(tex.get_bitmap_id(), tint_key);
		for (nl::node child : root)
			register_tree(child, tint_key);
	}
#endif

	void TintContext::set_char_tints(int32_t char_id,
		const std::unordered_map<int32_t, TintCache::HSL>& tints)
	{
		std::lock_guard<std::mutex> lock(mutex);
		char_tints[char_id] = tints;
	}

	void TintContext::clear_char(int32_t char_id)
	{
		std::lock_guard<std::mutex> lock(mutex);
		char_tints.erase(char_id);
	}

	TintContext::Scope::Scope(int32_t char_id) : prev(0), had(false)
	{
		if (!stack.empty()) { prev = stack.back(); had = true; }
		stack.push_back(char_id);
	}

	TintContext::Scope::~Scope()
	{
		if (!stack.empty()) stack.pop_back();
		(void)prev; (void)had; // nested scope restored by pop_back order
	}

	bool TintContext::resolve_hsl(size_t bitmap_id, TintCache::HSL& out) const
	{
		if (stack.empty() || bitmap_id == 0)
			return false;
		const int32_t char_id = stack.back();

		std::lock_guard<std::mutex> lock(mutex);
		auto b = bitmap_to_key.find(bitmap_id);
		if (b == bitmap_to_key.end())
			return false;
		auto c = char_tints.find(char_id);
		if (c == char_tints.end())
			return false;
		auto t = c->second.find(b->second);
		if (t == c->second.end() || TintCache::is_identity(t->second))
			return false;
		out = t->second;
		return true;
	}
}

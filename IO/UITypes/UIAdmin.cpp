//////////////////////////////////////////////////////////////////////////////////
//	Admin — SoftKey announce
//////////////////////////////////////////////////////////////////////////////////
#include "UIAdmin.h"
#include "UIMiscShells.h"
#include "UISoftKey.h"
#include "UIGMCommandList.h"
#include "GMCommandList.h"

#include "../UI.h"
#include "../../Configuration.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/SummonDragonPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"

#include <sstream>
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		constexpr int16_t GM_QUICK_X = 16;
		constexpr int16_t GM_QUICK_Y = 56;
		constexpr int16_t GM_QUICK_H = 14;
		constexpr int16_t GM_QUICK_W = 180;
	}

	UIAdmin::UIAdmin()
		: msg_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("admin.title"))
	{
		position = Point<int16_t>(300, 200);
		dimension = Point<int16_t>(209, 156);
		dragarea = Point<int16_t>(209, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Admin");
		if (!root) {
			LOG(LOG_WARN, "UIAdmin: missing");
			return;
		}
		has_assets = true;
		nl::node block = root["Block"];
		if (block["backgrnd"]) {
			backgrnd = Texture(block["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		center_on_screen();

		// GM 命令快捷（无参数常用命令），点击派发。硬编码坐标兜底，遵守 R5/R6。
		// buffme 必须排在前：Block/backgrnd 仅 209×156，GM_QUICK_Y=56、步长 14
		// → 最多约 7 行落在命中矩形内；旧序把 buffme 放末行 y=168 超出 dimension，
		// SoftKey/聊天注入又吃不进，导致 TemporaryStatView 验收点不到。
		const char* quick[] = { "buffme", "help", "online", "whereami", "rates", "uptime", "gm" };
		int16_t y = GM_QUICK_Y;
		for (const char* c : quick)
		{
			// formatted=false：默认 true 时 '!' 会进 Maple 格式解析，蓝字整行被吃掉（真机 GM TOOL 无 !buffme 可点）。
			gm_quick.push_back({
				Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLUE, std::string("!") + c, 0, false),
				Point<int16_t>(GM_QUICK_X, y),
				c
			});
			y += GM_QUICK_H;
		}

		// 不开构造期 SoftKey：Login SoftKey 盖住 Block 快捷行，!buffme 点不到。
		// 自定义输入仍走 SoftKey A/S（send_key）。
		LOG(LOG_DEBUG, "UIAdmin: open");
#endif
	}

	void UIAdmin::set_message(const std::string& msg)
	{
		msg_label.change_text(msg);
	}

	void UIAdmin::draw(float) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		msg_label.draw(position + Point<int16_t>(16, 40));
		for (const auto& e : gm_quick)
			e.text.draw(position + e.pos);
	}

	void UIAdmin::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'A' || keycode == 'a' || keycode == 'S' || keycode == 's') {
			// Text 模式：命令可键盘输入且可短于 6（Pic SoftKey 会拦键盘 / 拒短串）。
			
		}
		else if (keycode == 'U' || keycode == 'u')
		{
			AdminEnterMapPacket(AdminEnterMapPacket::Operation::SHOW_USERS).dispatch();
			msg_label.change_text(Locale::text("admin.show_users"));
		}
		else if (keycode == 'L' || keycode == 'l')
		{
			AdminEnterMapPacket(AdminEnterMapPacket::Operation::ALERT_ADMINS).dispatch();
			msg_label.change_text(Locale::text("admin.alert_admins"));
		}
		else if (keycode == 'G' || keycode == 'g')
		{
			auto ui = UI::get().get_element<UIGMBoard>();
			if (!ui) ui = UI::get().emplace<UIGMBoard>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			auto ui = UI::get().get_element<UIGMCommandList>();
			if (!ui) ui = UI::get().emplace<UIGMCommandList>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UIAdmin::get_type() const { return TYPE; }

	Cursor::State UIAdmin::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked && has_assets)
		{
			Point<int16_t> local(cursorpos.x() - position.x(), cursorpos.y() - position.y());
			for (const auto& e : gm_quick)
			{
				if (local.x() >= e.pos.x() && local.x() <= e.pos.x() + GM_QUICK_W &&
					local.y() >= e.pos.y() && local.y() <= e.pos.y() + GM_QUICK_H)
				{
					std::string cmd = "!" + e.cmd;
					GeneralChatPacket(cmd, true).dispatch();
					if (Configuration::get().get_admin())
						msg_label.change_text(">> " + cmd);
					return Cursor::State::IDLE;
				}
			}
		}
		return UIDragElement<PosNOTICE>::send_cursor(clicked, cursorpos);
	}
}

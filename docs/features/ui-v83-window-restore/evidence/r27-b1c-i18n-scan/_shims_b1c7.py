"""R27-B1c-7 运行时负对照的三件构建垫片（合并存档；只用于构建 worktree，未入任何提交）。

用法：在 worktree 根跑一遍本脚本（路径按 worktree 相对），再 MSBuild 增量链接。
1. read_pixels 垫片 —— 提交树缺员补链接（GraphicsGL.h 声明 + .cpp 定义）
2. dbginit 落文件 —— Win32 也写 _init_debug.log（等 init:done 的仪器）
3. refresh_localized 落文件 —— _refresh_debug.log + version.prefix 值探针（负对照判据）
"""
import io

def rd(p):
    return io.open(p, encoding='utf-8').read()

def wr(p, s):
    io.open(p, 'w', encoding='utf-8', newline='').write(s)

# 1) read_pixels
h = rd('Graphics/GraphicsGL.h')
anchor = '\t\tvoid drawscreenfill(float red, float green, float blue, float alpha);'
assert anchor in h
h = h.replace(anchor, anchor + '''

		// 构建垫片：提交树 IO/Window.cpp capture_screenshot 调本函数（aa0d4e2 把调用点
		// 入库了、声明没入库）。最小实现让负对照构建可链接；正式修法=并行会话把
		// GraphicsGL 的 read_pixels 入库。
		void read_pixels(int16_t width, int16_t height, uint8_t* out) const;''', 1)
wr('Graphics/GraphicsGL.h', h)

c = rd('Graphics/GraphicsGL.cpp')
i = c.index('\tvoid GraphicsGL::drawscreenfill')
j = c.index('\n\t}', i) + 3
wr('Graphics/GraphicsGL.cpp', c[:j] + '''

	void GraphicsGL::read_pixels(int16_t width, int16_t height, uint8_t* out) const
	{
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, out);
	}''' + c[j:])

# 2) dbginit 落文件
m = rd('MapleStory.cpp')
old = '''	static void dbginit(const std::string& stage)
	{
#ifdef __ANDROID__
		std::ofstream f("_init_debug.log", std::ios::app);
		f << stage << '\\n';
#endif
	}'''
new = '''	static void dbginit(const std::string& stage)
	{
		std::ofstream f("_init_debug.log", std::ios::app);
		f << stage << '\\n';
		f.flush();
	}'''
assert old in m
wr('MapleStory.cpp', m.replace(old, new, 1))

# 3) refresh_localized 落文件 + 值探针
u = rd('IO/UIElement.cpp')
old2 = '''		LOG(LOG_INFO, "Locale refresh gen=" << Locale::get().generation() << " rebound "
			<< locale_binds.count() << " labels (UIElement type " << get_type() << ")");'''
new2 = old2 + '''
		{
			std::ofstream f("_refresh_debug.log", std::ios::app);
			f << "REFRESH gen=" << Locale::get().generation() << " rebound="
				<< locale_binds.count() << " type=" << get_type()
				<< " version.prefix=" << Locale::text("version.prefix") << "\\n";
			f.flush();
		}'''
assert old2 in u
if '#include <fstream>' not in u:
    k = u.index('#include')
    k2 = u.index('\n', k)
    u = u[:k2 + 1] + '#include <fstream>\n' + u[k2 + 1:]
wr('IO/UIElement.cpp', u.replace(old2, new2, 1))
print('3 shims applied')

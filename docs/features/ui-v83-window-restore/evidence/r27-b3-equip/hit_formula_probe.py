# R27-B3-4a 判据：旧「绘制几何」与「命中除法」不同源时，一格有多少像素算到邻格
#
# 只读复刻两侧的真实实现（不改任何 cpp）：
#   版式/绘制 = UIItemInventory::get_slotpos()   (IO/UITypes/UIItemInventory.cpp:1543)
#   旧命中     = UIItemInventory::slot_by_position() 改造前的除法 (git show HEAD:...cpp)
# 复刻常数全部取自代码实测：slot_pos=(8,50)、step=32+4 / 32+2、page_dx={0,149,298,447}
# 判定 = 对该格**画出去的 32x32 全部像素**逐个喂给旧除法，看回解出来的槽号是不是这一格。
#
# 跑法：python docs/features/ui-v83-window-restore/evidence/r27-b3-equip/hit_formula_probe.py
import io
import sys

SLOT_POS = (8, 50)
STEP_X = 32 + 4
STEP_Y = 32 + 2
PAGE_DX = [0, 149, 298, 447]
FIRST_COLLAPSED = 2   # slotrange.at(tab).first（消耗药 = 2）
CELL = 32             # Icon::cell_width / cell_height


def get_slotpos(slot, full):
    """绘制侧版式（唯一定义处）。"""
    ab = slot - (1 if full else FIRST_COLLAPSED)
    if full:
        page = max(0, min(3, ab // 24))
        rem = ab % 24
        row, col = rem // 4, rem % 4
        return SLOT_POS[0] + col * STEP_X + PAGE_DX[page], SLOT_POS[1] + row * STEP_Y
    row, col = ab // 4, ab % 4
    return SLOT_POS[0] + col * STEP_X, SLOT_POS[1] + row * STEP_Y


def old_hit(x, y, full):
    """改造前的除法：col = x/36、row = y/34 - 1、+ (col/4)*28 的页补偿。"""
    first = 1 if full else FIRST_COLLAPSED
    col = x // STEP_X
    row = y // STEP_Y - 1
    return col + first + 4 * row + (col // 4) * 28


def main():
    out = io.StringIO()

    for full, label, slots in (
        (False, "收起态 4 列 x 6 行", range(FIRST_COLLAPSED, FIRST_COLLAPSED + 24)),
        (True, "展开态 4 页 x (4 列 x 6 行)", range(1, 97)),
    ):
        out.write("== %s ==\n" % label)

        # 单轴：格内有多少列/行的像素越过除法边界
        right = bottom = 0
        for s in slots:
            px, py = get_slotpos(s, full)
            right += sum(1 for dx in range(CELL) if old_hit(px + dx, py, full) != s)
            bottom += sum(1 for dy in range(CELL) if old_hit(px, py + dy, full) != s)
        n = len(list(slots))
        out.write("  每格右侧串到邻列：%d px      每格下串到下行：%d px\n"
                  % (right // n, bottom // n))

        # 整格面积
        bad = tot = 0
        shifts = {}
        for s in slots:
            px, py = get_slotpos(s, full)
            for dx in range(CELL):
                for dy in range(CELL):
                    got = old_hit(px + dx, py + dy, full)
                    tot += 1
                    if got != s:
                        bad += 1
                        shifts[got - s] = shifts.get(got - s, 0) + 1
        out.write("  画在这一格上的 %d px 里有 %d px（%.1f%%）解成别的槽\n"
                  % (tot, bad, 100.0 * bad / tot))
        out.write("  错到哪：偏移(槽号差) -> 像素数  %s\n\n"
                  % sorted(shifts.items(), key=lambda kv: -kv[1]))

    # 展开态：格心（最容易被点到的那一点）整体偏多少
    out.write("== 展开态·格心解出的槽号差（按页）==\n")
    for page in range(4):
        s = 1 + page * 24 + 6
        px, py = get_slotpos(s, True)
        d = old_hit(px + CELL // 2, py + CELL // 2, True) - s
        out.write("  第 %d 页（0 基）: %+.0f 槽\n" % (page, d))

    text = out.getvalue()
    with open(sys.argv[1] if len(sys.argv) > 1 else
              "docs/features/ui-v83-window-restore/evidence/r27-b3-equip/hit_formula_probe.txt",
              "w", encoding="utf-8") as fp:
        fp.write(text)
    print("written")


if __name__ == "__main__":
    main()

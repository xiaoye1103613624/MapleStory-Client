"""参考图像素定标结论 — 创建角色页（种族选择）

数据来源：Login.img runtime JSON + 正确客户端截图模板匹配（ssd/细化）。
用途：对齐 UI 常量的 ground truth；修复"位置不对"问题。

--- 一、元素屏幕 bbox（render_topleft → render_topright） ---

| 元素          | 左上     | 右下     | 尺寸    | 说明                   |
|---------------|----------|----------|---------|------------------------|
| title_text    | (555,92) | (737,131)| 182x39  | "选择您喜欢的角色"     |
| knight_card   | (344,119)| (553,338)| 209x219 | 骑士团                 |
| explorer_card | (533,119)| (756,340)| 223x221 | 冒险家                 |
| aran_card     | (751,120)| (932,342)| 181x222 | 战神                   |
| desc_board    | (358,351)| (937,514)| 579x163 | 职业介绍文本           |
| bt_select     | (770,462)| (843,491)| 73x29   | "选择 >>"              |

--- 二、窗口原点推断 ---

- classic 模式窗口原点 = position.y(代码) − 各元素相对偏移 = ?
- title 相对 origin = (315,52) → origin = (555−315, 92−52) = **(240,40)** ✅
- 验证：knight 原代码 pos+(104,79)= (240+104, 40+79)=(344,119) ✅  
- 验证：desc_board 原代码 pos+(60,270)=(300,310) ❌ 原生在 (358,351) → 差值 **(+58,+41)**

--- 三、经典模式常量错误清单 ---

UIRaceSelect::init_classic():
- classic_text_pos = Point<int16_t>(60, 270); // → 应改为 (118, 311) (=60+58, 270+41)

其他：
- 回到首页按钮位置需要重测（当前 BtStart 太小，原生可能是 BtStart2 124x52）
- "必须选性别"警告框未定位（UILoginNotice 类型？待 UIChatBar 或 Gender 证据）
- NAME OF CHARACTER 铭牌未定位（CharName 201x224）→ 待 NewChar/charName 提取

--- 四、下一步 ---

1. 改 `classic_text_pos` (60,270)→(118,311)，构建复测种族页
2. 提取 `NewChar/charName` 和 `Gender/backgrnd` 做剩余 UI 匹配
3. 确认 BtStart/BtStart2 哪个是“回到首页”（当前 left=50,y=425±10）

"""
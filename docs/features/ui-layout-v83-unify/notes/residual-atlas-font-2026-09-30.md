# 登录输入乱码 / 双弹窗 / 属性卡无背景（续修）

> 状态：已实现（2026-09-30）  
> 部署：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`  
> 产物：`docs/features/ui-layout-v83-unify/notes/residual-atlas-font-2026-09-30.md`

## 根因

| 现象 | 根因 |
| --- | --- |
| 账号密码白勾/短横乱码 | `GraphicsGL::init` 装完字体后 **未设置 `border=(0,fontymax)`**，后续 UI 位图从 (0,0) 打包 **覆盖字形区**；shader 仍按字体采 `.r` → L 形白线。另 FreeType `pitch≠width` 窄字上传串行 |
| 登录后两个弹窗 | BeiDou `Notice/text/*`（221×109 全不透明羊皮纸）与 `backgrnd/0`（362×219）叠画；旧阈值 70%×50% 对 61%×49.8% 判失败 |
| 属性卡无背景 | 同上：charInfo1 若被装进字体区按 `.r` 绘制会「看不见」；另强制清零脏 origin |

## 修复

1. `GraphicsGL.cpp`：字体装完后 `border = (0, fontymax)`；`addfont` 按 pitch 紧凑上传  
2. `UILoginNotice.cpp`：阈值改为 55%×45%，BeiDou 且 text≥180×90 只画文案板  
3. `ChatBalloonHorizontal`：空文案不画边框  
4. `UICharSelect.cpp`：charInfo origin 一律清零  

## 验证

- [ ] 账号/密码 ASCII 清晰可读  
- [ ] 错密/已登录等 Notice 只有一块羊皮纸  
- [ ] 选角后 charInfo1 米色属性表可见  

# 背景资源闪烁风险清单

- 扫描资源总数：631
- 含 alpha 渐变帧(a0/a1)的资源：94
- alpha 帧总数：1205
- 闪烁候选帧(span≥64 且 delay≤300ms)：49
- 涉及资源数：12

风险=opacity_span×(人眼敏感频段权重,3-12Hz×1.0)。

| 资源 | 候选帧 | 最大span | 最小delay(ms) | 最高频(Hz) | 风险分 |
|---|---|---|---|---|---|
| ancientforest2.img | 1 | 255 | 150 | 3.3 | 255.0 |
| moltenRock.img | 2 | 255 | 80 | 6.2 | 255.0 |
| moltenrocka.img | 2 | 255 | 80 | 6.2 | 255.0 |
| moltenrockabc.img | 2 | 255 | 80 | 6.2 | 255.0 |
| shineWood.img | 1 | 215 | 100 | 5.0 | 215.0 |
| shinewoodolaola.img | 1 | 215 | 100 | 5.0 | 215.0 |
| dragonRoad.img | 7 | 195 | 100 | 5.0 | 195.0 |
| pl_abrup_kaftafell_night.img | 8 | 165 | 150 | 3.3 | 165.0 |
| starplanetpvp.img | 2 | 255 | 300 | 1.7 | 127.5 |
| globalJP2.img | 1 | 150 | 300 | 1.7 | 75.0 |
| dragondream.img | 18 | 128 | 300 | 1.7 | 64.0 |
| toyCastleDungeon2.img | 4 | 65 | 250 | 2.0 | 32.5 |

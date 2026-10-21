app.preferences.rulerUnits = Units.PIXELS;

function solid(r, g, b) {
  var c = new SolidColor();
  c.rgb.red = r;
  c.rgb.green = g;
  c.rgb.blue = b;
  return c;
}

function fillSel(r, g, b) {
  app.activeDocument.selection.fill(solid(r, g, b));
}

function rectSel(x, y, w, h) {
  app.activeDocument.selection.select([
    [x, y],
    [x + w, y],
    [x + w, y + h],
    [x, y + h]
  ]);
}

function newLayer(name) {
  var layer = app.activeDocument.artLayers.add();
  layer.name = name;
  return layer;
}

function fillRectLayer(name, x, y, w, h, r, g, b) {
  var layer = newLayer(name);
  rectSel(x, y, w, h);
  fillSel(r, g, b);
  app.activeDocument.selection.deselect();
  return layer;
}

function strokeRectLayer(name, x, y, w, h, r, g, b, t) {
  var layer = newLayer(name);
  rectSel(x, y, w, h);
  app.activeDocument.selection.stroke(solid(r, g, b), t, StrokeLocation.INSIDE);
  app.activeDocument.selection.deselect();
  return layer;
}

function addCenteredText(name, text, cx, baselineY, sizePt, r, g, b) {
  var layer = newLayer(name);
  layer.kind = LayerKind.TEXT;
  var ti = layer.textItem;
  ti.kind = TextType.POINTTEXT;
  ti.contents = text;
  ti.size = new UnitValue(sizePt, "pt");
  ti.color = solid(r, g, b);
  ti.justification = Justification.CENTER;
  ti.position = [cx, baselineY];
  try { ti.font = "MicrosoftYaHei"; } catch (e1) {
    try { ti.font = "SimSun"; } catch (e2) {}
  }
  return layer;
}

function addLeftText(name, text, x, baselineY, sizePt, r, g, b) {
  var layer = newLayer(name);
  layer.kind = LayerKind.TEXT;
  var ti = layer.textItem;
  ti.kind = TextType.POINTTEXT;
  ti.contents = text;
  ti.size = new UnitValue(sizePt, "pt");
  ti.color = solid(r, g, b);
  ti.justification = Justification.LEFT;
  ti.position = [x, baselineY];
  try { ti.font = "Arial"; } catch (e) {}
  return layer;
}

function drawButton(name, x, y, w, h, tR, tG, tB, bR, bG, bB, bdR, bdG, bdB, label, pressed) {
  var topR = pressed ? Math.max(0, tR - 40) : tR;
  var topG = pressed ? Math.max(0, tG - 40) : tG;
  var topB = pressed ? Math.max(0, tB - 40) : tB;
  var botR = pressed ? Math.max(0, bR - 45) : bR;
  var botG = pressed ? Math.max(0, bG - 45) : bG;
  var botB = pressed ? Math.max(0, bB - 45) : bB;

  fillRectLayer(name + "_base", x, y, w, h, botR, botG, botB);
  var mid = Math.floor(h / 2);
  fillRectLayer(name + "_top", x + 1, y + 1, w - 2, mid, topR, topG, topB);
  fillRectLayer(name + "_bot", x + 1, y + mid, w - 2, h - mid - 1, botR, botG, botB);
  if (pressed) {
    var inset = fillRectLayer(name + "_inset", x + 2, y + 2, w - 4, 4, 90, 70, 55);
    inset.opacity = 40;
  } else {
    var hi = fillRectLayer(name + "_hi", x + 2, y + 2, w - 4, 2, 255, 248, 235);
    hi.opacity = 60;
  }
  strokeRectLayer(name + "_bd", x, y, w, h, bdR, bdG, bdB, 1);
  addCenteredText(name + "_txt", label, x + w / 2, y + Math.floor(h * 0.72), 11, 25, 25, 25);
}

function drawPortrait(name, x, y, s) {
  fillRectLayer(name + "_bg", x, y, s, s, 230, 230, 226);
  fillRectLayer(name + "_in", x + 3, y + 3, s - 6, s - 6, 248, 248, 245);
  strokeRectLayer(name + "_bd", x, y, s, s, 150, 150, 145, 1);
  strokeRectLayer(name + "_ib", x + 3, y + 3, s - 6, s - 6, 185, 185, 180, 1);
}

function isTopLocked(c, r) {
  return c >= 6;
}

function isBotLocked(c, r) {
  if (r <= 2) return c >= 3;
  if (c < 2) return true;
  return c >= 4;
}

function drawGrid(prefix, gx, gy, cols, rows, slot, lockFn) {
  fillRectLayer(prefix + "_panel", gx - 1, gy - 1, cols * slot + 2, rows * slot + 2, 252, 252, 250);
  var r, c, sx, sy;
  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++) {
      sx = gx + c * slot;
      sy = gy + r * slot;
      if (lockFn(c, r)) {
        fillRectLayer(prefix + "_L" + r + "_" + c, sx, sy, slot - 1, slot - 1, 255, 228, 228);
      } else {
        fillRectLayer(prefix + "_C" + r + "_" + c, sx, sy, slot - 1, slot - 1, 248, 248, 246);
      }
      strokeRectLayer(prefix + "_B" + r + "_" + c, sx, sy, slot - 1, slot - 1, 195, 195, 190, 1);
    }
  }
  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++) {
      if (!lockFn(c, r)) continue;
      sx = gx + c * slot;
      sy = gy + r * slot;
      var pad2 = 5;
      newLayer(prefix + "_Xh" + r + "_" + c);
      app.activeDocument.selection.select([
        [sx + pad2, sy + pad2],
        [sx + pad2 + 2, sy + pad2],
        [sx + slot - 2 - pad2, sy + slot - 2 - pad2],
        [sx + slot - 4 - pad2, sy + slot - 2 - pad2]
      ]);
      fillSel(235, 160, 165);
      app.activeDocument.selection.select([
        [sx + slot - 2 - pad2, sy + pad2],
        [sx + slot - 4 - pad2, sy + pad2],
        [sx + pad2, sy + slot - 2 - pad2],
        [sx + pad2 + 2, sy + slot - 2 - pad2]
      ]);
      fillSel(235, 160, 165);
      app.activeDocument.selection.deselect();
    }
  }
}

function drawMeso(prefix, x, y, w, h, amount) {
  fillRectLayer(prefix + "_bg", x, y, w, h, 232, 232, 228);
  strokeRectLayer(prefix + "_bd", x, y, w, h, 175, 175, 170, 1);
  var cs = h - 8;
  var cx = x + 6;
  var cy = y + 4;
  fillRectLayer(prefix + "_coin", cx, cy, cs, cs, 215, 45, 45);
  fillRectLayer(prefix + "_coinHi", cx + 2, cy + 2, cs - 4, Math.floor(cs / 2), 240, 85, 85);
  strokeRectLayer(prefix + "_coinBd", cx, cy, cs, cs, 130, 25, 25, 1);
  addCenteredText(prefix + "_c", "c", cx + cs / 2, cy + cs * 0.72, 9, 255, 255, 255);
  addLeftText(prefix + "_amt", amount, x + 28, y + h * 0.72, 11, 30, 30, 30);
}

(function () {
  var doc = app.activeDocument;

  var bg = doc.activeLayer;
  if (bg.isBackgroundLayer) {
    bg.isBackgroundLayer = false;
    bg.name = "Canvas_BG";
  }
  doc.selection.selectAll();
  fillSel(195, 200, 205);
  doc.selection.deselect();

  var WX = 30, WY = 10, WW = 420, WH = 560;
  var sh = fillRectLayer("frame_shadow", WX + 3, WY + 3, WW, WH, 140, 140, 140);
  sh.opacity = 35;
  fillRectLayer("frame_bg", WX, WY, WW, WH, 236, 236, 233);
  strokeRectLayer("frame_bd_out", WX, WY, WW, WH, 165, 165, 162, 2);
  strokeRectLayer("frame_bd_in", WX + 3, WY + 3, WW - 6, WH - 6, 205, 205, 202, 1);

  var CX = WX + 8, CY = WY + 8, CW = WW - 16, CH = WH - 16;
  fillRectLayer("content_bg", CX, CY, CW, CH, 243, 243, 240);

  var midY = WY + Math.floor(WH / 2);
  fillRectLayer("divider", CX + 2, midY - 1, CW - 4, 2, 155, 155, 152);

  var sideW = 92;
  var sideX = CX + 4;
  var btnW = 78, btnH = 22, btnX = sideX + 7;
  var slot = 28, cols = 10, rows = 4;
  var gx = CX + sideW + 4;

  var S_SWITCH = "\u5207\u6362\u7a97\u53e3";
  var S_SMART_IN = "\u667a\u80fd\u5b58\u5165";
  var S_FULL_IN = "\u5168\u989d\u5b58\u5165";
  var S_TIP1 = "\u53f3\u952e\u9053\u5177\u56fe\u6807";
  var S_TIP2 = "\u5feb\u901f\u5b58\u53d6\u9053\u5177";
  var S_EXIT = "\u9000\u51fa\u4ed3\u5e93";
  var S_SMART_OUT = "\u667a\u80fd\u53d6\u51fa";
  var S_FULL_OUT = "\u5168\u989d\u53d6\u51fa";
  var S_SORT = "\u9053\u5177\u6392\u5217";

  var topY = CY + 4;
  drawPortrait("top_portrait", sideX + 10, topY + 4, 70);
  var peachT = [255, 212, 172], peachB = [242, 178, 132], peachBd = [115, 85, 65];
  var by = topY + 84;
  drawButton("btn_switch", btnX, by, btnW, btnH, peachT[0], peachT[1], peachT[2], peachB[0], peachB[1], peachB[2], peachBd[0], peachBd[1], peachBd[2], S_SWITCH, false);
  drawButton("btn_smart_in", btnX, by + 28, btnW, btnH, peachT[0], peachT[1], peachT[2], peachB[0], peachB[1], peachB[2], peachBd[0], peachBd[1], peachBd[2], S_SMART_IN, false);
  drawButton("btn_full_in", btnX, by + 56, btnW, btnH, peachT[0], peachT[1], peachT[2], peachB[0], peachB[1], peachB[2], peachBd[0], peachBd[1], peachBd[2], S_FULL_IN, false);
  addCenteredText("tip1", S_TIP1, sideX + 46, by + 98, 10, 35, 95, 200);
  addCenteredText("tip2", S_TIP2, sideX + 46, by + 112, 10, 35, 95, 200);

  var gy = topY + 6;
  drawGrid("top", gx, gy, cols, rows, slot, isTopLocked);
  drawMeso("top_meso", gx, gy + rows * slot + 6, cols * slot, 22, "100,000,021");

  var botY = midY + 6;
  drawPortrait("bot_portrait", sideX + 10, botY + 2, 70);
  var greenT = [188, 228, 148], greenB = [148, 198, 108], greenBd = [75, 105, 55];
  var blueT = [168, 218, 238], blueB = [128, 182, 212], blueBd = [55, 95, 125];
  var bby = botY + 80;
  drawButton("btn_exit", btnX, bby, btnW, btnH, greenT[0], greenT[1], greenT[2], greenB[0], greenB[1], greenB[2], greenBd[0], greenBd[1], greenBd[2], S_EXIT, false);
  drawButton("btn_smart_out", btnX, bby + 26, btnW, btnH, peachT[0], peachT[1], peachT[2], peachB[0], peachB[1], peachB[2], peachBd[0], peachBd[1], peachBd[2], S_SMART_OUT, false);
  drawButton("btn_full_out", btnX, bby + 52, btnW, btnH, peachT[0], peachT[1], peachT[2], peachB[0], peachB[1], peachB[2], peachBd[0], peachBd[1], peachBd[2], S_FULL_OUT, false);
  drawButton("btn_sort", btnX, bby + 78, btnW, btnH, blueT[0], blueT[1], blueT[2], blueB[0], blueB[1], blueB[2], blueBd[0], blueBd[1], blueBd[2], S_SORT, false);

  var bgy = botY + 4;
  drawGrid("bot", gx, bgy, cols, rows, slot, isBotLocked);
  drawMeso("bot_meso", gx, bgy + rows * slot + 6, cols * slot, 22, "1,000,000,000");

  var sy0 = WY + WH + 18;
  addLeftText("states_title", "Button States  Normal | Pressed", 40, sy0 + 12, 12, 40, 40, 40);

  var items = [
    [S_SWITCH, peachT, peachB, peachBd],
    [S_SMART_IN, peachT, peachB, peachBd],
    [S_FULL_IN, peachT, peachB, peachBd],
    [S_EXIT, greenT, greenB, greenBd],
    [S_SMART_OUT, peachT, peachB, peachBd],
    [S_FULL_OUT, peachT, peachB, peachBd],
    [S_SORT, blueT, blueB, blueBd]
  ];
  var i;
  for (i = 0; i < items.length; i++) {
    var it = items[i];
    var yy = sy0 + 22 + i * 28;
    drawButton("N_" + i, 40, yy, btnW, btnH, it[1][0], it[1][1], it[1][2], it[2][0], it[2][1], it[2][2], it[3][0], it[3][1], it[3][2], it[0], false);
    drawButton("P_" + i, 140, yy, btnW, btnH, it[1][0], it[1][1], it[1][2], it[2][0], it[2][1], it[2][2], it[3][0], it[3][1], it[3][2], it[0], true);
  }
})();

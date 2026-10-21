// Build MapleStory-style 3x3 hotkey grid (white slot fills + opaque grid lines).
function solidColor(r, g, b) {
  var c = new SolidColor();
  c.rgb.red = r;
  c.rgb.green = g;
  c.rgb.blue = b;
  return c;
}

function fillRect(x, y, w, h, color) {
  var doc = app.activeDocument;
  doc.selection.select([
    [x, y],
    [x + w, y],
    [x + w, y + h],
    [x, y + h]
  ]);
  doc.selection.fill(color);
  doc.selection.deselect();
}

function drawHLine(x, y, w, color) {
  fillRect(x, y, w, 1, color);
}

function drawVLine(x, y, h, color) {
  fillRect(x, y, 1, h, color);
}

function drawSlotBevel(x, y, size, colHi, colLo, colOuter) {
  drawHLine(x, y, size, colOuter);
  drawHLine(x, y + size - 1, size, colOuter);
  drawVLine(x, y, size, colOuter);
  drawVLine(x + size - 1, y, size, colOuter);
  drawHLine(x + 1, y + 1, size - 2, colHi);
  drawVLine(x + 1, y + 1, size - 2, colHi);
  drawHLine(x + 1, y + size - 2, size - 2, colLo);
  drawVLine(x + size - 2, y + 1, size - 2, colLo);
}

function drawSlotFill(x, y, size, colFill) {
  fillRect(x + 2, y + 2, size - 4, size - 4, colFill);
}

function addLayerIn(group, name) {
  var layer = group.artLayers.add();
  layer.name = name;
  return layer;
}

function addGroupIn(parent, name) {
  var g;
  if (parent) {
    g = parent.layerSets.add();
  } else {
    g = app.activeDocument.layerSets.add();
  }
  g.name = name;
  return g;
}

function main() {
  var CELL = 32;
  var GAP = 1;
  var OX = 2;
  var OY = 2;
  var COLS = 3;
  var ROWS = 3;
  var boardW = OX * 2 + COLS * CELL + (COLS - 1) * GAP;
  var boardH = OY * 2 + ROWS * CELL + (ROWS - 1) * GAP;
  var canvasW = 220;
  var canvasH = 120;

  var colFill = solidColor(248, 248, 248);
  var colHi = solidColor(255, 255, 255);
  var colLo = solidColor(140, 140, 140);
  var colOuter = solidColor(90, 90, 90);
  var colBoardBg = solidColor(210, 210, 210);

  var doc = app.documents.add(
    canvasW,
    canvasH,
    72,
    "hotkey-grid-3x3",
    NewDocumentMode.RGB,
    DocumentFill.TRANSPARENT
  );

  // Materials (side sample)
  var materials = addGroupIn(null, "Materials");
  var matX = 120;
  var matY = 16;
  var matFillLayer = addLayerIn(materials, "slot/fill");
  doc.activeLayer = matFillLayer;
  drawSlotFill(matX, matY, CELL, colFill);
  var matBevelLayer = addLayerIn(materials, "slot/bevel");
  doc.activeLayer = matBevelLayer;
  drawSlotBevel(matX, matY, CELL, colHi, colLo, colOuter);

  // Final board
  var finalGroup = addGroupIn(null, "Final");

  var boardPlate = addLayerIn(finalGroup, "board/plate");
  doc.activeLayer = boardPlate;
  fillRect(OX - 1, OY - 1, boardW - 2, boardH - 2, colBoardBg);
  boardPlate.opacity = 35;

  var slotFills = addGroupIn(finalGroup, "SlotFills");
  var fillsLayer = addLayerIn(slotFills, "slotfills/all");
  doc.activeLayer = fillsLayer;
  var r, c, x, y;
  for (r = 0; r < ROWS; r++) {
    for (c = 0; c < COLS; c++) {
      x = OX + c * (CELL + GAP);
      y = OY + r * (CELL + GAP);
      drawSlotFill(x, y, CELL, colFill);
    }
  }

  var gridLines = addGroupIn(finalGroup, "GridLines");
  var linesLayer = addLayerIn(gridLines, "grid/bevels");
  doc.activeLayer = linesLayer;
  for (r = 0; r < ROWS; r++) {
    for (c = 0; c < COLS; c++) {
      x = OX + c * (CELL + GAP);
      y = OY + r * (CELL + GAP);
      drawSlotBevel(x, y, CELL, colHi, colLo, colOuter);
    }
  }

  // Keep Final above Materials in stack (created later = higher). Done.
  doc.activeLayer = slotFills;

  return {
    ok: true,
    document_id: doc.id,
    board: { w: boardW, h: boardH, cell: CELL, gap: GAP, ox: OX, oy: OY },
    colors: {
      fill: [248, 248, 248],
      hi: [255, 255, 255],
      lo: [140, 140, 140],
      outer: [90, 90, 90]
    }
  };
}

return main();

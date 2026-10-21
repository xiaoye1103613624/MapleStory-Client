// Auto-generated: char-stats v4 componentized PSD
app.displayDialogs = DialogModes.NO;
app.preferences.rulerUnits = Units.PIXELS;
var COMP = "E:/project/MapleStory-Client/docs/features/char-stats-ui-ps/ui/detail/v4/components/";
var OUT_PSD = "E:/project/MapleStory-Client/docs/features/char-stats-ui-ps/Character_Stats_Detail_V4.psd";
var W = 1102, H = 520;
var doc = app.documents.add(W, H, 72, 'Character_Stats_Detail_V4', NewDocumentMode.RGB, DocumentFill.TRANSPARENT);

function placeFile(path, name, dx, dy) {
  var f = new File(path);
  if (!f.exists) { throw new Error('missing: ' + path); }
  var desc = new ActionDescriptor();
  desc.putPath(charIDToTypeID('null'), f);
  desc.putEnumerated(charIDToTypeID('FTcs'), charIDToTypeID('QCSt'), charIDToTypeID('Qcsa'));
  executeAction(charIDToTypeID('Plc '), desc, DialogModes.NO);
  var layer = app.activeDocument.activeLayer;
  layer.name = name;
  var b = layer.bounds;
  layer.translate(dx - b[0].as('px'), dy - b[1].as('px'));
  return layer;
}
function addText(name, contents, x, y, sizePt, r, g, b) {
  var layer = app.activeDocument.artLayers.add();
  layer.kind = LayerKind.TEXT;
  layer.name = name;
  var ti = layer.textItem;
  ti.kind = TextType.POINTTEXT;
  ti.contents = contents;
  ti.position = [x, y + sizePt];
  try { ti.size = sizePt; } catch (eSz) { try { ti.size = new UnitValue(sizePt, 'pt'); } catch (eSz2) {} }
  try { ti.font = "MicrosoftYaHei"; } catch (e1) {
    try { ti.font = "MicrosoftYaHeiUI"; } catch (e2) {
      try { ti.font = "SimHei"; } catch (e3) {}
    }
  }
  var c = new SolidColor();
  c.rgb.red = r; c.rgb.green = g; c.rgb.blue = b;
  ti.color = c;
  return layer;
}
function ensureGroup(name) {
  var g = app.activeDocument.layerSets.add();
  g.name = name;
  return g;
}

// --- Materials shelf (reusable component sprites) ---
placeFile(COMP + "label_chip_pink.png", "mat_label_pink", 682, 8);
placeFile(COMP + "label_chip_green.png", "mat_label_green", 782, 8);
placeFile(COMP + "label_chip_resist.png", "mat_label_resist", 882, 8);
placeFile(COMP + "label_chip_combat.png", "mat_label_combat", 982, 8);
placeFile(COMP + "label_chip_dmg.png", "mat_label_dmg", 682, 36);
placeFile(COMP + "label_chip_crit_rate.png", "mat_label_crit_rate", 782, 36);
placeFile(COMP + "label_chip_crit_dmg.png", "mat_label_crit_dmg", 882, 36);
placeFile(COMP + "label_chip_final.png", "mat_label_final", 982, 36);
placeFile(COMP + "label_chip_boss.png", "mat_label_boss", 682, 64);
placeFile(COMP + "label_chip_ignore.png", "mat_label_ignore", 782, 64);
placeFile(COMP + "label_chip_status.png", "mat_label_status", 882, 64);
placeFile(COMP + "label_chip_buff.png", "mat_label_buff", 982, 64);
placeFile(COMP + "label_chip_stance.png", "mat_label_stance", 682, 92);
placeFile(COMP + "label_chip_drop.png", "mat_label_drop", 782, 92);
placeFile(COMP + "label_chip_meso.png", "mat_label_meso", 882, 92);
placeFile(COMP + "label_chip_honor.png", "mat_label_honor", 982, 92);
placeFile(COMP + "label_chip_ailment.png", "mat_label_ailment", 682, 120);
placeFile(COMP + "value_box_80.png", "mat_value_80", 782, 120);
placeFile(COMP + "value_box_240.png", "mat_value_240", 882, 120);
placeFile(COMP + "value_box_205.png", "mat_value_left", 982, 120);
placeFile(COMP + "resist_label.png", "mat_resist_label", 682, 148);
placeFile(COMP + "resist_value.png", "mat_resist_value", 782, 148);
placeFile(COMP + "panel_bg_main.png", "mat_panel_main", 682, 228);
placeFile(COMP + "panel_bg_detail.png", "mat_panel_detail", 682, 468);
// --- Final assembly (component instances) ---
placeFile(COMP + "panel_bg_main.png", "Main_PanelBg", 0, 0);
placeFile(COMP + "panel_bg_detail.png", "Detail_PanelBg", 306, 0);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_0", 6, 24);
placeFile(COMP + "value_box_205.png", "Main_IdVal_0", 88, 24);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_1", 6, 43);
placeFile(COMP + "value_box_205.png", "Main_IdVal_1", 88, 43);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_2", 6, 62);
placeFile(COMP + "value_box_205.png", "Main_IdVal_2", 88, 62);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_3", 6, 81);
placeFile(COMP + "value_box_205.png", "Main_IdVal_3", 88, 81);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_4", 6, 100);
placeFile(COMP + "value_box_205.png", "Main_IdVal_4", 88, 100);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_5", 6, 119);
placeFile(COMP + "value_box_205.png", "Main_IdVal_5", 88, 119);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_6", 6, 138);
placeFile(COMP + "value_box_205.png", "Main_IdVal_6", 88, 138);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_7", 6, 157);
placeFile(COMP + "value_box_205.png", "Main_IdVal_7", 88, 157);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_8", 6, 176);
placeFile(COMP + "value_box_205.png", "Main_IdVal_8", 88, 176);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_9", 6, 195);
placeFile(COMP + "value_box_205.png", "Main_IdVal_9", 88, 195);
placeFile(COMP + "label_chip_pink.png", "Main_IdChip_10", 6, 214);
placeFile(COMP + "value_box_205.png", "Main_IdVal_10", 88, 214);
placeFile(COMP + "label_chip_green.png", "Main_CoreChip_0", 6, 236);
placeFile(COMP + "value_box_205.png", "Main_CoreVal_0", 88, 236);
placeFile(COMP + "label_chip_green.png", "Main_CoreChip_1", 6, 255);
placeFile(COMP + "value_box_205.png", "Main_CoreVal_1", 88, 255);
placeFile(COMP + "label_chip_green.png", "Main_CoreChip_2", 6, 274);
placeFile(COMP + "value_box_205.png", "Main_CoreVal_2", 88, 274);
placeFile(COMP + "label_chip_green.png", "Main_CoreChip_3", 6, 293);
placeFile(COMP + "value_box_205.png", "Main_CoreVal_3", 88, 293);
placeFile(COMP + "resist_label.png", "Main_ResLbl_0", 6, 318);
placeFile(COMP + "resist_value.png", "Main_ResVal_0", 6, 334);
placeFile(COMP + "resist_label.png", "Main_ResLbl_1", 103, 318);
placeFile(COMP + "resist_value.png", "Main_ResVal_1", 103, 334);
placeFile(COMP + "resist_label.png", "Main_ResLbl_2", 200, 318);
placeFile(COMP + "resist_value.png", "Main_ResVal_2", 200, 334);
placeFile(COMP + "resist_label.png", "Main_ResLbl_3", 6, 353);
placeFile(COMP + "resist_value.png", "Main_ResVal_3", 6, 369);
placeFile(COMP + "resist_label.png", "Main_ResLbl_4", 103, 353);
placeFile(COMP + "resist_value.png", "Main_ResVal_4", 103, 369);
placeFile(COMP + "resist_label.png", "Main_ResLbl_5", 200, 353);
placeFile(COMP + "resist_value.png", "Main_ResVal_5", 200, 369);
placeFile(COMP + "label_chip_combat.png", "Detail_AtkChip", 312, 24);
placeFile(COMP + "value_box_240.png", "Detail_AtkVal", 394, 24);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_0", 312, 42);
placeFile(COMP + "value_box_80.png", "Detail_LVal_0", 394, 42);
placeFile(COMP + "label_chip_dmg.png", "Detail_RChip_0", 478, 42);
placeFile(COMP + "value_box_80.png", "Detail_RVal_0", 560, 42);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_1", 312, 60);
placeFile(COMP + "value_box_80.png", "Detail_LVal_1", 394, 60);
placeFile(COMP + "label_chip_combat.png", "Detail_RChip_1", 478, 60);
placeFile(COMP + "value_box_80.png", "Detail_RVal_1", 560, 60);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_2", 312, 78);
placeFile(COMP + "value_box_80.png", "Detail_LVal_2", 394, 78);
placeFile(COMP + "label_chip_final.png", "Detail_RChip_2", 478, 78);
placeFile(COMP + "value_box_80.png", "Detail_RVal_2", 560, 78);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_3", 312, 96);
placeFile(COMP + "value_box_80.png", "Detail_LVal_3", 394, 96);
placeFile(COMP + "label_chip_boss.png", "Detail_RChip_3", 478, 96);
placeFile(COMP + "value_box_80.png", "Detail_RVal_3", 560, 96);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_4", 312, 114);
placeFile(COMP + "value_box_80.png", "Detail_LVal_4", 394, 114);
placeFile(COMP + "label_chip_ignore.png", "Detail_RChip_4", 478, 114);
placeFile(COMP + "value_box_80.png", "Detail_RVal_4", 560, 114);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_5", 312, 132);
placeFile(COMP + "value_box_80.png", "Detail_LVal_5", 394, 132);
placeFile(COMP + "label_chip_status.png", "Detail_RChip_5", 478, 132);
placeFile(COMP + "value_box_80.png", "Detail_RVal_5", 560, 132);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_6", 312, 150);
placeFile(COMP + "value_box_80.png", "Detail_LVal_6", 394, 150);
placeFile(COMP + "label_chip_buff.png", "Detail_RChip_6", 478, 150);
placeFile(COMP + "value_box_80.png", "Detail_RVal_6", 560, 150);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_7", 312, 168);
placeFile(COMP + "value_box_80.png", "Detail_LVal_7", 394, 168);
placeFile(COMP + "label_chip_stance.png", "Detail_RChip_7", 478, 168);
placeFile(COMP + "value_box_80.png", "Detail_RVal_7", 560, 168);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_8", 312, 186);
placeFile(COMP + "value_box_80.png", "Detail_LVal_8", 394, 186);
placeFile(COMP + "label_chip_drop.png", "Detail_RChip_8", 478, 186);
placeFile(COMP + "value_box_80.png", "Detail_RVal_8", 560, 186);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_9", 312, 204);
placeFile(COMP + "value_box_80.png", "Detail_LVal_9", 394, 204);
placeFile(COMP + "label_chip_meso.png", "Detail_RChip_9", 478, 204);
placeFile(COMP + "value_box_80.png", "Detail_RVal_9", 560, 204);
placeFile(COMP + "label_chip_crit_rate.png", "Detail_LChip_10", 312, 222);
placeFile(COMP + "value_box_80.png", "Detail_LVal_10", 394, 222);
placeFile(COMP + "label_chip_honor.png", "Detail_RChip_10", 478, 222);
placeFile(COMP + "value_box_80.png", "Detail_RVal_10", 560, 222);
placeFile(COMP + "label_chip_crit_dmg.png", "Detail_LChip_11", 312, 240);
placeFile(COMP + "value_box_80.png", "Detail_LVal_11", 394, 240);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_11", 478, 240);
placeFile(COMP + "value_box_80.png", "Detail_RVal_11", 560, 240);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_12", 312, 258);
placeFile(COMP + "value_box_80.png", "Detail_LVal_12", 394, 258);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_12", 478, 258);
placeFile(COMP + "value_box_80.png", "Detail_RVal_12", 560, 258);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_13", 312, 276);
placeFile(COMP + "value_box_80.png", "Detail_LVal_13", 394, 276);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_13", 478, 276);
placeFile(COMP + "value_box_80.png", "Detail_RVal_13", 560, 276);
placeFile(COMP + "label_chip_combat.png", "Detail_LChip_14", 312, 294);
placeFile(COMP + "value_box_80.png", "Detail_LVal_14", 394, 294);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_14", 478, 294);
placeFile(COMP + "value_box_80.png", "Detail_RVal_14", 560, 294);
placeFile(COMP + "label_chip_ailment.png", "Detail_LChip_15", 312, 312);
placeFile(COMP + "value_box_80.png", "Detail_LVal_15", 394, 312);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_15", 478, 312);
placeFile(COMP + "value_box_80.png", "Detail_RVal_15", 560, 312);
placeFile(COMP + "label_chip_ailment.png", "Detail_LChip_16", 312, 330);
placeFile(COMP + "value_box_80.png", "Detail_LVal_16", 394, 330);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_16", 478, 330);
placeFile(COMP + "value_box_80.png", "Detail_RVal_16", 560, 330);
placeFile(COMP + "label_chip_ailment.png", "Detail_LChip_17", 312, 348);
placeFile(COMP + "value_box_80.png", "Detail_LVal_17", 394, 348);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_17", 478, 348);
placeFile(COMP + "value_box_80.png", "Detail_RVal_17", 560, 348);
placeFile(COMP + "label_chip_ailment.png", "Detail_LChip_18", 312, 366);
placeFile(COMP + "value_box_80.png", "Detail_LVal_18", 394, 366);
placeFile(COMP + "label_chip_ailment.png", "Detail_RChip_18", 478, 366);
placeFile(COMP + "value_box_80.png", "Detail_RVal_18", 560, 366);
// --- Editable text layers ---
addText("Main_Title", "\u89d2\u8272\u8d44\u6599", 150, 4, 12, 40, 40, 40);
addText("Detail_Title", "\u8be6\u7ec6\u8d44\u6599", 482, 4, 12, 40, 40, 40);
addText("Main_IdTxt_0", "\u540d\u79f0", 9, 25, 11, 255, 255, 255);
addText("Main_IdTxt_1", "\u804c\u4e1a", 9, 44, 11, 255, 255, 255);
addText("Main_IdTxt_2", "\u7b49\u7ea7", 9, 63, 11, 255, 255, 255);
addText("Main_IdTxt_3", "\u516c\u4f1a", 9, 82, 11, 255, 255, 255);
addText("Main_IdTxt_4", "\u540d\u58f0", 9, 101, 11, 255, 255, 255);
addText("Main_IdTxt_5", "\u4f24\u5bb3", 9, 120, 11, 255, 255, 255);
addText("Main_IdTxt_6", "HP", 9, 139, 11, 255, 255, 255);
addText("Main_IdTxt_7", "MP", 9, 158, 11, 255, 255, 255);
addText("Main_IdTxt_8", "AP", 9, 177, 11, 255, 255, 255);
addText("Main_IdTxt_9", "\u7ecf\u9a8c\u503c", 9, 196, 11, 255, 255, 255);
addText("Main_IdTxt_10", "EP", 9, 215, 11, 255, 255, 255);
addText("Main_CoreTxt_0", "\u529b\u91cf", 9, 237, 11, 255, 255, 255);
addText("Main_CoreTxt_1", "\u654f\u6377", 9, 256, 11, 255, 255, 255);
addText("Main_CoreTxt_2", "\u667a\u529b", 9, 275, 11, 255, 255, 255);
addText("Main_CoreTxt_3", "\u5e78\u8fd0", 9, 294, 11, 255, 255, 255);
addText("Main_ResTxt_0", "\u7269\u7406\u51cf\u4f24", 9, 319, 9, 255, 255, 255);
addText("Main_ResTxt_1", "\u706b\u7130\u6297\u6027", 106, 319, 9, 255, 255, 255);
addText("Main_ResTxt_2", "\u51b0\u51b7\u6297\u6027", 203, 319, 9, 255, 255, 255);
addText("Main_ResTxt_3", "\u95ea\u7535\u6297\u6027", 9, 354, 9, 255, 255, 255);
addText("Main_ResTxt_4", "\u6bd2\u7d20\u6297\u6027", 106, 354, 9, 255, 255, 255);
addText("Main_ResTxt_5", "\u795e\u5723\u6297\u6027", 203, 354, 9, 255, 255, 255);
addText("Detail_AtkTxt", "\u7269\u7406\u653b\u51fb\u529b", 315, 25, 11, 255, 255, 255);
addText("Detail_LTxt_0", "\u9b54\u6cd5\u653b\u51fb\u529b", 315, 43, 11, 255, 255, 255);
addText("Detail_RTxt_0", "\u4f24\u5bb3", 481, 43, 11, 255, 255, 255);
addText("Detail_LTxt_1", "\u7269\u7406\u9632\u5fa1\u529b", 315, 61, 11, 255, 255, 255);
addText("Detail_RTxt_1", "\u989d\u5916\u4f24\u5bb3", 481, 61, 11, 255, 255, 255);
addText("Detail_LTxt_2", "\u9b54\u6cd5\u9632\u5fa1\u529b", 315, 79, 11, 255, 255, 255);
addText("Detail_RTxt_2", "\u6700\u7ec8\u4f24\u5bb3", 481, 79, 11, 255, 255, 255);
addText("Detail_LTxt_3", "\u547d\u4e2d\u7387", 315, 97, 11, 255, 255, 255);
addText("Detail_RTxt_3", "\u9996\u9886\u4f24\u5bb3", 481, 97, 11, 255, 255, 255);
addText("Detail_LTxt_4", "\u56de\u907f\u7387", 315, 115, 11, 255, 255, 255);
addText("Detail_RTxt_4", "\u65e0\u89c6\u9632\u5fa1\u7387", 481, 115, 11, 255, 255, 255);
addText("Detail_LTxt_5", "\u624b\u6280", 315, 133, 11, 255, 255, 255);
addText("Detail_RTxt_5", "\u72b6\u6001\u5f02\u5e38\u6297\u6027", 481, 133, 11, 255, 255, 255);
addText("Detail_LTxt_6", "\u653b\u51fb\u901f\u5ea6", 315, 151, 11, 255, 255, 255);
addText("Detail_RTxt_6", "\u589e\u76ca\u6301\u7eed\u65f6\u95f4", 481, 151, 11, 255, 255, 255);
addText("Detail_LTxt_7", "\u65bd\u6cd5\u901f\u5ea6", 315, 169, 11, 255, 255, 255);
addText("Detail_RTxt_7", "\u7a33\u5982\u6cf0\u5c71", 481, 169, 11, 255, 255, 255);
addText("Detail_LTxt_8", "\u79fb\u52a8\u901f\u5ea6", 315, 187, 11, 255, 255, 255);
addText("Detail_RTxt_8", "\u9053\u5177\u6389\u843d\u7387", 481, 187, 11, 255, 255, 255);
addText("Detail_LTxt_9", "\u8df3\u8dc3\u529b", 315, 205, 11, 255, 255, 255);
addText("Detail_RTxt_9", "\u91d1\u5e01\u83b7\u5f97\u91cf", 481, 205, 11, 255, 255, 255);
addText("Detail_LTxt_10", "\u66b4\u51fb\u51e0\u7387", 315, 223, 11, 255, 255, 255);
addText("Detail_RTxt_10", "\u8363\u8a89", 481, 223, 11, 255, 255, 255);
addText("Detail_LTxt_11", "\u66b4\u51fb\u4f24\u5bb3", 315, 241, 11, 255, 255, 255);
addText("Detail_RTxt_11", "\u907f\u514d\u4e2d\u6bd2", 481, 241, 11, 255, 255, 255);
addText("Detail_LTxt_12", "\u683c\u6321\u7387", 315, 259, 11, 255, 255, 255);
addText("Detail_RTxt_12", "\u907f\u514d\u81f4\u76f2", 481, 259, 11, 255, 255, 255);
addText("Detail_LTxt_13", "HP\u6062\u590d", 315, 277, 11, 255, 255, 255);
addText("Detail_RTxt_13", "\u907f\u514d\u6655\u7729", 481, 277, 11, 255, 255, 255);
addText("Detail_LTxt_14", "MP\u6062\u590d", 315, 295, 11, 255, 255, 255);
addText("Detail_RTxt_14", "\u8bc5\u5492\u51cf\u514d", 481, 295, 11, 255, 255, 255);
addText("Detail_LTxt_15", "\u907f\u514d\u6d41\u8840", 315, 313, 11, 255, 255, 255);
addText("Detail_RTxt_15", "\u706b\u7130\u6297\u6027", 481, 313, 11, 255, 255, 255);
addText("Detail_LTxt_16", "\u907f\u514d\u70b9\u71c3", 315, 331, 11, 255, 255, 255);
addText("Detail_RTxt_16", "\u51b0\u51b7\u6297\u6027", 481, 331, 11, 255, 255, 255);
addText("Detail_LTxt_17", "\u907f\u514d\u51b0\u7f13", 315, 349, 11, 255, 255, 255);
addText("Detail_RTxt_17", "\u95ea\u7535\u6297\u6027", 481, 349, 11, 255, 255, 255);
addText("Detail_LTxt_18", "\u907f\u514d\u611f\u7535", 315, 367, 11, 255, 255, 255);
addText("Detail_RTxt_18", "\u795e\u5723\u6297\u6027", 481, 367, 11, 255, 255, 255);

function findLayer(name) {
  var doc = app.activeDocument;
  var i;
  for (i = 0; i < doc.artLayers.length; i++) {
    if (doc.artLayers[i].name === name) return doc.artLayers[i];
  }
  return null;
}
function moveList(names, group) {
  var i, L;
  for (i = 0; i < names.length; i++) {
    L = findLayer(names[i]);
    if (L) L.move(group, ElementPlacement.INSIDE);
  }
}

var gMat = ensureGroup('Materials');
var gFinal = ensureGroup('Final');
var gMain = gFinal.layerSets.add(); gMain.name = 'Final_Main';
var gDetail = gFinal.layerSets.add(); gDetail.name = 'Final_Detail';
var gTexts = gFinal.layerSets.add(); gTexts.name = 'Final_Texts';
moveList(["mat_label_pink", "mat_label_green", "mat_label_resist", "mat_label_combat", "mat_label_dmg", "mat_label_crit_rate", "mat_label_crit_dmg", "mat_label_final", "mat_label_boss", "mat_label_ignore", "mat_label_status", "mat_label_buff", "mat_label_stance", "mat_label_drop", "mat_label_meso", "mat_label_honor", "mat_label_ailment", "mat_value_80", "mat_value_240", "mat_value_left", "mat_resist_label", "mat_resist_value", "mat_panel_main", "mat_panel_detail"], gMat);
moveList(["Main_PanelBg", "Main_IdChip_0", "Main_IdVal_0", "Main_IdChip_1", "Main_IdVal_1", "Main_IdChip_2", "Main_IdVal_2", "Main_IdChip_3", "Main_IdVal_3", "Main_IdChip_4", "Main_IdVal_4", "Main_IdChip_5", "Main_IdVal_5", "Main_IdChip_6", "Main_IdVal_6", "Main_IdChip_7", "Main_IdVal_7", "Main_IdChip_8", "Main_IdVal_8", "Main_IdChip_9", "Main_IdVal_9", "Main_IdChip_10", "Main_IdVal_10", "Main_CoreChip_0", "Main_CoreVal_0", "Main_CoreChip_1", "Main_CoreVal_1", "Main_CoreChip_2", "Main_CoreVal_2", "Main_CoreChip_3", "Main_CoreVal_3", "Main_ResLbl_0", "Main_ResVal_0", "Main_ResLbl_1", "Main_ResVal_1", "Main_ResLbl_2", "Main_ResVal_2", "Main_ResLbl_3", "Main_ResVal_3", "Main_ResLbl_4", "Main_ResVal_4", "Main_ResLbl_5", "Main_ResVal_5"], gMain);
moveList(["Detail_PanelBg", "Detail_AtkChip", "Detail_AtkVal", "Detail_LChip_0", "Detail_LVal_0", "Detail_RChip_0", "Detail_RVal_0", "Detail_LChip_1", "Detail_LVal_1", "Detail_RChip_1", "Detail_RVal_1", "Detail_LChip_2", "Detail_LVal_2", "Detail_RChip_2", "Detail_RVal_2", "Detail_LChip_3", "Detail_LVal_3", "Detail_RChip_3", "Detail_RVal_3", "Detail_LChip_4", "Detail_LVal_4", "Detail_RChip_4", "Detail_RVal_4", "Detail_LChip_5", "Detail_LVal_5", "Detail_RChip_5", "Detail_RVal_5", "Detail_LChip_6", "Detail_LVal_6", "Detail_RChip_6", "Detail_RVal_6", "Detail_LChip_7", "Detail_LVal_7", "Detail_RChip_7", "Detail_RVal_7", "Detail_LChip_8", "Detail_LVal_8", "Detail_RChip_8", "Detail_RVal_8", "Detail_LChip_9", "Detail_LVal_9", "Detail_RChip_9", "Detail_RVal_9", "Detail_LChip_10", "Detail_LVal_10", "Detail_RChip_10", "Detail_RVal_10", "Detail_LChip_11", "Detail_LVal_11", "Detail_RChip_11", "Detail_RVal_11", "Detail_LChip_12", "Detail_LVal_12", "Detail_RChip_12", "Detail_RVal_12", "Detail_LChip_13", "Detail_LVal_13", "Detail_RChip_13", "Detail_RVal_13", "Detail_LChip_14", "Detail_LVal_14", "Detail_RChip_14", "Detail_RVal_14", "Detail_LChip_15", "Detail_LVal_15", "Detail_RChip_15", "Detail_RVal_15", "Detail_LChip_16", "Detail_LVal_16", "Detail_RChip_16", "Detail_RVal_16", "Detail_LChip_17", "Detail_LVal_17", "Detail_RChip_17", "Detail_RVal_17", "Detail_LChip_18", "Detail_LVal_18", "Detail_RChip_18", "Detail_RVal_18"], gDetail);
moveList(["Main_Title", "Detail_Title", "Main_IdTxt_0", "Main_IdTxt_1", "Main_IdTxt_2", "Main_IdTxt_3", "Main_IdTxt_4", "Main_IdTxt_5", "Main_IdTxt_6", "Main_IdTxt_7", "Main_IdTxt_8", "Main_IdTxt_9", "Main_IdTxt_10", "Main_CoreTxt_0", "Main_CoreTxt_1", "Main_CoreTxt_2", "Main_CoreTxt_3", "Main_ResTxt_0", "Main_ResTxt_1", "Main_ResTxt_2", "Main_ResTxt_3", "Main_ResTxt_4", "Main_ResTxt_5", "Detail_AtkTxt", "Detail_LTxt_0", "Detail_RTxt_0", "Detail_LTxt_1", "Detail_RTxt_1", "Detail_LTxt_2", "Detail_RTxt_2", "Detail_LTxt_3", "Detail_RTxt_3", "Detail_LTxt_4", "Detail_RTxt_4", "Detail_LTxt_5", "Detail_RTxt_5", "Detail_LTxt_6", "Detail_RTxt_6", "Detail_LTxt_7", "Detail_RTxt_7", "Detail_LTxt_8", "Detail_RTxt_8", "Detail_LTxt_9", "Detail_RTxt_9", "Detail_LTxt_10", "Detail_RTxt_10", "Detail_LTxt_11", "Detail_RTxt_11", "Detail_LTxt_12", "Detail_RTxt_12", "Detail_LTxt_13", "Detail_RTxt_13", "Detail_LTxt_14", "Detail_RTxt_14", "Detail_LTxt_15", "Detail_RTxt_15", "Detail_LTxt_16", "Detail_RTxt_16", "Detail_LTxt_17", "Detail_RTxt_17", "Detail_LTxt_18", "Detail_RTxt_18"], gTexts);

var outFile = new File(OUT_PSD);
var opts = new PhotoshopSaveOptions();
opts.embedColorProfile = true;
opts.alphaChannels = true;
opts.layers = true;
app.activeDocument.saveAs(outFile, opts, true, Extension.LOWERCASE);

var __v4Result = { ok: true, path: OUT_PSD, places: 122, texts: 62, components: 24, canvas: [W, H] };
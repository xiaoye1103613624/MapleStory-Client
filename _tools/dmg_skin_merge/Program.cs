// DamageSkin.img resource port (MapleLib-based).
//
// orange-wz MCP cannot read these classic .img files (returns empty children /
// "Index N out of bounds" — see memory 17387439), so we use MapleLib directly.
// Each standalone .img is a raw imgdir stream starting with 0x73 ("Property"),
// which WzImage.SaveImage reproduces exactly, so SaveImage's output IS the file.
//
// Cipher: current end is GMS-AES (MapleLib GMS reads it to 624 numeric skins).
// Sources: 20dalu_079MS = EMS, MapleRoot = GMS, BeiDou_S9 = GMS.
// We read each with its key, deep-clone missing skins (cipher-agnostic in
// memory), and re-save with GMS so the client can load it.
//
// Conservative default: only ADD skin ids absent from the current end. Skins
// present in both but richer in a source are reported as enhancement
// candidates and NOT added (per plan).

using MapleLib.WzLib;
using MapleLib.WzLib.Util;
using MapleLib.WzLib.WzProperties;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

string curPath = @"E:\MXD\BeiDou-Client-xy\Data\Effect\DamageSkin.img";
string outPath = @"E:\MXD\BeiDou-Client-xy\Data\Effect\DamageSkin.merged.img";

var sources = new (string path, WzMapleVersion ver, string tag)[]
{
    (@"E:\MXD\20dalu_079MS\客户端\Data\Effect\DamageSkin.img", WzMapleVersion.EMS, "20dalu_079MS"),
    (@"E:\MXD\MapleRoot Full Repack\Game Files\Data\Effect\DamageSkin.img", WzMapleVersion.GMS, "MapleRoot"),
    (@"E:\MXD\BeiDou-Client_S9\Data\Effect\DamageSkin.img", WzMapleVersion.GMS, "BeiDou_S9"),
};

WzImage Load(string path, WzMapleVersion ver, string tag)
{
    var ms = new MemoryStream(File.ReadAllBytes(path));
    var img = new WzImage(Path.GetFileName(path), ms, ver);
    if (!img.ParseImage())
        Console.WriteLine($"  [{tag}] WARN ParseImage returned false");
    return img;
}

int CountAll(WzImageProperty p)
{
    int n = 1;
    var children = p?.WzProperties;
    if (children != null)
        foreach (var c in children)
            if (c != null) n += CountAll(c);
    return n;
}

HashSet<string> SkinIds(WzImage img) =>
    new(img.WzProperties.Where(p => int.TryParse(p.Name, out _)).Select(p => p.Name));

var report = new List<string>();
report.Add($"# DamageSkin.img port report ({DateTime.Now:yyyy-MM-dd HH:mm})");
report.Add($"Current end : {curPath}");
report.Add("");

var cur = Load(curPath, WzMapleVersion.GMS, "CURRENT");
var curIds = SkinIds(cur);
Console.WriteLine($"CURRENT skins = {curIds.Count}");
report.Add($"CURRENT skins = {curIds.Count}");

int totalAdded = 0;
var addedAll = new List<string>();
var enhAll = new List<string>();
var srcImages = new List<WzImage>();

foreach (var (path, ver, tag) in sources)
{
    var src = Load(path, ver, tag);
    srcImages.Add(src);
    var srcIds = SkinIds(src);
    var missing = srcIds.Except(curIds).OrderBy(x => int.Parse(x)).ToList();
    var present = srcIds.Intersect(curIds).ToList();

    Console.WriteLine($"[{tag}] ver={ver} skins={srcIds.Count} missing-from-current={missing.Count}");
    report.Add("");
    report.Add($"## Source: {tag} (ver={ver}, skins={srcIds.Count})");
    report.Add($"  Missing in current (to ADD): {missing.Count}");
    foreach (var id in missing)
    {
        var sp = src.WzProperties.First(p => p.Name == id);
        var clone = sp.DeepClone();
        cur.AddProperty(clone);
        curIds.Add(id);
        addedAll.Add(id);
        totalAdded++;
    }
    report.Add("    " + (missing.Count > 0 ? string.Join(", ", missing) : "(none)"));

    // Enhancement candidates: same id, source has strictly more total nodes.
    var enh = new List<string>();
    foreach (var id in present)
    {
        var curNode = cur.WzProperties.First(p => p.Name == id);
        var srcNode = src.WzProperties.First(p => p.Name == id);
        int cN = CountAll(curNode), sN = CountAll(srcNode);
        if (sN > cN) enh.Add($"{id} (cur={cN} src={sN})");
    }
    report.Add($"  Enhancement candidates (NOT added): {enh.Count}");
    foreach (var e in enh) report.Add("    " + e);
    enhAll.AddRange(enh.Select(e => $"[{tag}] {e}"));

    Console.WriteLine($"  -> added {missing.Count} missing skins from {tag}");
}

Console.WriteLine($"\nTOTAL ADDED = {totalAdded}");
report.Add("");
report.Add($"# Summary");
report.Add($"Total skins added: {totalAdded}");
report.Add($"Final current-end skin count: {curIds.Count} (was {cur.WzProperties.Count - totalAdded})");
report.Add($"Enhancement candidates (skipped): {enhAll.Count}");

// Save with GMS key (matches what the client loads).
byte[] iv = WzTool.GetIvByMapleVersion(WzMapleVersion.GMS);
cur.Changed = true;
using (var fs = new FileStream(outPath, FileMode.Create, FileAccess.Write))
using (var writer = new WzBinaryWriter(fs, iv))
{
    cur.SaveImage(writer, true, true);
}
long outLen = new FileInfo(outPath).Length;
Console.WriteLine($"SAVED {outPath} ({outLen} bytes)");
report.Add($"Merged file: {outPath} ({outLen} bytes)");

// Validate by re-reading.
var re = Load(outPath, WzMapleVersion.GMS, "MERGED");
var reIds = SkinIds(re);
bool ok = reIds.Count == curIds.Count;
Console.WriteLine($"RE-READ MERGED skins = {reIds.Count} (expected {curIds.Count}) -> {(ok ? "OK" : "MISMATCH")}");
report.Add($"Re-read validation: {reIds.Count} skins -> {(ok ? "OK" : "MISMATCH")}");

// Header + pixel integrity check for the added skins.
byte firstByte = File.ReadAllBytes(outPath)[0];
bool headerOk = firstByte == 0x73;
Console.WriteLine($"First byte = 0x{firstByte:X2} (expect 0x73) -> {(headerOk ? "OK" : "BAD")}");
report.Add($"File header byte = 0x{firstByte:X2} -> {(headerOk ? "OK" : "BAD")}");

// For each added skin, verify it contains at least one canvas with real pixels.
int badSkins = 0;
var canvasReport = new List<string>();
foreach (var id in addedAll)
{
    var node = re.WzProperties.FirstOrDefault(p => p.Name == id);
    if (node == null) { badSkins++; canvasReport.Add($"{id}: MISSING AFTER RELOAD"); continue; }
    int canvases = 0, zeroDim = 0;
    CollectCanvases(node, ref canvases, ref zeroDim);
    if (canvases == 0 || zeroDim > 0)
    {
        badSkins++;
        canvasReport.Add($"{id}: canvases={canvases} zeroDim={zeroDim}");
    }
}
void CollectCanvases(WzImageProperty p, ref int canvases, ref int zeroDim)
{
    if (p == null) return;
    if (p is WzCanvasProperty cv)
    {
        canvases++;
        try
        {
            var png = cv.PngProperty;
            if (png == null || png.Width <= 0 || png.Height <= 0) zeroDim++;
        }
        catch { zeroDim++; }
    }
    var children = p.WzProperties;
    if (children != null)
        foreach (var c in children)
            if (c != null) CollectCanvases(c, ref canvases, ref zeroDim);
}
Console.WriteLine($"Added-skin pixel check: {addedAll.Count - badSkins}/{addedAll.Count} OK, {badSkins} suspicious");
report.Add($"Added-skin pixel integrity: {addedAll.Count - badSkins}/{addedAll.Count} OK, {badSkins} suspicious");
foreach (var c in canvasReport) report.Add("  " + c);
bool pixelOk = badSkins == 0;
ok = ok && headerOk && pixelOk;

report.Add("");
report.Add("Added skin ids (sorted): " + string.Join(", ", addedAll.OrderBy(x => int.Parse(x))));

foreach (var img in srcImages) img.Dispose();
cur.Dispose();
re.Dispose();

string reportPath = Path.Combine(
    Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) ?? ".",
    "merge-report.txt");
File.WriteAllLines(reportPath, report);
Console.WriteLine($"\nREPORT -> {reportPath}");
Console.WriteLine(ok ? "VALIDATION OK" : "VALIDATION MISMATCH");

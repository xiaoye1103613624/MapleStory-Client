using System.Drawing.Imaging;
using MapleLib.WzLib;
using MapleLib.WzLib.WzProperties;

var outDir = args.Length > 1
    ? args[1]
    : @"E:\project\MapleStory-Client\docs\features\char-stats-ui-ps\source";
var wzPath = args.Length > 0
    ? args[0]
    : @"E:\project\sdlMS\build\Data2\UI.wz";

Directory.CreateDirectory(outDir);

Console.WriteLine($"Loading {wzPath}");
using var wz = new WzFile(wzPath, WzMapleVersion.GMS);
Console.WriteLine($"Parse status: {wz.ParseWzFile()}");

var exports = new (string path, string file)[]
{
    // Modern (UIWindow2)
    ("UIWindow2.img/Stat/main/backgrnd", "w2_main_backgrnd"),
    ("UIWindow2.img/Stat/main/backgrnd2", "w2_main_backgrnd2"),
    ("UIWindow2.img/Stat/main/backgrnd3", "w2_main_backgrnd3"),
    ("UIWindow2.img/Stat/main/cover0", "w2_main_cover0"),
    ("UIWindow2.img/Stat/main/cover1", "w2_main_cover1"),
    ("UIWindow2.img/Stat/main/BtDetailOpen/normal/0", "w2_BtDetailOpen"),
    ("UIWindow2.img/Stat/main/BtDetailClose/normal/0", "w2_BtDetailClose"),
    ("UIWindow2.img/Stat/detail/backgrnd", "w2_detail_backgrnd"),
    ("UIWindow2.img/Stat/detail/backgrnd2", "w2_detail_backgrnd2"),
    // Classic fallback
    ("UIWindow.img/Stat/backgrnd", "w1_backgrnd"),
    ("UIWindow.img/Stat/backgrnd2", "w1_backgrnd2"),
    ("UIWindow.img/Stat/backgrnd3", "w1_backgrnd3"),
    ("UIWindow.img/Stat/backgrnd4", "w1_backgrnd4"),
    ("UIWindow.img/Stat/basicStat", "w1_basicStat"),
    ("UIWindow.img/Stat/BtDetail/normal/0", "w1_BtDetail"),
    ("Basic.img/BtClose3/normal/0", "BtClose3"),
    ("Basic.img/BtClose2/normal/0", "BtClose2"),
};

int ok = 0;
foreach (var (path, file) in exports)
{
    WzObject? obj = wz.GetObjectFromPath(path, checkFirstDirectoryName: false)
                 ?? wz.GetObjectFromPath(path, checkFirstDirectoryName: true);

    if (obj is not WzCanvasProperty c)
    {
        Console.WriteLine($"MISSING {path}");
        continue;
    }

    using var bmp = c.GetBitmap();
    if (bmp is null)
    {
        Console.WriteLine($"NOBITMAP {path}");
        continue;
    }

    var outPath = Path.Combine(outDir, file + ".png");
    bmp.Save(outPath, ImageFormat.Png);
    Console.WriteLine($"OK {file} {bmp.Width}x{bmp.Height}");
    ok++;
}

Console.WriteLine($"Exported {ok}/{exports.Length} -> {outDir}");
return ok > 0 ? 0 : 2;

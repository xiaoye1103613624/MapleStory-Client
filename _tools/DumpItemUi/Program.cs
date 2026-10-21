using System.Drawing.Imaging;
using MapleLib.WzLib;
using MapleLib.WzLib.WzProperties;

var outDir = args.Length > 1
    ? args[1]
    : @"E:\project\MapleStory-Client\docs\features\inventory-ui-ps\source";
var wzPath = args.Length > 0
    ? args[0]
    : @"E:\project\sdlMS\build\Data2\UI.wz";

Directory.CreateDirectory(outDir);

Console.WriteLine($"Loading {wzPath}");
using var wz = new WzFile(wzPath, WzMapleVersion.GMS);
var status = wz.ParseWzFile();
Console.WriteLine($"Parse status: {status}");

var exports = new (string path, string file)[]
{
    ("UIWindow.img/Item/FullBackgrnd", "FullBackgrnd"),
    ("UIWindow.img/Item/backgrnd", "backgrnd"),
    ("UIWindow.img/Item/disabled", "disabled"),
    ("UIWindow.img/Item/activeIcon", "activeIcon"),
    ("UIWindow.img/Item/Tab/enabled/0", "Tab_enabled_0"),
    ("UIWindow.img/Item/Tab/enabled/1", "Tab_enabled_1"),
    ("UIWindow.img/Item/Tab/enabled/2", "Tab_enabled_2"),
    ("UIWindow.img/Item/Tab/enabled/3", "Tab_enabled_3"),
    ("UIWindow.img/Item/Tab/enabled/4", "Tab_enabled_4"),
    ("UIWindow.img/Item/Tab/disabled/0", "Tab_disabled_0"),
    ("UIWindow.img/Item/Tab/disabled/1", "Tab_disabled_1"),
    ("UIWindow.img/Item/Tab/disabled/2", "Tab_disabled_2"),
    ("UIWindow.img/Item/Tab/disabled/3", "Tab_disabled_3"),
    ("UIWindow.img/Item/Tab/disabled/4", "Tab_disabled_4"),
    ("UIWindow.img/Item/BtCoin/normal/0", "BtCoin"),
    ("UIWindow.img/Item/BtGather/normal/0", "BtGather"),
    ("UIWindow.img/Item/BtSort/normal/0", "BtSort"),
    ("UIWindow.img/Item/BtSmall/normal/0", "BtSmall"),
    ("UIWindow.img/Item/BtFull/normal/0", "BtFull"),
    ("UIWindow.img/Item/BtCashshop/normal/0", "BtCashshop"),
    ("Basic.img/BtClose3/normal/0", "BtClose3"),
    ("Basic.img/BtClose2/normal/0", "BtClose2"),
};

int ok = 0;
foreach (var (path, file) in exports)
{
    WzObject? obj = wz.GetObjectFromPath(path, checkFirstDirectoryName: false);
    if (obj is not WzCanvasProperty)
        obj = wz.GetObjectFromPath(path, checkFirstDirectoryName: true);

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
    Console.WriteLine($"OK {file} {bmp.Width}x{bmp.Height} -> {outPath}");
    ok++;
}

Console.WriteLine($"Exported {ok}/{exports.Length}");
return ok > 0 ? 0 : 2;

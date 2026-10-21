using System.Drawing.Imaging;
using MapleLib.WzLib;
using MapleLib.WzLib.WzProperties;

var wzPath = args.Length > 0 ? args[0] : @"E:\project\sdlMS\build\Data2\UI.wz";
var borderOut = @"E:\project\MapleStory-Client\docs\features\map-mob-drops\resources\source\UIWindow_WorldMap";
var tabOut = @"E:\project\MapleStory-Client\docs\features\map-search-ui\resources\source\ItemTab";
Directory.CreateDirectory(borderOut);
Directory.CreateDirectory(tabOut);

using var wz = new WzFile(wzPath, WzMapleVersion.GMS);
Console.WriteLine(wz.ParseWzFile());

void SaveCanvas(WzCanvasProperty c, string path)
{
	using var bmp = c.GetBitmap();
	if (bmp is null) return;
	Directory.CreateDirectory(Path.GetDirectoryName(path)!);
	bmp.Save(path, ImageFormat.Png);
	Console.WriteLine($"OK {bmp.Width}x{bmp.Height} {path}");
}

var ui1 = (WzImage)wz.WzDirectory["UIWindow.img"];
ui1.ParseImage();
var border = (WzSubProperty)((WzSubProperty)ui1["WorldMap"])["Border"];
foreach (var child in border.WzProperties)
{
	if (child is WzCanvasProperty c)
		SaveCanvas(c, Path.Combine(borderOut, "WorldMap_Border_" + child.Name + ".png"));
}

WzObject? item = ui1["Item"];
if (item is null)
{
	var ui2 = wz.WzDirectory["UIWindow2.img"] as WzImage;
	ui2?.ParseImage();
	item = ui2?["Item"];
}
if (item is WzSubProperty itemSub)
{
	foreach (var state in new[] { "enabled", "disabled" })
	{
		if (itemSub["Tab"]?[state] is not WzSubProperty tabs)
			continue;
		foreach (var t in tabs.WzProperties)
		{
			if (t is WzCanvasProperty c)
				SaveCanvas(c, Path.Combine(tabOut, $"Tab_{state}_{t.Name}.png"));
		}
	}
}

// Basic close / sample buttons for size reference
if (wz.WzDirectory["Basic.img"] is WzImage basic)
{
	basic.ParseImage();
	foreach (var name in new[] { "BtClose3", "BtClose2" })
	{
		if (basic[name] is not WzSubProperty btn) continue;
		foreach (var st in new[] { "normal", "mouseOver", "pressed", "disabled" })
		{
			if (btn[st]?["0"] is WzCanvasProperty c)
				SaveCanvas(c, Path.Combine(tabOut, $"..", "Basic", $"{name}_{st}.png"));
			else if (btn[st] is WzCanvasProperty c2)
				SaveCanvas(c2, Path.Combine(tabOut, $"..", "Basic", $"{name}_{st}.png"));
		}
	}
}

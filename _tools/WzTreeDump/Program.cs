// Dump every image in a .wz file to its own JSON (structure only, no base64)
// using MapleLib's WzJsonBsonSerializer. Gives the full node tree: names,
// types, canvas dimensions, vector coords, property values.
using MapleLib.WzLib;
using MapleLib.WzLib.Serializer;

string wzPath = args.Length > 0 ? args[0] : @"E:\project\sdlMS\build\Data2\UI.wz";
string outDir = args.Length > 1 ? args[1] : @"C:\Users\11036\.rea\wzdump";
var filter = args.Length > 2 ? args[2].Split(',') : Array.Empty<string>();

Directory.CreateDirectory(outDir);
Console.WriteLine($"Loading {wzPath}");
using var wz = new WzFile(wzPath, WzMapleVersion.GMS);
Console.WriteLine($"Parse status: {wz.ParseWzFile()}");

var ser = new WzJsonBsonSerializer(1, LineBreak.Unix, false, true);

int dumped = 0;
long totalBytes = 0;
void DumpDir(WzDirectory dir, string prefix)
{
    foreach (var img in dir.WzImages)
    {
        if (filter.Length > 0 && !filter.Any(f => img.Name.Contains(f, StringComparison.OrdinalIgnoreCase)))
            continue;
        string safe = prefix + img.Name.Replace('.', '_').Replace('/', '_').Replace('\\', '_');
        string outPath = Path.Combine(outDir, safe + ".json");
        ser.SerializeImage(img, outPath);
        var fi = new FileInfo(outPath);
        totalBytes += fi.Length;
        Console.WriteLine($"OK {img.Name} -> {safe}.json ({fi.Length/1024} KB)");
        dumped++;
    }
    foreach (var sub in dir.WzDirectories)
        DumpDir(sub, prefix + sub.Name + "_");
}

DumpDir(wz.WzDirectory, "");
Console.WriteLine($"Dumped {dumped} images, {totalBytes/1024/1024} MB -> {outDir}");
return 0;

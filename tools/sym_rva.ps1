param(
    [Parameter(Mandatory=$true)][string]$Exe,
    [Parameter(Mandatory=$true)][long[]]$Rvas
)
$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public static class Sym {
    [DllImport("kernel32.dll")] public static extern IntPtr GetCurrentProcess();
    [DllImport("dbghelp.dll", CharSet=CharSet.Unicode)]
    public static extern bool SymInitialize(IntPtr h, string u, bool invade);
    [DllImport("dbghelp.dll", CharSet=CharSet.Unicode)]
    public static extern ulong SymLoadModuleEx(IntPtr h, IntPtr f, string name, string url, ulong baseOf, uint size, IntPtr data, uint flags);
    [DllImport("dbghelp.dll", CharSet=CharSet.Ansi)]
    public static extern bool SymFromAddr(IntPtr h, ulong addr, out ulong disp, ref SYMBOL_INFO si);
    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
    public struct SYMBOL_INFO {
        public uint SizeOfStruct; public uint TypeIndex; public ulong Reserved0; public ulong Reserved1;
        public uint Index; public uint Size; public ulong ModBase; public uint Flags; public ulong Value;
        public ulong Address; public uint Register; public uint Scope; public uint Tag; public uint NameLen; public uint MaxNameLen;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst=1024)] public string Name;
    }
}
"@
$h = [Sym]::GetCurrentProcess()
if (-not [Sym]::SymInitialize($h, $null, $false)) { throw "SymInitialize failed" }
$base = [Sym]::SymLoadModuleEx($h, [IntPtr]::Zero, $Exe, $null, 0x140000000, 0, [IntPtr]::Zero, 0)
if ($base -eq 0) { throw "SymLoadModuleEx failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())" }
foreach ($rva in $Rvas) {
    $si = New-Object Sym+SYMBOL_INFO
    $si.SizeOfStruct = [uint32]([Runtime.InteropServices.Marshal]::SizeOf([type][Sym+SYMBOL_INFO]) - 1024)
    $si.MaxNameLen = 1024
    $disp = [UInt64]0
    $addr = [uint64]($base + [uint64]$rva)
    if ([Sym]::SymFromAddr($h, $addr, [ref]$disp, [ref]$si)) {
        "{0:x} = {1} + 0x{2:x} (size 0x{3:x})" -f $rva, $si.Name, $disp, $si.Size
    } else {
        "{0:x} = ? (Win32 {1})" -f $rva, [Runtime.InteropServices.Marshal]::GetLastWin32Error()
    }
}

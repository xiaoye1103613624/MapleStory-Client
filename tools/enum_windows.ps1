param([int]$TargetPid)
$sig = @'
[DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc cb, IntPtr l);
public delegate bool EnumWindowsProc(IntPtr h, IntPtr l);
[DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
[DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
[DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, System.Text.StringBuilder t, int n);
'@
Add-Type -MemberDefinition $sig -Name W -Namespace EnumWin
$found = @()
$cb = [EnumWin.W+EnumWindowsProc]{
    param($h, $l)
    [uint32]$wpid = 0
    [void][EnumWin.W]::GetWindowThreadProcessId($h, [ref]$wpid)
    if ($wpid -eq $TargetPid) {
        $sb = New-Object System.Text.StringBuilder 256
        [void][EnumWin.W]::GetWindowText($h, $sb, 256)
        $script:found += "HWND=$h vis=$([EnumWin.W]::IsWindowVisible($h)) title='$($sb.ToString())'"
    }
    return $true
}
[void][EnumWin.W]::EnumWindows($cb, [IntPtr]::Zero)
if ($found.Count -eq 0) { "NO TOP-LEVEL WINDOWS for pid $TargetPid" } else { $found }

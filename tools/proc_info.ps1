$p = Get-Process -Name MapleStory -ErrorAction SilentlyContinue | Select-Object -First 1
if ($p) {
    "PID=$($p.Id) START=$($p.StartTime) TITLE='$($p.MainWindowTitle)' HWND=$($p.MainWindowHandle) RESP=$($p.Responding)"
} else {
    'NO PROC'
}

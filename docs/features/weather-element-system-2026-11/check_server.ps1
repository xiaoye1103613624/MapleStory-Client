Get-CimInstance Win32_Process -Filter "Name='java.exe'" | ForEach-Object {
    $cl = $_.CommandLine
    if ($cl -and $cl.Length -gt 160) { $cl = $cl.Substring(0, 160) }
    [PSCustomObject]@{
        Pid  = $_.ProcessId
        Since = $_.CreationDate
        Cmd  = $cl
    }
} | Format-List

Write-Output '--- listening 8686 ---'
Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue |
  Where-Object { $_.LocalPort -eq 8686 } |
  Select-Object LocalAddress, LocalPort, OwningProcess | Format-Table -AutoSize

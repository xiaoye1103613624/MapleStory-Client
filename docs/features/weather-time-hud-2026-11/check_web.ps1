Write-Output '--- ports 8686/8787 ---'
Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue |
  Where-Object { $_.LocalPort -eq 8686 -or $_.LocalPort -eq 8787 } |
  Select-Object LocalAddress, LocalPort, OwningProcess | Format-Table -AutoSize

Write-Output '--- node/java procs (cmd trimmed) ---'
Get-CimInstance Win32_Process |
  Where-Object { $_.Name -match '^(node|java|cmd|powershell)(.exe)?$' } |
  ForEach-Object {
    $c = $_.CommandLine
    if ($c -and $c.Length -gt 130) { $c = $c.Substring(0, 130) }
    [PSCustomObject]@{ Pid = $_.ProcessId; Name = $_.Name; Cmd = $c }
  } | Format-Table -AutoSize -Wrap

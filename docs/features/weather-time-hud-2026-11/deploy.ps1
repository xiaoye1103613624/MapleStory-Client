$ErrorActionPreference = 'Stop'
$live = 'E:\MXD\BeiDou-Client-xy\MapleStory.exe'
$src  = 'E:\project\MapleStory-Client\build-weather-time-hud-2026-11\MapleStory.exe'

Write-Output '--- stop PID 22668 ---'
Stop-Process -Id 22668 -Force -ErrorAction SilentlyContinue
Start-Sleep -Milliseconds 1000
$p = Get-Process -Id 22668 -ErrorAction SilentlyContinue
if ($p) { Write-Output 'PID 22668 STILL RUNNING' } else { Write-Output 'PID 22668 STOPPED' }

Write-Output '--- remaining live holders ---'
$holders = Get-CimInstance Win32_Process -Filter "Name='MapleStory.exe'" | Where-Object { $_.ExecutablePath -like '*BeiDou-Client-xy*' }
$holders | Select-Object ProcessId, ExecutablePath | Format-Table -AutoSize
if ($holders) { Write-Output 'ABORT: live exe still held'; exit 2 }

Write-Output '--- backup live ---'
$bak = "$live.bak-before-weathertime-20261123"
Copy-Item -LiteralPath $live -Destination $bak -Force
Write-Output ("backup sha=" + (Get-FileHash $bak -Algorithm SHA256).Hash)

Write-Output '--- overwrite live ---'
Copy-Item -LiteralPath $src -Destination $live -Force
$sha1 = (Get-FileHash $live -Algorithm SHA256).Hash
Write-Output ("live sha immediately=" + $sha1)
Write-Output ("expected            =" + (Get-FileHash $src -Algorithm SHA256).Hash)

Write-Output '--- anti-rollback wait 8s ---'
Start-Sleep -Seconds 8
$sha2 = (Get-FileHash $live -Algorithm SHA256).Hash
Write-Output ("live sha after 8s   =" + $sha2)
if ($sha2 -ne $sha1) { Write-Output 'WARN: live sha changed after 8s (rollback?)' } else { Write-Output 'OK: stable' }

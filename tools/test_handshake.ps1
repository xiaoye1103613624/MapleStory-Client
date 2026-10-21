$c = New-Object Net.Sockets.TcpClient
$c.Connect('127.0.0.1', 8484)
$s = $c.GetStream()
$s.ReadTimeout = 4000
$b = New-Object byte[] 16
try {
    $n = $s.Read($b, 0, 16)
    "HANDSHAKE bytes=$n hex=$([BitConverter]::ToString($b, 0, $n))"
} catch {
    "READ-FAIL: $($_.Exception.Message)"
}
$c.Close()

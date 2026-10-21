import bcrypt

hashes = {
    "admin": b"$2y$12$aFD9BDeUocDMY1X4tDYDyeJw/HhkQwCQWs3KAY7gCaRG0cpqJcaL.",
    "beidou_test": b"$2y$12$LsV0SqtQHyqLUL26MtQz8eydDlwcFnMUd5jcwfzGTR2wWmX5Y3wyi",
    "test_acct_001": b"$2y$12$o8./xflJZWAXU1O5MBXF/.cMvMC81fQjl5sNsbu2WC7z39FNvdB7y",
}
cands = [
    "admin", "Admin", "123456", "password", "pass", "beidou", "test", "000000",
    "111111", "admin123", "beidou123", "test123", "qwerty", "maple", "maplestory",
    "xiaoye", "root", "1234", "12345", "888888", "666666", "abcd1234", "BeiDou",
    "beidou_test", "test_acct_001", "12345678", "asdfgh", "passw0rd", "gm", "GM",
]

for name, h in hashes.items():
    hh = h.replace(b"$2y$", b"$2a$")
    matched = None
    for c in cands:
        if bcrypt.checkpw(c.encode("utf-8"), hh):
            matched = c
            break
    print(f"{name}: {matched or 'NO_MATCH'}")

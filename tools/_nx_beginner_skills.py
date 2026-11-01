import importlib.util
spec = importlib.util.spec_from_file_location('nxi', 'e:/project/MapleStory-Client/tools/nx_inspect.py')
nxi = importlib.util.module_from_spec(spec)
spec.loader.exec_module(nxi)

skill = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/Skill.nx')
string = nxi.Nx('E:/MXD/BeiDou-Client-xy/Data/.nxcache/String.nx')


def find(nx, path):
    i = 0
    for part in path.split('/'):
        hit = None
        for c in nx.children(i):
            if nx.name(c) == part:
                hit = c
                break
        if hit is None:
            return None
        i = hit
    return i


def child(nx, i, name):
    for c in nx.children(i):
        if nx.name(c) == name:
            return c
    return None


out = []
node = find(skill, '000.img/skill')
for c in skill.children(node):
    sid = nx_name = skill.name(c)
    inv = child(skill, c, 'invisible')
    inv_v = skill.value(inv) if inv is not None else None
    hid = child(skill, c, 'hastobebought')
    hid_v = skill.value(hid) if hid is not None else None
    np_ = find(string, 'Skill.img/' + sid)
    nm = ''
    if np_ is not None:
        nc = child(string, np_, 'name')
        if nc is not None:
            nm = string.value(nc)
    out.append('%s invisible=%s hastobebought=%s name=%s' % (sid, inv_v, hid_v, nm))

bn = find(string, 'Skill.img/000')
if bn is not None:
    bnc = child(string, bn, 'bookName')
    out.insert(0, 'bookName=%s' % string.value(bnc))

with open('E:/project/_beginner_skills.txt', 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
print('wrote', len(out))

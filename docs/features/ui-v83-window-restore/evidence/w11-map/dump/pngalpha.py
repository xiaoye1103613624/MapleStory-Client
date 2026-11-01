import zlib, struct, sys

def read_png(path):
    with open(path,'rb') as f: data=f.read()
    assert data[:8]==b'\x89PNG\r\n\x1a\n'
    i=8; W=H=bd=ct=None; idat=b''
    while i<len(data):
        ln=struct.unpack('>I',data[i:i+4])[0]; typ=data[i+4:i+8]; chunk=data[i+8:i+8+ln]; i+=12+ln
        if typ==b'IHDR':
            W,H,bd,ct=struct.unpack('>IIBB',chunk[:10])
        elif typ==b'IDAT': idat+=chunk
        elif typ==b'IEND': break
    raw=zlib.decompress(idat)
    ch={0:0,2:3,3:1,4:1,6:4}[ct]
    stride=W*ch
    pixels=[]
    prev=bytearray(stride)
    pos=0
    for y in range(H):
        ft=raw[pos]; pos+=1
        line=bytearray(raw[pos:pos+stride]); pos+=stride
        if ft==1:
            for x in range(ch,stride): line[x]=(line[x]+line[x-ch])&255
        elif ft==2:
            for x in range(stride): line[x]=(line[x]+prev[x])&255
        elif ft==3:
            for x in range(stride):
                a=line[x-ch] if x>=ch else 0
                line[x]=(line[x]+((a+prev[x])>>1))&255
        elif ft==4:
            for x in range(stride):
                a=line[x-ch] if x>=ch else 0
                b=prev[x]; c=prev[x-ch] if x>=ch else 0
                p=a+b-c; pa=abs(p-a); pb=abs(p-b); pc=abs(p-c)
                pr=a if (pa<=pb and pa<=pc) else (b if pb<=pc else c)
                line[x]=(line[x]+pr)&255
        prev=line
        for x in range(W):
            if ct==6:
                r,g,b,al=line[x*4:x*4+4]
            elif ct==2:
                r,g,b=line[x*3:x*3+3]; al=255
            else:
                r=g=b=0; al=line[x]
            pixels.append((x,y,r,g,b,al))
    return W,H,pixels

def analyze(path):
    W,H,pix=read_png(path)
    def opaque(px): return px[5]>8
    top=sum(1 for x,y,r,g,b,a in pix if y==0 and a>8)
    bot=sum(1 for x,y,r,g,b,a in pix if y==H-1 and a>8)
    left=sum(1 for x,y,r,g,b,a in pix if x==0 and a>8)
    right=sum(1 for x,y,r,g,b,a in pix if x==W-1 and a>8)
    tot=sum(1 for px in pix if px[5]>8)
    # sample mean color of opaque
    cols=[(r,g,b) for x,y,r,g,b,a in pix if a>128]
    if cols:
        mr=sum(c[0] for c in cols)//len(cols); mg=sum(c[1] for c in cols)//len(cols); mb=sum(c[2] for c in cols)//len(cols)
    else: mr=mg=mb=0
    print('%s  %dx%d  opaque=%d/%d  top=%d bot=%d left=%d right=%d  color=#%02X%02X%02X'%(
        path.split('/')[-1],W,H,tot,W*H,top,bot,left,right,mr,mg,mb))
    # ascii map downsample
    step_y=max(1,H//12); step_x=max(1,W//24)
    for y in range(0,H,step_y):
        row=''
        for x in range(0,W,step_x):
            a=pix[y*W+x][5]
            row+= '#' if a>128 else ('+' if a>8 else '.')
        print('   '+row)

for p in sys.argv[1:]:
    analyze(p)

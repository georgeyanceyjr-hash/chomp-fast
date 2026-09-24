import functools,struct,itertools,json,os
from pathlib import Path
import argparse
ap=argparse.ArgumentParser();ap.add_argument('--output-dir',default='bootstrap-test');a=ap.parse_args()
out=Path(a.output_dir);out.mkdir(parents=True,exist_ok=True)
@functools.lru_cache(None)
def losing(rows):
    for y,r in enumerate(rows):
        for x in range(r):
            if x==0 and y==0: continue
            child=rows[:y]+tuple(min(x,z) for z in rows[y:] if min(x,z))
            if losing(child): return False
    return True

def shapes(w,h,p=()):
    if p: yield p
    if h:
        for r in range(1,w+1): yield from shapes(r,h-1,p+(r,))
def bits(rows):
    v=0;last=0
    for r in reversed(rows):
        d=r-last;v=(v<<d)|((1<<d)-1);v<<=1;last=r
    return v
seed=[]
for w in range(1,12):
    for p in shapes(w,12-w-1,(w,)):
        if losing(p):seed.append(bits(p))
seed=sorted(set(seed));frontier=(1<<12)-1
(out/'seed.bin').write_bytes(b'CHOMPP01'+struct.pack('<II',len(seed),frontier)+struct.pack('<%dI'%len(seed),*seed))
base=set()
for p in shapes(19,2):
    if losing(p):
        base.add(bits(p)); conj=tuple(sum(r>i for r in p) for i in range(p[0]));base.add(bits(conj))
for d in range(1,20):base.add(bits((d,)+(1,)*(d-1)))
base=sorted(v for v in base if v>frontier)
def checksum(w,h,values):
    s=1469598103934665603
    for z in [w,h,len(values)]+values:
        for b in struct.pack('<Q',z):s=((s^b)*1099511628211)&((1<<64)-1)
    return s
(out/'base.bin').write_bytes(b'CHOMPCP2CLAS'+struct.pack('<HHQQ',0,0,len(base),checksum(0,0,base))+struct.pack('<%dQ'%len(base),*base))
expected=sorted(bits(p) for p in shapes(8,8) if losing(p))
(out/'expected8.json').write_text(json.dumps(expected))
(out/'bootstrap.json').write_text(json.dumps({'seed_entries':len(seed),'seed_frontier':frontier,'base_entries':len(base),'independent_8x8_P_count':len(expected),'cpu_count':os.cpu_count()},indent=2))
print((out/'bootstrap.json').read_text())

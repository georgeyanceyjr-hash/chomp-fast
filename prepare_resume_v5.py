#!/usr/bin/env python3
"""Prepare a preload for extending a completed dimension D to D+1.
Does not launch any sieve. Requires the original seed.bin and full checkpoint.
"""
import argparse,functools,struct
from pathlib import Path
ap=argparse.ArgumentParser();ap.add_argument('seed');ap.add_argument('checkpoint');ap.add_argument('output');ap.add_argument('--dimension',type=int,required=True);a=ap.parse_args()
if not 4<=a.dimension<=30:raise SystemExit('dimension must be 4..30')
raw=Path(a.seed).read_bytes();assert raw[:8]==b'CHOMPP01';_,frontier=struct.unpack_from('<II',raw,8)
@functools.lru_cache(None)
def losing(rows):
 for y,r in enumerate(rows):
  for x in range(r):
   if x==y==0:continue
   child=rows[:y]+tuple(min(x,z) for z in rows[y:] if min(x,z))
   if losing(child):return False
 return True
def bits(rows):
 v=0;last=0
 for r in reversed(rows):
  d=r-last;v=((v<<d)|((1<<d)-1))<<1;last=r
 return v
base=set()
for w in range(1,a.dimension+1):
 for h2 in range(w+1):
  p=(w,h2) if h2 else (w,)
  if losing(p):
   base.add(bits(p));base.add(bits(tuple(sum(r>i for r in p) for i in range(w))))
for d in range(1,a.dimension+1):base.add(bits((d,)+(1,)*(d-1)))
values=sorted(v for v in base if v>frontier);s=1469598103934665603
for z in [0,0,len(values)]+values:
 for b in struct.pack('<Q',z):s=((s^b)*1099511628211)&((1<<64)-1)
# Stream the existing checkpoint: do not load the gigabyte-scale file in RAM.
with open(a.checkpoint,'rb') as src,open(a.output,'xb') as dst:
 assert src.read(8)==b'CHOMPCP2'
 dst.write(b'CHOMPCP2CLAS'+struct.pack('<HHQQ',0,0,len(values),s)+struct.pack('<%dQ'%len(values),*values))
 while True:
  chunk=src.read(8*1024*1024)
  if not chunk:break
  dst.write(chunk)
print('Prepared preload for dimension',a.dimension,'with',len(values),'base entries. No computation launched.')

"""Generate only analytic base cases; no previous computed catalog is read."""
from pathlib import Path
import struct,json
def bits(rows):
 v=last=0
 for r in reversed(rows):
  d=r-last;v=((v<<d)|((1<<d)-1))<<1;last=r
 return v
def transpose(rows):return tuple(sum(r>i for r in rows) for i in range(rows[0]))
def create(out,W=42,H=10):
 out=Path(out);out.mkdir(parents=True,exist_ok=True)
 (out/'seed.bin').write_bytes(b'CHOMPP01'+struct.pack('<III',1,3,2))
 values=set()
 for w in range(2,W+1):
  rows=(w,w-1);values.update([bits(rows),bits(transpose(rows))])
 for d in range(2,H+1):values.add(bits((d,)+(1,)*(d-1)))
 values=sorted(values);s=1469598103934665603
 for z in [0,0,len(values)]+values:
  for b in struct.pack('<Q',z):s=((s^b)*1099511628211)&((1<<64)-1)
 (out/'base.bin').write_bytes(b'CHOMPCP2CLAS'+struct.pack('<HHQQ',0,0,len(values),s)+struct.pack(f'<{len(values)}Q',*values))
 info=dict(seed_positions=1,seed='poisoned single square only',analytic_base_positions=len(values),base_families=['two-row (w,w-1), including transposes','equal-arm L shapes'],previous_computed_positions_reused=0)
 (out/'inputs.json').write_text(json.dumps(info,indent=2)+'\n');return info
if __name__=='__main__':
 import sys
 print(create(sys.argv[1],int(sys.argv[2]),int(sys.argv[3])))

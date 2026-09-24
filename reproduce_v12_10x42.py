#!/usr/bin/env python3
"""Fresh 10x42 benchmark, analytic inputs only; requires compiled V12 binaries."""
from pathlib import Path
import argparse,subprocess,time,json,hashlib,datetime,struct
from fresh_inputs import create
ap=argparse.ArgumentParser();ap.add_argument('--solver',type=Path,required=True);ap.add_argument('--validator',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--workers',type=int,default=9);a=ap.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=False);solver=a.solver.resolve();validator=a.validator.resolve();assert a.workers>0
started=datetime.datetime.now(datetime.timezone.utc).isoformat();t=time.monotonic();inputs=create(out);cp=out/'checkpoint.bin'
cmd=[str(solver),str(out/'seed.bin'),'42',str(cp),str(out/'base.bin'),'0','0',str(a.workers),'10','0'];st=time.monotonic()
with (out/'sieve.log').open('w') as log:subprocess.run(cmd,stdout=log,stderr=subprocess.STDOUT,check=True)
solver_seconds=time.monotonic()-st;total=time.monotonic()-t
report=dict(state='checking',started_at=started,workers=a.workers,inputs=inputs,solver_wall_seconds=solver_seconds,total_wall_seconds=total,timing_scope='Fresh analytic input generation through solver exit; excludes compilation and post-run validation.')
(out/'status.json').write_text(json.dumps(report,indent=2)+'\n')
with (out/'validation.log').open('w') as log:subprocess.run([str(validator),'merge',str(out/'base.bin'),str(out/'seed.bin'),str(cp),'42','10','0'],stdout=log,stderr=subprocess.STDOUT,check=True)
cat=Path(str(cp)+'.catalog.bin');h=hashlib.sha256()
with cat.open('rb') as f:
 header=f.read(32);h.update(header);n=struct.unpack_from('<Q',header,8)[0]
 for b in iter(lambda:f.read(8*1024*1024),b''):h.update(b)
assert n==107342199 and h.hexdigest()=='13893537aba7adcf0d5f168d7e9fd9147732c73777021568b18d100051440cb5'
report.update(state='validated',nonempty_P_positions=n,catalog_sha256=h.hexdigest());(out/'status.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))

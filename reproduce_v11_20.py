"""Reproduce the V11 19-to20 continuation; refuse an existing output directory."""
from pathlib import Path
import argparse,json,hashlib,subprocess,time,datetime
ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--solver',type=Path,required=True);ap.add_argument('--output',type=Path,required=True);ap.add_argument('--workers',type=int,default=9);a=ap.parse_args();root=Path(__file__).resolve().parent
if a.workers<1:ap.error('workers must be positive')
def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(8*1024*1024),b''):h.update(b)
 return h.hexdigest()
for name,expected in [('seed.bin','d9d23f9b2779a6c991d6c62d1a78f244c64dbaadadf365619574e2ec28661a36'),('preload20.bin','dd2a903c45f9af8ceb5650315ba52a53f44ea14e094f69d9115748c442c22587')]:
 if sha(root/name)!=expected:ap.error(name+' checksum mismatch')
a.output.mkdir(parents=True,exist_ok=False);cp=(a.output/'checkpoint20.bin').resolve();command=[str(a.solver.resolve()),str(root/'seed.bin'),'20',str(cp),str(root/'preload20.bin'),'20','0',str(a.workers)]
s={'state':'running','workers':a.workers,'command':command,'started_at':datetime.datetime.now(datetime.timezone.utc).isoformat()};(a.output/'status.json').write_text(json.dumps(s,indent=2));start=time.monotonic()
with (a.output/'sieve.log').open('w') as f:rc=subprocess.run(command,stdout=f,stderr=subprocess.STDOUT).returncode
s.update(exit_code=rc,wall_seconds=time.monotonic()-start,finished_at=datetime.datetime.now(datetime.timezone.utc).isoformat())
if rc==0:
 s['checkpoint_sha256']=sha(cp);s['state']='validated' if s['checkpoint_sha256']=='7b94bb92f3474bc70014b176a117420d473a2985e77d1586c72ef9f1af6c16c3' else 'mismatch'
else:s['state']='failed'
(a.output/'status.json').write_text(json.dumps(s,indent=2)+'\n');print(json.dumps(s,indent=2));raise SystemExit(0 if s['state']=='validated' else 1)

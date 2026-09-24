"""Time a fresh 18x18 run and verify the complete checkpoint against V5/V6."""
import argparse, datetime, hashlib, json, pathlib, subprocess, time
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--solver',type=pathlib.Path,required=True)
p.add_argument('--seed',type=pathlib.Path,required=True)
p.add_argument('--base',type=pathlib.Path,required=True,help='dimension-18 base18.bin')
p.add_argument('--output',type=pathlib.Path,required=True,help='new output directory')
p.add_argument('--workers',type=int,default=9)
p.add_argument('--source',type=pathlib.Path,required=True)
a=p.parse_args()
EXPECTED='a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a'
def sha(path):
 h=hashlib.sha256()
 with path.open('rb') as f:
  for b in iter(lambda:f.read(8*1024*1024),b''):h.update(b)
 return h.hexdigest()
if a.workers<1:p.error('workers must be positive')
if sha(a.seed)!='d9d23f9b2779a6c991d6c62d1a78f244c64dbaadadf365619574e2ec28661a36':p.error('seed does not match the validated bootstrap')
if sha(a.base)!='29ef74bccbaf50ae7bb7c078f615b5f63fa088562cf29a99228c513fb55f0fae':p.error('base does not match the validated dimension-18 base')
a.output.mkdir(parents=True,exist_ok=False)
cp=(a.output/'checkpoint.bin').resolve()
cmd=[str(a.solver.resolve()),str(a.seed.resolve()),'18',str(cp),str(a.base.resolve()),'0','0',str(a.workers)]
s={'source_sha256':sha(a.source),'binary_sha256':sha(a.solver),'state':'running','dimension':18,'workers':a.workers,'command':cmd,'started_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'seed_sha256':sha(a.seed),'base_sha256':sha(a.base),'expected_checkpoint_sha256':EXPECTED}
(a.output/'status.json').write_text(json.dumps(s,indent=2))
start=time.perf_counter()
with (a.output/'sieve.log').open('w') as f:
 rc=subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT).returncode
s.update(exit_code=rc,wall_seconds=time.perf_counter()-start,finished_at=datetime.datetime.now(datetime.timezone.utc).isoformat())
s['state']='checking' if rc==0 else 'failed'
(a.output/'status.json').write_text(json.dumps(s,indent=2))
if rc==0:
 s['actual_checkpoint_sha256']=sha(cp)
 s['exact_match']=s['actual_checkpoint_sha256']==EXPECTED
 s['state']='validated' if s['exact_match'] else 'mismatch'
else:s['state']='failed'
(a.output/'status.json').write_text(json.dumps(s,indent=2));print(json.dumps(s,indent=2))
raise SystemExit(0 if s['state']=='validated' else 1)

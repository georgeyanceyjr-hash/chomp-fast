"""Compare a fresh 8x8 run with an independently generated row-model reference."""
import argparse,hashlib,json,pathlib,struct,subprocess
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--solver',type=pathlib.Path,required=True)
p.add_argument('--expected',type=pathlib.Path,required=True)
p.add_argument('--output',type=pathlib.Path,required=True)
a=p.parse_args();root=pathlib.Path(__file__).resolve().parent
expected=json.loads(a.expected.read_text());a.output.mkdir(parents=True,exist_ok=False)
cp=a.output/'checkpoint.bin'
with (a.output/'sieve.log').open('w') as f:
 subprocess.run([str(a.solver.resolve()),str(root/'seed.bin'),'8',str(cp.resolve()),str(root/'base18.bin'),'0','0','3'],stdout=f,stderr=subprocess.STDOUT,check=True)
raw=(root/'seed.bin').read_bytes();n=struct.unpack_from('<I',raw,8)[0]
values=set(struct.unpack_from('<%dI'%n,raw,16))
for path in (root/'base18.bin',cp):
 raw=path.read_bytes()
 if raw[:8]!=b'CHOMPCP2':raise RuntimeError('bad checkpoint magic')
 offset=8
 while offset<len(raw):
  if raw[offset:offset+4]!=b'CLAS':raise RuntimeError('bad record marker')
  w,h,n,checksum=struct.unpack_from('<HHQQ',raw,offset+4);offset+=24
  entries=struct.unpack_from('<%dQ'%n,raw,offset);offset+=n*8;s=1469598103934665603
  for z in [w,h,n]+list(entries):
   for b in struct.pack('<Q',z):s=((s^b)*1099511628211)&((1<<64)-1)
  if s!=checksum:raise RuntimeError('record checksum mismatch')
  values.update(entries)
actual=sorted(v for v in values if bin(v).count('1')<=8 and v.bit_length()-bin(v).count('1')<=8)
if actual!=expected or len(actual)!=322:raise RuntimeError('independent 8x8 reference mismatch')
digest=hashlib.sha256(cp.read_bytes()).hexdigest()
if digest!='9e1779f0056efce30ac104fb10fb321afb649a467d314c3e5c257859c6ccd491':raise RuntimeError('8x8 checkpoint hash mismatch')
r={'independent_8x8_positions':len(actual),'exact_match':True,'checkpoint_sha256':digest}
(a.output/'result.json').write_text(json.dumps(r,indent=2));print(json.dumps(r,indent=2))

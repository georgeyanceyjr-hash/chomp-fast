#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
template<class T>T rd(ifstream&f){T x;if(!f.read((char*)&x,sizeof x))throw runtime_error("truncated input");return x;}
void magic(ifstream&f,string s){string t(s.size(),' ');if(!f.read(&t[0],t.size())||t!=s)throw runtime_error("bad magic");}
void feed(uint64_t&s,uint64_t v){for(int i=0;i<8;i++){s^=uint8_t(v);s*=1099511628211ull;v>>=8;}}
int width(uint64_t v){return __builtin_popcountll(v);}int height(uint64_t v){return 64-__builtin_clzll(v)-width(v);}
bool bound(uint64_t v,int W,int H){if(!v||(v&1))return false;int w=width(v),h=height(v);return w>0&&h>0&&((w<=W&&h<=H)||(w<=H&&h<=W));}
uint64_t enc(vector<int> rows){uint64_t v=0;int last=0;for(auto it=rows.rbegin();it!=rows.rend();it++){int d=*it-last;v=((v<<d)|((1ull<<d)-1))<<1;last=*it;}return v;}
uint64_t conjugate(uint64_t v){int n=64-__builtin_clzll(v);uint64_t r=0;for(int i=0;i<n;i++){r=(r<<1)|((v&1)^1);v>>=1;}return r;}
void writecp(string path,const vector<uint64_t>&v){ofstream f(path,ios::binary);uint16_t z=0;uint64_t n=v.size(),s=1469598103934665603ull;feed(s,0);feed(s,0);feed(s,n);for(auto x:v)feed(s,x);f.write("CHOMPCP2CLAS",12);f.write((char*)&z,2);f.write((char*)&z,2);f.write((char*)&n,8);f.write((char*)&s,8);f.write((char*)v.data(),n*8);if(!f)throw runtime_error("write");}
int main(int argc,char**argv){try{
 if(argc!=8)throw runtime_error("usage: prepare|merge catalog|preload seed checkpoint|output W H old-width");string mode=argv[1];int W=stoi(argv[5]),H=stoi(argv[6]),old=stoi(argv[7]);if(W+H>63||W<H||H<3)throw runtime_error("bounds");ifstream seed(argv[3],ios::binary);magic(seed,"CHOMPP01");auto ns=rd<uint32_t>(seed),front=rd<uint32_t>(seed);vector<uint64_t> sv;for(unsigned i=0;i<ns;i++)sv.push_back(rd<uint32_t>(seed));vector<uint64_t> all;
 if(mode=="prepare"){
  ifstream f(argv[2],ios::binary);magic(f,"CHOMPP02");auto n=rd<uint64_t>(f);rd<uint64_t>(f);rd<uint32_t>(f);rd<uint32_t>(f);uint64_t prev=0;
  for(uint64_t i=0;i<n;i++){auto v=rd<uint64_t>(f);if(v<=prev)throw runtime_error("catalog order");prev=v;if(v>front&&bound(v,old,H))all.push_back(v);}if(f.peek()!=EOF)throw runtime_error("catalog extra");
  for(int w=2;w<=W;w++){auto v=enc({w,w-1});if(v>front)all.push_back(v);v=conjugate(v);if(v>front)all.push_back(v);}
  for(int d=1;d<=H;d++){vector<int> rows(d,1);rows[0]=d;auto v=enc(rows);if(v>front)all.push_back(v);}
  sort(all.begin(),all.end());all.erase(unique(all.begin(),all.end()),all.end());for(auto v:all)if(!bound(v,W,H))throw runtime_error("preload bounds");writecp(argv[4],all);cout<<"preload_positions="<<all.size()<<"\n";
 }else if(mode=="merge"){
  for(auto v:sv)if(bound(v,W,H))all.push_back(v);int records=0;vector<pair<int,int>> expected;for(int h=3;h<=H;h++)for(int w=max(h+1,old+1);w<=W;w++)expected.push_back({w,h});
  for(int file=0;file<2;file++){ifstream f(file?argv[4]:argv[2],ios::binary);magic(f,"CHOMPCP2");int rec=0;while(f.peek()!=EOF){magic(f,"CLAS");auto w=rd<uint16_t>(f),h=rd<uint16_t>(f);auto n=rd<uint64_t>(f),sum=rd<uint64_t>(f);uint64_t s=1469598103934665603ull;feed(s,w);feed(s,h);feed(s,n);uint64_t prev=0;if(file&&(rec>=int(expected.size())||w!=expected[rec].first||h!=expected[rec].second))throw runtime_error("class sequence");if(!file&&(w||h||rec))throw runtime_error("preload record");
   for(uint64_t i=0;i<n;i++){auto v=rd<uint64_t>(f);if(v<=prev||v<=front||!bound(v,W,H))throw runtime_error("record bounds/order");prev=v;feed(s,v);if(file&&!((width(v)==w&&height(v)==h)||(width(v)==h&&height(v)==w)))throw runtime_error("wrong class");all.push_back(v);}if(s!=sum)throw runtime_error("checksum");rec++;}
   if(file&&rec!=int(expected.size()))throw runtime_error("missing classes");if(file)records=rec;
  }
  sort(all.begin(),all.end());if(adjacent_find(all.begin(),all.end())!=all.end())throw runtime_error("overlap");vector<uint64_t> result;for(auto v:all)if(width(v)<=W&&height(v)<=H)result.push_back(v);string path=string(argv[4])+".catalog.bin";ofstream f(path,ios::binary);uint64_t n=result.size(),mx=result.back();uint32_t w=W,h=H;f.write("CHOMPP02",8);f.write((char*)&n,8);f.write((char*)&mx,8);f.write((char*)&w,4);f.write((char*)&h,4);f.write((char*)result.data(),n*8);if(!f)throw runtime_error("catalog write");cout<<"classes="<<records<<" union_positions="<<all.size()<<" catalog_positions="<<n<<" bytes="<<32+n*8<<"\n";
 }else throw runtime_error("mode");
}catch(exception&e){cerr<<e.what()<<"\n";return 1;}}

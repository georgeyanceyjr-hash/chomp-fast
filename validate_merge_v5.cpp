#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
using namespace std;
template<class T>T read(ifstream&f){T v; if(!f.read((char*)&v,sizeof v))throw runtime_error("truncated input");return v;}
void feed(uint64_t& s,uint64_t v){for(int i=0;i<8;i++){s^=uint8_t(v);s*=1099511628211ull;v>>=8;}}
int main(int argc,char**argv){try{if(argc!=5)throw runtime_error("usage: merge seed base checkpoint output");vector<uint64_t> all;all.reserve(150000000);ifstream seed(argv[1],ios::binary);char m[8];seed.read(m,8);if(string(m,8)!="CHOMPP01")throw runtime_error("seed magic");auto n=read<uint32_t>(seed);auto frontier=read<uint32_t>(seed);uint64_t prev=0;for(uint32_t i=0;i<n;i++){auto v=read<uint32_t>(seed);if(v<=prev||v>frontier)throw runtime_error("seed order");all.push_back(v);prev=v;}if(seed.peek()!=EOF)throw runtime_error("seed trailing data");
for(int file=2;file<=3;file++){ifstream f(argv[file],ios::binary);f.read(m,8);if(string(m,8)!="CHOMPCP2")throw runtime_error("checkpoint magic");int lastw=0,lasth=0,records=0;while(f.peek()!=EOF){char marker[4];f.read(marker,4);if(string(marker,4)!="CLAS")throw runtime_error("record marker");auto w=read<uint16_t>(f),h=read<uint16_t>(f);auto count=read<uint64_t>(f),expected=read<uint64_t>(f);uint64_t sum=1469598103934665603ull;feed(sum,w);feed(sum,h);feed(sum,count);prev=0;for(uint64_t i=0;i<count;i++){auto v=read<uint64_t>(f);if(v<=prev||v<=frontier)throw runtime_error("record order/frontier");int width=__builtin_popcountll(v),height=64-__builtin_clzll(v)-width;if(!v||(v&1)||width>19||height>19)throw runtime_error("position bounds");if(file==3&&!((width==w&&height==h)||(width==h&&height==w)))throw runtime_error("class mismatch");feed(sum,v);all.push_back(v);prev=v;}if(sum!=expected)throw runtime_error("checksum mismatch");if(file==3&&(h<lasth||(h==lasth&&w<=lastw)))throw runtime_error("class order");lastw=w;lasth=h;records++;}if(file==3&&(lastw!=19||lasth!=18||records!=136))throw runtime_error("incomplete class sequence");cout<<"validated "<<argv[file]<<" records="<<records<<" final="<<lastw<<"x"<<lasth<<endl;}
sort(all.begin(),all.end());auto end=unique(all.begin(),all.end());if(end!=all.end())throw runtime_error("overlapping catalogs");uint64_t count=all.size(),maxv=all.back();uint32_t dim=19;ofstream out(argv[4],ios::binary);out.write("CHOMPP02",8);out.write((char*)&count,8);out.write((char*)&maxv,8);out.write((char*)&dim,4);out.write((char*)&dim,4);out.write((char*)all.data(),all.size()*8);out.close();if(!out)throw runtime_error("write failed");cout<<"total_unique_positions="<<count<<" bytes="<<32+count*8<<endl;
}catch(const exception&e){cerr<<e.what()<<endl;return 1;}}

from pathlib import Path
p=Path(__file__).resolve().parent
s=(p/'chomp_sieve_v9_candidate.cpp').read_text()
init=s.split('      uint64_t comb=(1ull<<k)-1, limit=1ull<<m, class_tested=0, class_new=0;')[1].split('      const ClassShard* current_class=')[0]
fill=s.split('        if(fast_enumeration){')[1].split('        unsigned active=')[0]
(p/'test_generation_v9.cpp').write_text('''#define main solver_main
#include "chomp_sieve_v9_candidate.cpp"
#undef main
#include <cassert>
struct Result { std::vector<uint64_t> positions; uint64_t basic=0,known=0,tested=0; };
static Result direct(int w,int h,uint64_t frontier,size_t CHUNK){
    struct {uint64_t frontier;} db{frontier};
    int n=w+h,k=w-1,m=n-2;
    uint64_t comb=(1ull<<k)-1, limit=1ull<<m, class_tested=0,class_new=0;
    Result result;auto& basic=result.basic;auto& known=result.known;auto& tested=result.tested;
'''+init+'''
    struct Work{uint64_t v;Hit hit;};
    while(comb<limit){
      std::vector<Work> batch;batch.reserve(CHUNK);
      if(fast_enumeration){'''+fill+'''
      for(auto q:batch)result.positions.push_back(q.v);
    }
    return result;
}
static Result reference(int w,int h,uint64_t frontier){
    Result result;
    int n=w+h,k=w-1,m=n-2;
    uint64_t comb=(1ull<<k)-1,limit=1ull<<m;
    while(comb<limit){
        uint64_t v=(1ull<<(n-1))|(comb<<1);
        uint64_t c=comb&-comb,r=comb+c;comb=((r^comb)>>(__builtin_ctzll(comb)+2))|r;
        if(v<=frontier){++result.known;continue;}
        if(basic_sieved(v,n,w,h)){++result.basic;continue;}
        result.positions.push_back(v);++result.tested;
    }
    return result;
}
int main(){
 uint64_t cases=0,positions=0;
 for(int h=3;h<12;++h)for(int w=h+1;w<=13;++w){
  if(w+h>23)continue;
  for(uint64_t frontier:{0ull,4095ull,8191ull,123456ull}){
   auto r=reference(w,h,frontier);
   for(size_t chunk:{1,7,257,16384}){
    auto d=direct(w,h,frontier,chunk);
    if(r.positions!=d.positions||r.basic!=d.basic||r.known!=d.known||r.tested!=d.tested){
      std::cerr<<"Mismatch "<<w<<"x"<<h<<" frontier "<<frontier<<" chunk "<<chunk<<" positions "<<r.positions.size()<<" vs "<<d.positions.size()<<" basic "<<r.basic<<" vs "<<d.basic<<"\\n";return 1;
    }
    ++cases;positions+=r.positions.size();
   }
  }
 }
 std::cout<<"PASSED "<<cases<<" parameter cases, "<<positions<<" emitted candidates checked in exact ascending order, with identical counters.\\n";
}
''')

#define main sieve_main
#include "chomp_sieve_v9_candidate.cpp"
#undef main
#include "reference_corner_v9.h"
#include <random>
static uint64_t encode(const std::vector<int>& rows){
 uint64_t bits=0;int last=0;
 for(auto it=rows.rbegin();it!=rows.rend();++it){int d=*it-last;bits=(bits<<d)|((1ull<<d)-1);bits<<=1;last=*it;}return bits;
}
static uint64_t row_move(const std::vector<int>& rows,int x,int y){
 auto child=rows;for(int r=int(rows.size())-1-y;r<int(rows.size());r++)child[r]=std::min(child[r],x);
 while(!child.empty()&&!child.back())child.pop_back();return encode(child);
}
int main(int argc,char** argv){
 std::mt19937_64 gen(20260924);DB db(argc>1?argv[1]:"seed.bin");
 uint64_t row_moves=0,decisions=0;
 for(int trial=0;trial<30000;trial++){
  int w=1+gen()%31,h=1+gen()%31;std::vector<int> rows(h);
  for(auto& r:rows)r=1+gen()%w;rows[0]=w;std::sort(rows.rbegin(),rows.rend());
  uint64_t v=encode(rows);int n=w+h,ones[64],zeros[64],no=0,nz=0;
  for(int i=0;i<n;i++)((v>>(n-1-i))&1?ones[no++]:zeros[nz++])=i;
  std::vector<uint64_t> moves;ClassShard shard;shard.reserve(128);
  for(int xi=1;xi<no;xi++)for(int zi=nz-2;zi>=0;zi--){
   if(zeros[zi]<=ones[xi])break;
   const uint64_t expected=row_move(rows,xi,zi);moves.push_back(expected);++row_moves;
   if((gen()&15)==0)shard.insert(expected);
  }
  for(int j=0;j<10;j++)shard.insert((gen()&((1ull<<n)-1))|1);
  for(int j=0;j<10;j++){
   uint64_t floor=j==0?0:j==1?v:j==2?v+1:j==3?v-1:gen()%v;
   bool expected=false;for(auto q:moves)if(q>=floor&&shard.contains(q)){expected=true;break;}
   const bool got=corner_hit({v,n},db,&shard,floor);
   const bool original=corner_hit_reference({v,n},db,&shard,floor);
   if(got!=expected||original!=expected){std::cerr<<"mismatch n="<<n<<" v="<<v<<" floor="<<floor<<" expected="<<expected<<" got="<<got<<" original="<<original<<"\n";return 1;}
   ++decisions;
  }
 }
 std::cout<<row_moves<<" independent row-model interior moves, "<<decisions<<" corner/floor membership decisions passed\n";
}

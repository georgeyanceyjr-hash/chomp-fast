#define main sieve_main
#include "chomp_sieve_v7_candidate.cpp"
#undef main
#include <random>
#include <cassert>
static uint64_t encode(const std::vector<int>& rows){
 uint64_t bits=0;int last=0;
 for(auto it=rows.rbegin();it!=rows.rend();++it){int d=*it-last;bits=(bits<<d)|((1ull<<d)-1);bits<<=1;last=*it;}return bits;
}
static uint64_t row_move(const std::vector<int>& rows,int x,int y){
 auto child=rows;for(int r=int(rows.size())-1-y;r<int(rows.size());r++)child[r]=std::min(child[r],x);
 while(!child.empty()&&!child.back())child.pop_back();return encode(child);
}
int main(){
 uint64_t transforms=0,row_checks=0;
 // Exhaust every normalized bit-pattern through 20 boundary bits, including
 // degenerate single-row and single-column cases.
 for(int n=2;n<=20;n++)for(uint64_t v=1ull<<(n-1);v<(1ull<<n);v+=2){
  int ones[64],zeros[64],no=0,nz=0;
  for(int i=0;i<n;i++)((v>>(n-1-i))&1?ones[no++]:zeros[nz++])=i;
  uint64_t edge=1ull<<(n-1);
  for(int j=0;j<nz-1;j++){
   uint64_t q=height_move_bits(v,edge,1ull<<(n-1-zeros[j]));
   Pos expected=play({v,n},ones[0],zeros[j]);
   if(q!=expected.bits||expected.n!=no+nz-j-1){std::cerr<<"height mismatch";return 1;}
   edge>>=1;++transforms;
  }
  uint64_t prefix=1ull<<(n-1);
  for(int i=1;i<no;i++){
   uint64_t q=width_move_bits(prefix,no,i);Pos expected=play({v,n},ones[i],zeros[nz-1]);
   if(q!=expected.bits||expected.n!=nz+i){std::cerr<<"width mismatch";return 1;}
   prefix|=1ull<<(n-1-ones[i]);++transforms;
  }
 }
 std::mt19937 gen(20260924);
 for(int trial=0;trial<50000;trial++){
  int w=1+gen()%31,h=1+gen()%31;std::vector<int> rows(h);
  for(auto& r:rows)r=1+gen()%w;rows[0]=w;std::sort(rows.rbegin(),rows.rend());
  uint64_t v=encode(rows);int n=w+h,ones[64],zeros[64],no=0,nz=0;
  for(int i=0;i<n;i++)((v>>(n-1-i))&1?ones[no++]:zeros[nz++])=i;
  uint64_t edge=1ull<<(n-1),prefix=edge;
  for(int j=0;j<nz-1;j++){
   uint64_t q=height_move_bits(v,edge,1ull<<(n-1-zeros[j]));
   if(q!=row_move(rows,0,j)){std::cerr<<"row height mismatch";return 1;}
   edge>>=1;++row_checks;
  }
  for(int i=1;i<no;i++){
   uint64_t q=width_move_bits(prefix,no,i);
   if(q!=row_move(rows,i,h-1)){std::cerr<<"row width mismatch";return 1;}
   prefix|=1ull<<(n-1-ones[i]);++row_checks;
  }
 }
 std::cout<<transforms<<" exhaustive boundary transforms matched original play (n<=20)\n";
 std::cout<<row_checks<<" independent row-model boundary moves passed (w,h<=31)\n";
}

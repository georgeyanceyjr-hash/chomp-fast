#define main sieve_main
#include "chomp_sieve_v8_candidate.cpp"
#undef main
#include <random>
#include <unordered_set>
int main(){
 ClassShard shard;
 std::unordered_set<uint64_t> expected;
 std::mt19937_64 rng(20260924);
 for(unsigned i=0;i<10000;i++){
  uint64_t v=(rng()&((1ull<<38)-2))|(1ull<<37);
  bool first=expected.insert(v).second;
  if(shard.insert(v)!=first || !shard.contains(v) || shard.insert(v))return 1;
 }
 for(uint64_t v:expected)if(!shard.contains(v)||!shard.maybe(v))return 2;
 unsigned negatives=0,false_positives=0;
 for(unsigned i=0;i<200000;i++){
  uint64_t v=(rng()&((1ull<<38)-2))|(1ull<<37);
  bool wanted=expected.count(v)!=0;
  if(shard.contains(v)!=wanted)return 3;
  if(!wanted){++negatives;false_positives+=shard.maybe(v);}
 }
 if(shard.contains(0))return 4;
 std::cout<<expected.size()<<" inserted keys; duplicates and grow/rebuild checked; "
          <<negatives<<" negative membership queries; "<<false_positives
          <<" Bloom false positives; all exact membership results correct\n";
}

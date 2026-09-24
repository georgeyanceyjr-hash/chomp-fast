#define main sieve_main
#include "chomp_sieve_v5.cpp"
#undef main
#include <random>
int main(){std::mt19937 gen(19);size_t checked=0;for(int trial=0;trial<10000;trial++){
 std::vector<int> rows(19);for(auto& r:rows)r=1+gen()%19;rows[0]=19;std::sort(rows.rbegin(),rows.rend());
 uint64_t v=0;int last=0;for(auto it=rows.rbegin();it!=rows.rend();++it){int d=*it-last;v=(v<<d)|((1ull<<d)-1);v<<=1;last=*it;}
 int n=64-__builtin_clzll(v);std::vector<int> ones,zeros;for(int i=0;i<n;i++)((v>>(n-1-i))&1?ones:zeros).push_back(i);
 for(int x=0;x<19;x++)for(int y=0;y<19;y++){if(ones[x]>zeros[y])continue;
 auto child=rows;for(int r=18-y;r<19;r++)child[r]=std::min(child[r],x);while(!child.empty()&&!child.back())child.pop_back();
 uint64_t expected=0;last=0;for(auto it=child.rbegin();it!=child.rend();++it){int d=*it-last;expected=(expected<<d)|((1ull<<d)-1);expected<<=1;last=*it;}
 if(play({v,n},ones[x],zeros[y]).bits!=expected){std::cerr<<"move mismatch\n";return 1;}checked++;
 }}std::cout<<"Independent row-model moves passed: "<<checked<<"\n";}

// Experimental V8 candidate based on V7. Validation scope is recorded separately.
// Based on V5; not yet validated by a full 19x19 production run.
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

struct Pos { uint64_t bits; int n; };

static std::string format_time(double seconds){
    uint64_t s=uint64_t(seconds+0.5), h=s/3600, m=(s%3600)/60; s%=60;
    std::string out;
    if(h)out+=std::to_string(h)+"h ";
    if(h||m)out+=std::to_string(m)+"m ";
    out+=std::to_string(s)+"s";
    return out;
}

static Pos clean_bits(uint64_t v, int n) {
    if (!v) return {0,0};
    n=64-__builtin_clzll(v);
    int trailing_ones=__builtin_ctzll(~v);
    v >>= trailing_ones;
    n -= trailing_ones;
    return {v,n};
}

static Pos play(Pos p, int x, int y) {
    int middle=y-x-1, suffix_n=p.n-y-1;
    uint64_t suffix_mask=suffix_n ? ((1ull<<suffix_n)-1):0;
    uint64_t suffix=p.bits & suffix_mask;
    uint64_t mm=middle ? ((1ull<<middle)-1):0;
    uint64_t mb=(p.bits>>(suffix_n+1)) & mm;
    int zeros=middle-__builtin_popcountll(mb), ones=middle-zeros+1;
    uint64_t prefix=x ? p.bits>>(p.n-x):0;
    uint64_t out=prefix;
    out <<= zeros+1;
    out=(out<<ones)|((1ull<<ones)-1);
    out=(out<<suffix_n)|suffix;
    return clean_bits(out,p.n);
}

// Interior corner moves (x>0 and y before the last zero) retain both the
// leading one and trailing zero, so normalization is provably a no-op.
static uint64_t play_interior_bits(Pos p, int x, int y) {
    int middle=y-x-1, suffix_n=p.n-y-1;
    uint64_t suffix_mask=(1ull<<suffix_n)-1;
    uint64_t suffix=p.bits & suffix_mask;
    uint64_t mm=middle ? ((1ull<<middle)-1):0;
    uint64_t mb=(p.bits>>(suffix_n+1)) & mm;
    int zeros=middle-__builtin_popcountll(mb), ones=middle-zeros+1;
    uint64_t out=p.bits>>(p.n-x);
    out <<= zeros+1;
    out=(out<<ones)|((1ull<<ones)-1);
    return (out<<suffix_n)|suffix;
}

static uint64_t conjugate(uint64_t v, int n) {
    uint64_t r=0;
    for(int i=0;i<n;i++) { r=(r<<1)|((v&1ull)^1ull); v>>=1; }
    return r;
}

struct FlatSet {
    std::vector<uint64_t> slots;
    size_t used=0;
    static uint64_t hash(uint64_t x){x^=x>>30;x*=0xbf58476d1ce4e5b9ull;x^=x>>27;x*=0x94d049bb133111ebull;x^=x>>31;return x;}
    explicit FlatSet(size_t expected=0){if(expected)reserve(expected);}
    void reserve(size_t expected){
        size_t cap=1024;while(cap*7/10<expected)cap<<=1;
        if(cap<=slots.size())return;
        std::vector<uint64_t> old=std::move(slots);slots.assign(cap,0);used=0;
        for(uint64_t v:old)if(v)insert(v);
    }
    bool contains_hashed(uint64_t v,uint64_t z)const{
        if(!v)return false;
        if(slots.empty())return false;
        size_t mask=slots.size()-1, i=size_t(z)&mask;
        while(slots[i] && slots[i]!=v)i=(i+1)&mask;
        return slots[i]==v;
    }
    bool contains(uint64_t v)const{return contains_hashed(v,hash(v));}
    void grow(){
        if(slots.empty()){slots.assign(1024,0);return;}
        std::vector<uint64_t> old=std::move(slots);slots.assign(old.size()*2,0);used=0;
        for(uint64_t v:old)if(v)insert(v);
    }
    bool insert(uint64_t v){
        if((used+1)*10>=slots.size()*7)grow();
        size_t mask=slots.size()-1, i=size_t(hash(v))&mask;
        while(slots[i] && slots[i]!=v)i=(i+1)&mask;
        if(slots[i]==v)return false;
        slots[i]=v;++used;return true;
    }
    std::vector<uint64_t> values()const{std::vector<uint64_t> out;out.reserve(used);for(uint64_t v:slots)if(v)out.push_back(v);return out;}
    size_t size()const{return used;}
};

struct ClassShard {
    FlatSet exact;
    std::vector<uint64_t> bloom;
    uint32_t bloom_bits=0;
    static uint64_t mix(uint64_t x){return FlatSet::hash(x);}
    static uint64_t bloom_mask(uint64_t z){
        return (1ull<<((z>>32)&63))|(1ull<<((z>>40)&63))|
               (1ull<<((z>>48)&63))|(1ull<<((z>>56)&63));
    }
    void mark(uint64_t v){
        uint64_t z=mix(v);
        size_t index=size_t(z)&((bloom_bits>>6)-1);
        bloom[index]|=bloom_mask(z);
    }
    void ensure(size_t expected){
        uint32_t wanted=1u<<15;while(uint64_t(wanted)<uint64_t(expected)*12)wanted<<=1;
        if(wanted<=bloom_bits)return;
        bloom_bits=wanted;bloom.assign(bloom_bits/64,0);
        for(uint64_t v:exact.slots)if(v)mark(v);
    }
    void reserve(size_t expected){exact.reserve(expected);ensure(expected);}
    bool maybe_hashed(uint64_t z)const{
        if(!bloom_bits)return false;
        size_t index=size_t(z)&((bloom_bits>>6)-1);
        uint64_t mask=bloom_mask(z);
        return (bloom[index]&mask)==mask;
    }
    bool maybe(uint64_t v)const{return maybe_hashed(mix(v));}
    bool contains(uint64_t v)const{
        uint64_t z=mix(v);
        return maybe_hashed(z)&&exact.contains_hashed(v,z);
    }
    bool insert(uint64_t v){
        if(!exact.insert(v))return false;
        if(uint64_t(bloom_bits)<uint64_t(exact.size())*12)ensure(exact.size()); else mark(v);
        return true;
    }
};

struct DB {
    // The immutable seed gets a compact 4 MiB filter. Added positions use
    // independently sized per-class filters, avoiding global saturation.
    static constexpr uint32_t SEED_BB=1u<<25;
    std::vector<uint32_t> seed;
    static constexpr unsigned SIDE=65;
    std::unique_ptr<ClassShard> added[SIDE*SIDE];
    size_t added_count=0;
    std::vector<uint64_t> seed_bloom;
    uint32_t frontier=0;
    static uint64_t mix(uint64_t x){x^=x>>30;x*=0xbf58476d1ce4e5b9ull;x^=x>>27;x*=0x94d049bb133111ebull;x^=x>>31;return x;}
    static unsigned shard_index(uint64_t v){
        unsigned n=64-__builtin_clzll(v), w=__builtin_popcountll(v), h=n-w;
        return w*SIDE+h;
    }
    void mark_seed(uint64_t v){
        uint64_t z=mix(v);
        size_t base=(size_t(z&((SEED_BB>>9)-1)))<<3;
        for(unsigned k=0;k<3;k++){uint32_t h=uint32_t(z>>(18+k*9))&511;seed_bloom[base+(h>>6)]|=1ull<<(h&63);}
    }
    bool seed_maybe(uint64_t v)const{
        uint64_t z=mix(v);size_t base=(size_t(z&((SEED_BB>>9)-1)))<<3;
        for(unsigned k=0;k<3;k++){uint32_t h=uint32_t(z>>(18+k*9))&511;if(!(seed_bloom[base+(h>>6)]>>(h&63)&1u))return false;}
        return true;
    }
    explicit DB(const std::string& path):seed_bloom(SEED_BB/64){
        std::ifstream f(path,std::ios::binary); char m[8]; uint32_t c;
        f.read(m,8);f.read((char*)&c,4);f.read((char*)&frontier,4);
        if(!f||std::string(m,8)!="CHOMPP01") throw std::runtime_error("bad database");
        seed.resize(c);f.read((char*)seed.data(),std::streamsize(c)*4);
        for(auto v:seed)mark_seed(v);
    }
    bool has(uint64_t v)const{
        if(!v)return false;
        if(v<=frontier)return seed_maybe(v)&&std::binary_search(seed.begin(),seed.end(),uint32_t(v));
        const auto& s=added[shard_index(v)];return s&&s->contains(v);
    }
    bool has_class(uint64_t v,unsigned w,unsigned h)const{
        if(!v)return false;
        if(v<=frontier)return seed_maybe(v)&&std::binary_search(seed.begin(),seed.end(),uint32_t(v));
        if(w>=SIDE||h>=SIDE)return false;
        const auto& s=added[w*SIDE+h];return s&&s->contains(v);
    }
    bool add(uint64_t v){
        if(!v||v<=frontier)return false;
        unsigned i=shard_index(v);if(!added[i])added[i]=std::make_unique<ClassShard>();
        if(!added[i]->insert(v))return false;
        ++added_count;return true;
    }
    void reserve_class(unsigned w,unsigned h,size_t expected){
        unsigned i=w*SIDE+h;if(!added[i])added[i]=std::make_unique<ClassShard>();added[i]->reserve(expected);
    }
    const ClassShard* class_shard(unsigned w,unsigned h)const{
        if(w>=SIDE||h>=SIDE)return nullptr;
        return added[w*SIDE+h].get();
    }
    size_t size()const{return added_count;}
    std::vector<uint64_t> values()const{
        std::vector<uint64_t> out;out.reserve(added_count);
        for(const auto& s:added)if(s){auto v=s->exact.values();out.insert(out.end(),v.begin(),v.end());}
        return out;
    }
};

static bool basic_sieved(uint64_t v,int n,int w,int h){
    uint64_t rectangle = (((1ull<<w)-1) << h);
    uint64_t skeleton = (1ull<<(n-1)) | (((1ull<<(w-1))-1)<<1);
    bool pointed=((v>>(n-3))==5u)||((v&7u)==2u);
    if(v==rectangle) return true;
    if(w==2||h==2) return !pointed;
    if(pointed) return true;
    if(w==h) return v!=skeleton;
    return v==skeleton;
}

static uint64_t choose_u64(unsigned n,unsigned k){
    if(k>n)return 0;if(k>n-k)k=n-k;__uint128_t r=1;
    for(unsigned i=1;i<=k;i++)r=r*(n-k+i)/i;
    return uint64_t(r);
}

enum class Hit { none, width, height, corner };
// Boundary moves normalize directly. Top-edge moves remove j+1 rows;
// right-edge moves retain the prefix before the ith one and remove width-i
// columns. Their empty poison move endpoints cannot hit the database.
static uint64_t height_move_bits(uint64_t bits,uint64_t edge,uint64_t cut){
    return (edge-cut)|(bits&(cut-1));
}
static uint64_t width_move_bits(uint64_t prefix,int width,int one_index){
    return prefix>>(width-one_index);
}
static Hit expansion_hit(Pos p,const DB& db){
    const int no=__builtin_popcountll(p.bits),nz=p.n-no;
    const uint64_t leading=1ull<<(p.n-1);
    uint64_t zeros=(~p.bits)&(leading-1)&~1ull;
    uint64_t edge=leading;
    int remaining_height=nz-1;
    while(zeros){
        uint64_t cut=1ull<<(63-__builtin_clzll(zeros));
        uint64_t q=height_move_bits(p.bits,edge,cut);
        if(db.has_class(q,no,remaining_height))return Hit::height;
        zeros^=cut;edge>>=1;--remaining_height;
    }
    uint64_t ones=p.bits^leading,prefix=leading;
    int one_index=1;
    while(ones){
        uint64_t cut=1ull<<(63-__builtin_clzll(ones));
        uint64_t q=width_move_bits(prefix,no,one_index);
        if(db.has_class(q,one_index,nz))return Hit::width;
        ones^=cut;prefix|=cut;++one_index;
    }
    return Hit::none;
}

static bool corner_hit(Pos p,const DB& db,const ClassShard* shard,uint64_t floor=0){
    // A move's first changed bit is its selected one (1 -> 0). Any one
    // above the highest p/floor difference would put the result below floor.
    if(p.bits<=floor)return false;
    const uint64_t leading=1ull<<(p.n-1);
    const uint64_t eligible_mask=~0ull>>__builtin_clzll(p.bits^floor);
    uint64_t ones=p.bits&eligible_mask&(leading-1);
    if(!ones)return false;
    const int no=__builtin_popcountll(p.bits),nz=p.n-no;
    int xi=no-__builtin_popcountll(ones);
    uint64_t prefix=p.bits^ones;
    const uint64_t all_zeros=(~p.bits)&(leading-1)&~1ull;
    while(ones){
        const uint64_t one=1ull<<(63-__builtin_clzll(ones));
        // Preserve V7's order: selected ones from high to low, then zero
        // endpoints from low to high, omitting the terminal boundary zero.
        uint64_t zeros=all_zeros&(one-1);
        uint64_t edge=1ull<<(no+1-xi);
        while(zeros){
            const uint64_t cut=zeros&-zeros;
            const uint64_t q=prefix|(edge-cut)|(p.bits&(cut-1));
            if(q>=floor && (shard ? shard->contains(q) : db.has_class(q,no,nz)))return true;
            zeros^=cut;edge<<=1;
        }
        prefix|=one;ones^=one;++xi;
    }
    return false;
}

static uint64_t delta_checksum(int w,int h,const std::vector<uint64_t>& v){
    uint64_t x=1469598103934665603ull;
    auto feed=[&](uint64_t z){for(int i=0;i<8;i++){x^=uint8_t(z);x*=1099511628211ull;z>>=8;}};
    feed(uint64_t(w));feed(uint64_t(h));feed(v.size());for(uint64_t z:v)feed(z);return x;
}

static void checkpoint(const std::string& path,std::vector<uint64_t> delta,int done_i,int done_j){
    std::sort(delta.begin(),delta.end());
    bool fresh=true;{std::ifstream in(path,std::ios::binary);char m[8];fresh=!(in.read(m,8)&&std::string(m,8)=="CHOMPCP2");}
    if(fresh){std::ofstream out(path,std::ios::binary|std::ios::trunc);out.write("CHOMPCP2",8);}
    std::ofstream out(path,std::ios::binary|std::ios::app);
    const char record[4]={'C','L','A','S'};uint16_t w=done_i,h=done_j;uint64_t count=delta.size(),sum=delta_checksum(w,h,delta);
    out.write(record,4);out.write((char*)&w,2);out.write((char*)&h,2);out.write((char*)&count,8);out.write((char*)&sum,8);
    out.write((char*)delta.data(),std::streamsize(delta.size()*sizeof(uint64_t)));out.flush();
    if(!out)throw std::runtime_error("checkpoint write failed");
}

static std::pair<int,int> load_checkpoint(const std::string& path,DB& db){
    std::ifstream in(path,std::ios::binary);char magic[8];
    if(!in.read(magic,8))return {0,0};
    if(std::string(magic,8)=="CHOMPCP2"){
        int last_w=0,last_h=0;
        for(;;){
            char rec[4];uint16_t w,h;uint64_t count,sum;
            if(!in.read(rec,4))break;
            if(std::string(rec,4)!="CLAS"||!in.read((char*)&w,2)||!in.read((char*)&h,2)||
               !in.read((char*)&count,8)||!in.read((char*)&sum,8)||count>(1ull<<32))break;
            std::vector<uint64_t> v(count);if(!in.read((char*)v.data(),std::streamsize(count*8)))break;
            if(delta_checksum(w,h,v)!=sum)break;
            for(uint64_t x:v)db.add(x);last_w=w;last_h=h;
        }
        return {last_w,last_h};
    }
    // Backward-compatible reader for the earlier text checkpoint.
    in.close();std::ifstream text(path);std::string tag,done;int w=0,h=0;
    if(text>>tag>>done>>w>>h){uint64_t v;while(text>>v){db.add(v);db.add(conjugate(v,64-__builtin_clzll(v)));}}
    return {w,h};
}

int main(int argc,char**argv){
 try{
    if(argc<2){std::cerr<<"usage: chomp_sieve_v8_candidate ppositions.bin [max-dimension] [checkpoint] [preload-checkpoint] [only-width] [only-height] [threads]\n";return 2;}
    int dim=argc>2?std::stoi(argv[2]):16;std::string cp=argc>3?argv[3]:"sieve_checkpoint.txt";
    DB db(argv[1]);
    if(argc>4&&std::string(argv[4]).size())load_checkpoint(argv[4],db);
    int only_width=argc>5?std::stoi(argv[5]):0;
    int only_height=argc>6?std::stoi(argv[6]):0;
    unsigned workers=argc>7?unsigned(std::stoi(argv[7])):std::thread::hardware_concurrency();
    if(!workers)workers=1;
    auto resume=load_checkpoint(cp,db);int resume_i=resume.first,resume_j=resume.second;
    uint64_t tested=0,basic=0,known=0,sw=0,sh=0,sc=0;auto start=std::chrono::steady_clock::now(),milestone=start;
    for(int h=3;h<=dim;h++)for(int w=h+1;w<=dim;w++){
      if(only_width && w!=only_width)continue;
      if(only_height && h!=only_height)continue;
      if(h<resume_j||(h==resume_j&&w<=resume_i))continue;
      int n=w+h, k=w-1, m=n-2;
      if((1ull<<(n-1))>db.frontier){
        size_t expected=std::max<uint64_t>(1024,choose_u64(m,k)/128);
        db.reserve_class(w,h,expected);db.reserve_class(h,w,expected);
      }
      uint64_t comb=(1ull<<k)-1, limit=1ull<<m, class_tested=0, class_new=0;
      const ClassShard* current_class=db.class_shard(w,h);
      std::vector<uint64_t> class_delta;
      struct Work { uint64_t v; Hit hit=Hit::none; };
      constexpr size_t CHUNK=1u<<14;
      while(comb<limit){
        std::vector<Work> batch;batch.reserve(CHUNK);
        while(comb<limit&&batch.size()<CHUNK){
          uint64_t v=(1ull<<(n-1))|(comb<<1);
          uint64_t c=comb&-comb, r=comb+c;comb=((r^comb)>>(__builtin_ctzll(comb)+2))|r;
          if(v<=db.frontier){++known;continue;}
          if(basic_sieved(v,n,w,h)){++basic;continue;}
          batch.push_back({v,Hit::none});++tested;++class_tested;
        }
        unsigned active=std::min<unsigned>(workers,unsigned(batch.size()));
        std::atomic<size_t> next{0};
        std::vector<std::thread> pool;pool.reserve(active);
        for(unsigned t=0;t<active;t++)pool.emplace_back([&,t]{
          static constexpr size_t STRIDE=128;
          for(size_t lo=next.fetch_add(STRIDE,std::memory_order_relaxed);lo<batch.size();lo=next.fetch_add(STRIDE,std::memory_order_relaxed)){
            size_t hi=std::min(batch.size(),lo+STRIDE);
            for(size_t q=lo;q<hi;q++) {
                batch[q].hit=expansion_hit({batch[q].v,n},db);
                // The database is immutable while workers run. A corner hit
                // here remains valid. Misses must be retried in original order.
                if(batch[q].hit==Hit::none && corner_hit({batch[q].v,n},db,current_class))
                    batch[q].hit=Hit::corner;
            }
          }
        });
        for(auto&t:pool)t.join();
        // Corner dependencies remain sequential and in the original order.
        for(const Work& q:batch){
          if(q.hit==Hit::width){++sw;continue;}
          if(q.hit==Hit::height){++sh;continue;}
          if(q.hit==Hit::corner){++sc;continue;}
          // Workers already excluded every old database entry. During this
          // ordered pass, new same-class keys are all >= the first batch key;
          // conjugates belong to the opposite class because w > h.
          if(corner_hit({q.v,n},db,current_class,batch.front().v)){++sc;continue;}
          if(db.add(q.v))class_delta.push_back(q.v);
          uint64_t qc=conjugate(q.v,n);if(db.add(qc))class_delta.push_back(qc);
          ++class_new;
        }
      }
      checkpoint(cp,std::move(class_delta),w,h);
      auto now=std::chrono::steady_clock::now();
      double sec=std::chrono::duration<double>(now-start).count();
      std::cout<<"class "<<w<<'x'<<h<<" tested="<<class_tested<<" new="<<class_new
               <<" total_new_with_conjugates="<<db.size()<<" rate="<<uint64_t(tested/sec)<<"/s"
               <<" elapsed="<<format_time(sec)<<"\n";
      if(w==h+1){
        double split=std::chrono::duration<double>(now-milestone).count();
        std::cout<<"MILESTONE coverage "<<w<<'x'<<w<<" reached"
                 <<" split="<<format_time(split)<<" elapsed="<<format_time(sec)<<"\n";
        milestone=now;
      }
      std::cout<<std::flush;
    }
    std::cout<<"done tested="<<tested<<" basic="<<basic<<" known="<<known
             <<" width="<<sw<<" height="<<sh<<" corner="<<sc<<" added="<<db.size()<<'\n';
 }catch(const std::exception&e){std::cerr<<"error: "<<e.what()<<'\n';return 1;}
}

static bool corner_hit_reference(Pos p,const DB& db,const ClassShard* shard,uint64_t floor=0){
    int ones[64],zeros[64],no=0,nz=0;
    uint64_t suffix[64],cut[64],edge[64];
    for(int i=0;i<p.n;i++) {
        if((p.bits>>(p.n-1-i))&1ull) ones[no++]=i;
        else {
            zeros[nz]=i;
            cut[nz]=1ull<<(p.n-i-1);
            suffix[nz]=p.bits&(cut[nz]-1);
            edge[nz]=1ull<<(p.n-1-nz);
            ++nz;
        }
    }
    for(int xi=1;xi<no;xi++) {
        uint64_t prefix=p.bits&(~0ull<<(p.n-ones[xi]));
        for(int zi=nz-2;zi>=0;--zi) {
            if(zeros[zi]<=ones[xi])break;
            uint64_t q=prefix|((edge[zi]>>xi)-cut[zi])|suffix[zi];
            if(q>=floor && (shard ? shard->contains(q) : db.has_class(q,no,nz))) return true;
        }
    }
    return false;
}

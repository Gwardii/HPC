    % 01a
    a7f0:	vmovupd ymm0,YMMWORD PTR [rdx+rax*1]
    a7f5:	vaddpd ymm0,ymm0,ymm0
    a7f9:	vmovupd YMMWORD PTR [r12+rax*1],ymm0
    a7ff:	add    rax,0x20
    a803:	cmp    rcx,rax
    a806:	jne    a7f0 <BM_vec_mult2(benchmark::State&)+0x140>

    %01b
    a800:	vmovupd ymm0,YMMWORD PTR [rdx+rax*1]
    a805:	vmulpd ymm1,ymm0,ymm3
    a809:	vfmadd132pd ymm0,ymm1,ymm0
    a80e:	vmovupd YMMWORD PTR [r12+rax*1],ymm0
    a814:	add    rax,0x20
    a818:	cmp    rcx,rax
    a81b:	jne    a800 <BM_vec_quad(benchmark::State&)+0x150>

    %01c
    a9e0:	vmovupd ymm0,YMMWORD PTR [rcx+rax*1]
    a9e5:	vmulpd ymm1,ymm0,ymm3
    a9e9:	vfmadd132pd ymm0,ymm1,ymm0
    a9ee:	vmovupd YMMWORD PTR [rdx+rax*1],ymm0
    a9f3:	add    rax,0x20
    a9f7:	cmp    rax,rsi
    a9fa:	jne    a9e0 <BM_vec_quad(benchmark::State&)+0x300>

    %01d
    a9e0:	vmovapd ymm0,YMMWORD PTR [r13+rax*1+0x0]
    a9e7:	vmulpd ymm1,ymm0,ymm3
    a9eb:	vfmadd132pd ymm0,ymm1,ymm0
    a9f0:	vmovupd YMMWORD PTR [r12+rax*1],ymm0
    a9f6:	add    rax,0x20
    a9fa:	cmp    rdx,rax
    a9fd:	jne    a9e0 <BM_vec_quad(benchmark::State&)+0x300>

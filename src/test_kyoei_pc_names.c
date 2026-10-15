#include "kyoei_pc_names.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"PC names failed: %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char **argv){
    CHECK(argc==3);_putenv_s("KYOEI_NAME_STORE",argv[1]);
    const uint16_t name[]={'A','l','e','x','a','n','d','e','r',0};uint16_t token[5],again[5],out[25];
    if(!strcmp(argv[2],"write")){
        FILE *f=fopen(argv[1],"rb");if(f){fclose(f);return 2;}
        CHECK(kyoei_pc_name_store(name,9,token));CHECK(token[0]=='Z'&&!token[4]);
        CHECK(kyoei_pc_name_store(name,9,again)&&!memcmp(token,again,10));
        CHECK(kyoei_pc_name_lookup(token,4,out)&&!memcmp(out,name,sizeof(name)));
    }else{
        const uint16_t saved[]={'Z','A','A','A',0};
        CHECK(kyoei_pc_name_lookup(saved,4,out)&&!memcmp(out,name,sizeof(name)));
        CHECK(!kyoei_pc_name_lookup(saved,3,out));
        CHECK(remove(argv[1])==0);
    }
    puts("PC NAME STORE PASS: full nine-letter name, four-unit token, reuse and cross-process persistence");return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "bloom_filter.h"

unsigned char* bloom_filter_init(unsigned char* bloomtable,int M,int k){

   bloomtable = malloc(sizeof(unsigned char)*M);

   for(int i = 0 ; i < M; i++){
      bloomtable[i] = 0;
   }

   return bloomtable;

}

unsigned char* bloom_filter_insert(unsigned char* bloomtable,int M,int k,unsigned char *data){

   int index;

   for(int i = 0 ; i < k; i++) {
      index = hash_i(data,i) % M;
      bloomtable[index] = 1;
   }

   return bloomtable;

}

char bloom_filter_search(unsigned char* bloomtable,int M,int k,unsigned char *data){

   int index,count = 0;

   for(int i = 0 ; i < k; i++) {
      index = hash_i(data,i) % M;
      if(bloomtable[index] == 1)
         count += 1;
   }

   if(count == k)
      return 0;
   else if(count < k);
      return 1;

}



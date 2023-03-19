#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "hash.h"
#include "structs.h"
#include "bloom_filter.h"
#include "skip_list.h"
#include "funcs.h"
#define BUFLEN 256


void main(int argc,char** argv){

   FILE* fp = NULL;
   list* l;
   int count = 0,recsizetotal,hashsize;
   datarec rec;
   unsigned char* bloomtable;
   char str1[10];
   char value[10];
   char flag,str[200],id[10],virusname[10] = "  ",country[10] = "  ",temp[12],*token = " ",command[200],*filename;
   hs* hash;
   ht* hashv;
   ht* listvir = NULL;
   ht* countries = NULL;
   dr* r = NULL;
   dr** rrec;
   int iter = 1,iterval = 1,i;
   int index,bloomsize;
   char allstr[200];
   hov* temphov;
   ht* target;
   datarec *record;
   time_t rawtime;
   struct tm *ptm;
   sp* sup;

   /* Control if arguments are different from default argc = 5 */
   if(argc == 5){
      if(strncmp(argv[1],"-c",strlen(argv[1])) == 0 && strncmp(argv[3],"-b",strlen(argv[3])) == 0){
         filename = malloc(strlen(argv[2])+1);
         strcpy(filename,argv[2]);
         hashsize = findsizehash(listvir,filename);
         bloomsize = atoi(argv[4]);
      }
      else if(strncmp(argv[1],"-b",strlen(argv[1])) == 0 && strncmp(argv[3],"-c",strlen(argv[3])) == 0){
         filename = malloc(strlen(argv[4])+1);
         strcpy(filename,argv[4]);
         hashsize = findsizehash(listvir,filename);
         bloomsize = atoi(argv[2]);
      }
      else{
         printf("Error false flags \nUsage ./vaccineMonitor -c recordfile -b bloosize or ./vaccineMonitor -b bloosize -c recordfile \n");
         return;
      }
   }
   else{
      printf("Error different number of arguments \nUsage ./vaccineMonitor -c recordfile -b bloosize \n");
      return;
   }

   sup = malloc(sizeof(sp));
   hash = malloc(sizeof(hs)*hashsize);

   fp = fopen(filename,"r");

   if(fp == NULL){
      printf("Error file not open");
      return;
   }

   fseek(fp,0,SEEK_END);

   recsizetotal = ftell(fp);

   fclose(fp);

   record = malloc(recsizetotal*sizeof(record));

   for(int i = 0; i < hashsize; i++){
      hash[i].nam = malloc(2*sizeof(char));
      strncpy(hash[i].nam,"  ",2);
      hash[i].bloomfilter = bloom_filter_init(hash[i].bloomfilter,bloomsize,3);
      hash[i].vaccinated = listinit(hash[i].vaccinated);
      hash[i].novaccinated = listinit(hash[i].novaccinated);
      hash[i].listover = NULL;
   }

   //sup->hashv = hasinit(sup->hashv);

   //sup->r = NULL;

   countries = readfiledata(filename,hash,sup->hashv,rrec,hashsize,countries,record,recsizetotal,bloomsize);

   printf("Enter your command:\n");

   while(1){

      scanf("%s",command);

      if(strncmp(command,"/vaccineStatusBloom",19) == 0){

         fgets(str,100,stdin);
         sscanf(str," %s %s",id,virusname);

         vaccineStatusBloom(id,virusname,hash,sup->hashv,hashsize,bloomsize);

         strncpy(virusname,"  ",1);
      }
      else if(strncmp(command,"/vaccineStatus",14) == 0){

         fgets(str,100,stdin);
         sscanf(str," %s %s",id,virusname);

         if(strncmp(virusname," ",1) != 0){

            vaccineStatus(id,virusname,hash,sup->hashv,hashsize,command);
         }
         else if(strncmp(virusname," ",1) == 0){

            sscanf(str,"%s",id);

            vaccineStatusall(id,hash,sup->hashv,hashsize,command);
         }

         strncpy(virusname,"  ",1);
      }
      else if(strncmp(command,"/list-nonVaccinated-Persons",27) == 0){

         scanf(" %s",virusname);

         novaccineStatus(virusname,hash,sup->hashv,hashsize);

         strncpy(virusname,"  ",1);
      }
      else if(strncmp(command,"/insertCitizenrecord",20) == 0){

         fgets(str,200,stdin);

         sscanf(str," %s %s %s %s %s %s %s %s",value,rec.name,rec.surname,rec.country,str1,rec.virname,rec.vacflag,rec.date);

         rec.id = atoi(value);
         rec.age = atoi(str1);

         hash = insertCitizenrecord(rec,hash,sup->hashv,r,hashsize,command,countries,bloomsize,value);

         strncpy(virusname,"  ",1);
      }
      else if(strncmp(command,"/vaccinateNow",13) == 0){

         fgets(str,100,stdin);

         sscanf(str," %s %s %s %s %s %s",value,rec.name,rec.surname,rec.country,str1,rec.virname);

         rec.id = atoi(value);
         rec.age = atoi(str1);
         strncpy(rec.vacflag,"YES",3);

         rawtime = time(NULL);
         ptm = localtime(&rawtime);
         strftime(rec.date,BUFLEN,"%D",ptm);

         /* strncpy(id,rec.date+3,2);
         strncpy(temp,rec.date,2);
         strncpy(rec.date,id,2);
         strncpy(rec.date,temp,2); */
         strncpy(temp,rec.date+6,2);
         strncpy(rec.date+6,"20",2);
         strncpy(rec.date+8,temp,2);

         rec.date[2] = '-';
         rec.date[5] = '-';

         hash = insertCitizenrecord(rec,hash,sup->hashv,r,hashsize,command,countries,bloomsize,value);

         strncpy(temp,"  ",1);
         strncpy(virusname,"  ",1);
      }
      else if(strncmp(command,"/populationStatus",17) == 0){

          fgets(str,100,stdin);

          strcpy(command,str);

          token = strtok(command," ");

          while(token != NULL){
             token = strtok(NULL," ");
             count++;
          }
          if(count == 3){
             sscanf(str," %s %s %s",virusname,temp,rec.date);
             populationstatus(hash,virusname,hashsize,countries,rec,temp,country,0,rec.date);
          }
          else if(count == 4){
             sscanf(str," %s %s %s %s",country,virusname,temp,rec.date);
             printf("%s %s %s %s \n",country,virusname,temp,rec.date);
             populationstatus(hash,virusname,hashsize,countries,rec,temp,country,0,rec.date);
          }

          strncpy(rec.date,"  ",1);
          strncpy(temp,"  ",1);
          strncpy(country,"  ",1);
          strncpy(virusname,"  ",1);

          count = 0;
      }
      else if(strncmp(command,"/popStatusByAge",15) == 0){

          fgets(str,100,stdin);

          strncpy(command,str,strlen(str));

          token = strtok(command," ");

          while(token != NULL){
             token = strtok(NULL," ");
             count++;
          }
          if(count == 3){
             sscanf(str," %s %s %s",virusname,temp,rec.date);
             populationstatus(hash,virusname,hashsize,countries,rec,temp,country,1,rec.date);
          }
          else if(count == 4){
             sscanf(str," %s %s %s %s",country,virusname,temp,rec.date);
             printf("%s %s %s %s \n",country,virusname,temp,rec.date);
             populationstatus(hash,virusname,hashsize,countries,rec,temp,country,1,rec.date);
          }
          else
             printf("ERROR few arguments\n Usage: populationStatus or popStatusByAge with [country] virusname date1 date2 \n");

          strncpy(rec.date,"  ",1);
          strncpy(temp,"  ",1);
          strncpy(country,"  ",1);
          strncpy(virusname,"  ",1);

          count = 0;
      }
      else if(strncmp(command,"/exit",5) == 0){
         free(sup);
         free(record);
         freestructs(hash,hashsize,countries);
         return;
      }

   }

}

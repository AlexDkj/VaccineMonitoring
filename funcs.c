#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "hash.h"
#include "structs.h"
#include "bloom_filter.h"
#include "skip_list.h"

/* Supported functions for findsizehash */
ht* hasinsert(ht* h,char* virusname,int iter){

   if(h == NULL){
      h = malloc(sizeof(ht));
      h->name = malloc(strlen(virusname));
      strncpy(h->name,virusname,strlen(virusname));
      h->iter = iter;
      h->next = NULL;
   }
   else
      h->next = hasinsert(h->next,virusname,iter);

   return h;
}

/* Supported functions for findsizehash */
int hassearch(ht* h,char* virusname){

   ht* temp;
   int flag = 0;

   temp = h;

   while(temp != NULL){

      if(strncmp(virusname,temp->name,strlen(virusname)) == 0){
         flag = temp->iter;
         break;
      }
      temp = temp->next;
   }

   return flag;
}

/* Find sum of viruses */
int findsizehash(ht* listvir,char* file){

   FILE* fp = NULL;
   char allstr[100];
   char value[10];
   char str[5];
   datarec rec;
   int countvir = 0,iter;
   ht* temp;

   fp = fopen(file,"r");

   if(fp == NULL){
      printf("Error file not open");
      return countvir;
   }

   listvir = hasinit(listvir);

   while(fgets(allstr,100,fp) != NULL){

      sscanf(allstr,"%s %s %s %s %s %s %s %s",value,rec.name,rec.surname,rec.country,str,rec.virname,rec.vacflag,rec.date);

      /* See if list of viruses contain reading virus */
      if((iter = hassearch(listvir,rec.virname)) == 0){
         listvir = hasinsert(listvir,rec.virname,1);
         countvir++;
      }
      else
         continue;
   }

   /* free list of viruses */
   while(listvir != NULL){
      free(listvir->name);
      temp = listvir->next;
      free(listvir);
      listvir = temp;
   }

   fclose(fp);

   return countvir;
}

/* Overflow list for hashtable, if not be empty bucket not used */
hov* hashoverlist(hov* listover,char* str,datarec rec,dr* r,char* valid,ht* countries,hs hash,int bloomsize){

   if(strncmp(str,listover->nam,strlen(str)) == 0){
      if(strncmp(rec.vacflag,"YE",2) == 0){
          listover->vaccinated = listinsert(listover->vaccinated,rec.id,0,listover->vaccinated,rec,r,countries);
          listover->bloomfilter = bloom_filter_insert(listover->bloomfilter,bloomsize,3,valid);
          free(listover->nam);
          listover->nam = malloc(strlen(str));
          strncpy(listover->nam,str,strlen(str));
      }
      else if(strncmp(rec.vacflag,"NO",2) == 0){
          listover->novaccinated = listinsert(listover->novaccinated,rec.id,0,listover->novaccinated,rec,r,countries);
          free(listover->nam);
          listover->nam = malloc(strlen(str));
          strncpy(listover->nam,str,strlen(str));
      }
   }
   else
      listover->next = hashoverlist(listover->next,str,rec,r,valid,countries,hash,bloomsize);

   return listover;
}

/* Read from file of records and insert data in the structs */
ht* readfiledata(char* file,hs* hash,ht* hashv,dr** rs,int sizeofhash,ht* countries,datarec* reccheck,int recsizetotal,int bloomsize){

   FILE* fp = NULL;
   int iter = 1,iterval = 1,count = 0,i;
   int index;
   char allstr[200];
   char str[5];
   char strtemp[10];
   char value[10];
   datarec rec;
   hov* temphov = NULL;
   ht* target = NULL;
   char flag;
   dr* temp,*r = NULL;
   //datarec* reccheck;

   //reccheck = malloc(recsizetotal*sizeof(dr));

   fp = fopen(file,"r");

   if(fp == NULL){
      printf("Error file not open");
      return hashv;
   }

   strncpy(rec.date,"  ",2);

   while(fgets(allstr,100,fp) != NULL){

      strncpy(rec.date,"  ",2);

      flag = 0;

      sscanf(allstr,"%s %s %s %s %s %s %s %s",value,rec.name,rec.surname,rec.country,str,rec.virname,rec.vacflag,rec.date);
      sscanf(allstr,"%s %s %s %s %s %s %s %s",value,reccheck[count].name,reccheck[count].surname,reccheck[count].country,str,reccheck[count].virname,reccheck[count].vacflag,reccheck[count].date);

      rec.id = atoi(value);
      rec.age = atoi(str);

      reccheck[count].id = atoi(value);
      reccheck[count].age = atoi(str);

      countries = listofcountries(countries,rec.country,target);

      if(strncmp(rec.date," ",1) != 0 && strncmp(rec.vacflag,"NO",2) == 0){
         printf("ERROR IN RECORD \n");
         continue;
      }

      for(i = 0; i < count-1; i++){
         if(reccheck[i].id == rec.id && strncmp(rec.name,reccheck[i].name,strlen(rec.name)) == 0 && strncmp(rec.surname,reccheck[i].surname,strlen(rec.surname)) == 0 && strncmp(rec.virname,reccheck[i].virname,strlen(rec.virname)) == 0){
            flag = 1;
            printf("ERROR IN RECORD line %d \n",count);
            break;
         }
      }

      if(flag == 1)
         continue;

      strcpy(strtemp,rec.virname);

      index = sdbm(strtemp) % sizeofhash;

      if(strncmp(rec.vacflag,"YE",2) == 0){

         if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) != 0) && (strncmp(hash[index].nam," ",1) != 0)){

            hash[index].listover = hashoverinsert(hash[index].listover,rec,strtemp);
            hash[index].listover = hashoverlist(hash[index].listover,strtemp,rec,r,value,countries,hash[index],bloomsize);
         }
         else if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) == 0) || (strncmp(hash[index].nam," ",1) == 0)){

            hash[index].vaccinated = listinsert(hash[index].vaccinated,rec.id,0,hash[index].vaccinated,rec,r,countries);
            hash[index].bloomfilter = bloom_filter_insert(hash[index].bloomfilter,bloomsize,3,value);

            free(hash[index].nam);
            hash[index].nam = malloc(strlen(strtemp));
            strncpy(hash[index].nam,strtemp,strlen(strtemp));
         }

      }
      else if(strncmp(rec.vacflag,"NO",2) == 0){

         if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) != 0) && (strncmp(hash[index].nam," ",1) != 0)){

            hash[index].listover = hashoverinsert(hash[index].listover,rec,strtemp);
            hash[index].listover = hashoverlist(hash[index].listover,strtemp,rec,r,value,countries,hash[index],bloomsize);
         }
         else if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) == 0) || (strncmp(hash[index].nam," ",1) == 0)){
            hash[index].novaccinated = listinsert(hash[index].novaccinated,rec.id,0,hash[index].novaccinated,rec,r,countries);

            free(hash[index].nam);
            hash[index].nam = malloc(strlen(strtemp));
            strncpy(hash[index].nam,strtemp,strlen(strtemp));
         }
      }

      count++;

   }

   for(int i = 0; i < sizeofhash; i++){

      if(hash[i].vaccinated != NULL)
         hash[i].vaccinated = skip_list(hash[i].vaccinated,3);
      if(hash[i].novaccinated != NULL)
         hash[i].novaccinated = skip_list(hash[i].novaccinated,3);
      if(hash[i].listover != NULL)
         hash[i].listover = skip_list_level_assign(hash[i].listover);
   }

   printf("Inserted data completed\n");

   fclose(fp);

   return countries;
}

/* Function who search specific bloomfilter for specific virus and id of person and print maybe if find them vaccinated or not vaccinated */
void vaccineStatusBloom(char* id,char* virusname,hs* hash,ht* hashv,int sizeofhash,int bloomsize){

   int index,iter;
   char flag = 1;
   hov* temp;


   index = sdbm(virusname) % sizeofhash;

   if(strncmp(virusname,hash[index].nam,3) == 0)
      flag = bloom_filter_search(hash[index].bloomfilter,bloomsize,3,id);
   else{
      temp = hash[index].listover;
      while(temp != NULL){
         if(strncmp(virusname,temp->nam,3) == 0){
            flag = bloom_filter_search(temp->bloomfilter,bloomsize,3,id);
            break;
         }
         temp = temp->next;
      }
   }

   if(flag == 0)
      printf("MAYBE\n");
   else if(flag == 1)
      printf("NOT VACCINATED\n");

}

/* Search for specific id and virus, if person with id has vaccinated for the virus */
void vaccineStatus(char* id,char* virusname,hs* hash,ht* hashv,int sizeofhash,char* command){

   int index,iter=1,result;
   hov* temp;

   index = sdbm(virusname) % sizeofhash;

   if(strncmp(virusname,hash[index].nam,strlen(virusname)) != 0){

      temp = hash[index].listover;

      while(temp != NULL){
         if(strncmp(virusname,temp->nam,strlen(virusname)) == 0){
            skip_list_search(temp->vaccinated,atoi(id),command);
            return;
         }
         temp = temp->next;
      }
   }
   else
      skip_list_search(hash[index].vaccinated,atoi(id),command);
}

/* Search specific id, for all virus in the hashtable, if has vaccinated or not and print all */
void vaccineStatusall(char* id,hs* hash,ht* hashv,int sizeofhash,char* command){

   int index;
   char flag;
   hov* temp;

   strncpy(command,"vaccinepersons",17);

   for(int i = 0; i < sizeofhash; i++){
      if(strncmp(hash[i].nam," ",1) != 0){
         printf("%s ",hash[i].nam);
         skip_list_search(hash[i].vaccinated,atoi(id),command);
      }
      temp = hash[i].listover;
      while(temp != NULL){
         printf("%s ",temp->nam);
         skip_list_search(temp->vaccinated,atoi(id),command);
         temp = temp->next;
      }
   }

}

/* Supported function to populationsearch */
int addcategory(list* l,int addtab[4]){

   int index;

   if(l->otherdata->age <= 20){
      addtab[0] += 1;
      index = 0;
   }
   else if(l->otherdata->age > 20 && l->otherdata->age <= 40){
      addtab[1] += 1;
      index = 1;
   }
   else if(l->otherdata->age > 40 && l->otherdata->age <= 60){
      addtab[2] += 1;
      index = 2;
   }
   else{
      addtab[3] += 1;
      index = 3;
   }

   return index;
}

int populationsearch(hs hash,char* virusname,char* date,ht* temp,int flag,char* date2){

   hov* temp2;
   list* l;
   char country[10];
   int sumc = 0,agesum[4],agetotal[4],index;
   float percentage = 0.0;

   for(int i = 0; i < 4; i++){
      agesum[i] = 0;
      agetotal[i] = 0;
   }

   if(strncmp(virusname,hash.nam,strlen(virusname)) != 0){
         temp2 = hash.listover;

         while(temp2 != NULL){
            if(strncmp(virusname,temp2->nam,strlen(virusname)) == 0){
               l = temp2->vaccinated;
               while(l != NULL){
                  index = addcategory(l,agetotal);
                  agetotal[index] += 1;
                  if(strcmp(temp->name,l->otherdata->countries->name) == 0 && (strcmp(l->otherdata->date,date) >= 0 && strcmp(l->otherdata->date,date2) <= 0)){
                     sumc += 1;
                     agesum[index] += 1;
                  }
                  l = l->next;
               }
            }
            temp2 = temp2->next;
         }
   }
   else{

         l = hash.vaccinated;
         while(l != NULL){
            index = addcategory(l,agetotal);
            agetotal[index] += 1;
            if(strcmp(temp->name,l->otherdata->countries->name) == 0 && (strcmp(l->otherdata->date,date) >= 0 && strcmp(l->otherdata->date,date2) <= 0)){
               sumc += 1;
               agesum[index] += 1;
            }
            l = l->next;
         }
   }

   if(flag == 1){
      for(int i = 0; i < 4; i++){
         if(i == 0){
            printf("\n0-20 ");
            printf("%d ",agesum[i]);
         }
         else if(i == 1){
            printf("20-40 ");
            printf("%d ",agesum[i]);
         }
         else if(i == 2){
           printf("40-60 ");
           printf("%d ",agesum[i]);
         }
         else{
            printf("60+ ");
            printf("%d ",agesum[i]);
         }
         percentage = (((float)agesum[i] / (float)agetotal[i]) * 100.00);
         printf("%.2f%% \n",percentage);
      }
   }

   return sumc;

}

void populationstatus(hs* hash,char* virusname,int sizeofhash,ht* countries,datarec rec,char* date,char* country,int flag,char* date2){

   int index;
   ht* temp;
   list* l;
   hov* temp2;
   int sumc = 0,total = 0;
   float percentage = 0.0;

   index = sdbm(virusname) % sizeofhash;

   temp = countries;

   if(strncmp(country," ",1) == 0)
      while(temp != NULL){

         total = temp->iter;
         printf("%s ",temp->name);

         sumc = populationsearch(hash[index],virusname,date,temp,flag,date2);

         if(flag == 0){
            printf("%d ",sumc);

            percentage = (((float)sumc / (float)total) * 100.00);
            printf("%.2f%% \n",percentage);
         }

         sumc = 0;
         temp = temp->next;
      }
   else if(strncmp(country," ",1) != 0)
      while(temp != NULL){

         if(strncmp(country,temp->name,strlen(country)) == 0){

            total = temp->iter;
            printf("%s ",temp->name);

            sumc = populationsearch(hash[index],virusname,date,temp,flag,date2);

            if(flag == 0){
               printf("%d ",sumc);

               percentage = (((float)sumc / (float)total) * 100.00);
               printf("%.2f%% \n",percentage);
            }

            return;
         }

         sumc = 0;
         temp = temp->next;
      }

}

void novaccineStatus(char* virusname,hs* hash,ht* hashv,int sizeofhash){

   int index,iter;
   char flag;
   list* l;
   hov* temp;

   index = sdbm(virusname) % sizeofhash;

   l = hash[index].novaccinated;


   if(strncmp(virusname,hash[index].nam,strlen(virusname)) == 0){
      while(l != NULL){
         printf("%d %s %s %s %d \n",l->i,l->otherdata->name,l->otherdata->surname,l->otherdata->countries->name,l->otherdata->age);
         l = l->next;
      }
   }
   else if(strncmp(virusname,hash[index].nam,strlen(virusname)) != 0){
      temp = hash[index].listover;
      while(temp != NULL){
         if(strncmp(virusname,temp->nam,strlen(virusname)) == 0){
            l = temp->novaccinated;
            while(l != NULL){
               printf("%d %s %s %s %d \n",l->i,l->otherdata->name,l->otherdata->surname,l->otherdata->countries->name,l->otherdata->age);
               l = l->next;
            }
         }
         temp = temp->next;
      }
   }

}

hs* removenonvaccinated(int id,char* virusname,hs* hash,ht* hashv,int sizeofhash){

   int index,iter;
   hov* temp;

   index = sdbm(virusname) % sizeofhash;

   if((strncmp(virusname,hash[index].nam,strlen(virusname)) != 0)){

      temp = hash[index].listover;

      while(temp != NULL){
         if(strncmp(virusname,temp->nam,strlen(virusname)) == 0){
            temp->novaccinated = remove_skip_list_data(temp->novaccinated,id);
            hash[index].listover = temp;
            return hash;
         }
         temp = temp->next;
      }
   }
   else
      hash[index].novaccinated = remove_skip_list_data(hash[index].novaccinated,id);

   return hash;
}

/* list* listinsertafter(list* l,int id,int num,datarec rec,dr* r,ht* countries){

   int level;


   for(level = 3; level >= 0; level--){
      if(level > 0){
         while(l->i < id && l->i != l->i){
            l = l->level[level];
         }
      }
      else if(level == 0){
         while(l->i < id && l->next != NULL){
            l = l->next;
         }
      }
   }

   return listinsert(l,id,0,l,rec,r,countries);

} */

hs* insertCitizenrecord(datarec rec,hs* hash,ht* hashv,dr* r,int sizeofhash,char* command,ht* countries,int bloomsize,char* value){

   int iter=1,count = 2,index;
   char str[5],strtemp[10];
   ht* target;

   r = NULL;

   countries = listofcountries(countries,rec.country,target);

   strcpy(strtemp,rec.virname);

   index = sdbm(strtemp) % sizeofhash;

   hash = removenonvaccinated(rec.id,rec.virname,hash,hashv,sizeofhash);

   if(skip_list_search(hash[index].vaccinated,rec.id,command) == 1){

      if(strncmp(rec.vacflag,"YE",2) == 0){

         if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) != 0) && (strncmp(hash[index].nam," ",1) != 0)){

            hash[index].listover = hashoverinsert(hash[index].listover,rec,strtemp);
            hash[index].listover = hashoverlist(hash[index].listover,strtemp,rec,r,value,countries,hash[index],bloomsize);
         }
         else if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) == 0) || (strncmp(hash[index].nam," ",1) == 0)){

            hash[index].vaccinated = listinsert(hash[index].vaccinated,rec.id,0,hash[index].vaccinated,rec,r,countries);
            hash[index].bloomfilter = bloom_filter_insert(hash[index].bloomfilter,bloomsize,3,value);

            free(hash[index].nam);
            hash[index].nam = malloc(strlen(strtemp));
            strncpy(hash[index].nam,strtemp,strlen(strtemp));
         }

      }
      else if(strncmp(rec.vacflag,"NO",2) == 0){

         if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) != 0) && (strncmp(hash[index].nam," ",1) != 0)){

            hash[index].listover = hashoverinsert(hash[index].listover,rec,strtemp);
            hash[index].listover = hashoverlist(hash[index].listover,strtemp,rec,r,value,countries,hash[index],bloomsize);
         }
         else if((strncmp(strtemp,hash[index].nam,strlen(strtemp)) == 0) || (strncmp(hash[index].nam," ",1) == 0)){
            hash[index].novaccinated = listinsert(hash[index].novaccinated,rec.id,0,hash[index].novaccinated,rec,r,countries);

            free(hash[index].nam);
            hash[index].nam = malloc(strlen(strtemp));
            strncpy(hash[index].nam,strtemp,strlen(strtemp));
         }
      }

   }


   return hash;
}

void freelists(list* vaccineStat){

   list* l;

   while(vaccineStat != NULL){
      for(int j = 0; j < 4; j++)
         vaccineStat->level[j] = NULL;

      free(vaccineStat->otherdata->name);
      free(vaccineStat->otherdata->surname);
      free(vaccineStat->otherdata->virname);
      free(vaccineStat->otherdata->vacflag);
      free(vaccineStat->otherdata->date);
      free(vaccineStat->otherdata);

      l = vaccineStat->next;
      free(vaccineStat);
      vaccineStat = l;
   }

}

void freestructs(hs* hash,int sizeofhash,ht* countries){

   hov* l;
   ht* temp;

   for(int i = 0; i < sizeofhash; i++){

      free(hash[i].nam);
      free(hash[i].bloomfilter);
      freelists(hash[i].vaccinated);
      freelists(hash[i].novaccinated);

      while(hash[i].listover != NULL){

         free(hash[i].listover->nam);
         free(hash[i].listover->bloomfilter);

         freelists(hash[i].listover->vaccinated);
         freelists(hash[i].listover->novaccinated);

         l = hash[i].listover->next;
         free(hash[i].listover);
         hash[i].listover = l;
      }
   }

   while(countries != NULL){
      temp = countries->next;
      free(countries);
      countries = temp;
   }

}

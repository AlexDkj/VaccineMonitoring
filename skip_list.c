#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "skip_list.h"
#include "bloom_filter.h"

/* List of countries and their populations */
ht* listofcountries(ht* countries,char* country,ht* target){

  if(countries == NULL){
    countries = malloc(sizeof(ht));
    countries->name = malloc(strlen(country));
    strncpy(countries->name,country,strlen(country));
    countries->iter = 1;
    countries->next = NULL;
    //target = countries;
  }
  else
    if(strncmp(country,countries->name,strlen(country)) == 0)
      countries->iter += 1;
    else
      countries->next = listofcountries(countries->next,country,target);

  return countries;
}

/* Assign data to base level of skip list */
dr* recordassign(dr* r,datarec rec,dr* target,ht* countries){

   if(r == NULL){
      r = malloc(sizeof(dr));
      r->name = malloc(strlen(rec.name));
      r->surname = malloc(strlen(rec.surname));
      r->virname = malloc(strlen(rec.virname));
      r->vacflag = malloc(strlen(rec.vacflag));
      r->date = malloc(strlen(rec.date));

      strncpy(r->name,rec.name,strlen(rec.name));
      strncpy(r->surname,rec.surname,strlen(rec.surname));
      r->age = rec.age;
      r->countries = listofcountries(countries,rec.country,r->countries);
      while(r->countries != NULL || r->countries->next != NULL){
         if(strncmp(rec.country,r->countries->name,strlen(rec.country)) == 0)
            break;
         r->countries = r->countries->next;
      }
      strncpy(r->virname,rec.virname,strlen(rec.virname));
      strncpy(r->vacflag,rec.vacflag,strlen(rec.vacflag));
      strncpy(r->date,rec.date,strlen(rec.date));
      r->next = NULL;

      //target = r;
   }
   else{
      if((strncmp(rec.name,r->name,strlen(rec.name)) == 0) && (strncmp(rec.surname,r->surname,strlen(rec.surname)) == 0) && (strncmp(rec.virname,r->virname,strlen(rec.virname)) == 0))
         return r;
      else
         r->next = recordassign(r->next,rec,target,countries);
   }

   return r;
}

/* Create base layer of skip list */
list* listinsert(list* l,int k,int curlevel,list* leveldata,datarec rec,dr* r,ht* countries){

   list *o = l,*temp2 = l,*temp = l;

   if(o == NULL){
      o = malloc(sizeof(list));
      o->i = k;
      o->otherdata = NULL;
      o->otherdata = recordassign(r,rec,o->otherdata,countries);
      while(o->otherdata != NULL || o->otherdata->next != NULL){
         if((strncmp(rec.name,o->otherdata->name,strlen(rec.name)) == 0) && (strncmp(rec.surname,o->otherdata->surname,strlen(rec.surname)) == 0) && (strncmp(rec.virname,o->otherdata->virname,strlen(rec.virname)) == 0))
            break;
         o->otherdata = o->otherdata->next;
      }
      o->curlevel = curlevel;
      o->level = malloc(4*sizeof(list*));
      for(int i = 1; i < 4; i++)
         o->level[i] = NULL;
      o->next = NULL;
   }
   else{
      if(o->next != NULL && k > o->next->i && k > l->i)
         o->next = listinsert(o->next,k,curlevel,leveldata,rec,r,countries);
      else if(o->next != NULL && k < o->next->i && k > l->i){
         temp2 = malloc(sizeof(list));
         temp2->i = k;
         temp2->otherdata = NULL;
         temp2->otherdata = recordassign(r,rec,temp2->otherdata,countries);
         while(temp2->otherdata != NULL || temp2->otherdata->next != NULL){
            if((strncmp(rec.name,temp2->otherdata->name,strlen(rec.name)) == 0) && (strncmp(rec.surname,temp2->otherdata->surname,strlen(rec.surname)) == 0) && (strncmp(rec.virname,temp2->otherdata->virname,strlen(rec.virname)) == 0))
               break;
            temp2->otherdata = temp2->otherdata->next;
         }
         temp2->curlevel = curlevel;
         temp2->level = malloc(4*sizeof(list*));
         for(int i = 1; i < 4; i++)
            temp2->level[i] = NULL;
         temp2->next = o->next;
         o->next = temp2;
      }
      else if(k < l->i){
         temp2 = malloc(sizeof(list));
         temp2->i = k;
         temp2->otherdata = NULL;
         temp2->otherdata = recordassign(r,rec,temp2->otherdata,countries);
         while(temp2->otherdata != NULL || temp2->otherdata->next != NULL){
            if((strncmp(rec.name,temp2->otherdata->name,strlen(rec.name)) == 0) && (strncmp(rec.surname,temp2->otherdata->surname,strlen(rec.surname)) == 0) && (strncmp(rec.virname,temp2->otherdata->virname,strlen(rec.virname)) == 0))
               break;
            temp2->otherdata = temp2->otherdata->next;
         }
         temp2->curlevel = curlevel;
         temp2->level = malloc(4*sizeof(list*));
         for(int i = 1; i < 4; i++)
            temp2->level[i] = NULL;
         temp2->next = temp;
         o = temp2;
         l = o;
      }
      else
         o->next = listinsert(o->next,k,curlevel,leveldata,rec,r,countries);
   }

   l = o;

   return l;

}

hov* hashoverinit(hov* listover,ht* hash){

   listover = NULL;

   return listover;
}

list* listinit(list* l){

   l = NULL;

   return l;
}

ht* hasinit(ht* h){

   h = NULL;

   return h;
}

/* Insert in overflow list virus name and their structs for each virus */
hov* hashoverinsert(hov* listover,datarec rec,char* str){

   if(listover == NULL){
      listover = malloc(sizeof(hov));
      listover->nam = malloc(strlen(str));
      strncpy(listover->nam,str,strlen(str));
      listover->bloomfilter = bloom_filter_init(listover->bloomfilter,800000,3);
      listover->vaccinated = NULL;
      listover->novaccinated = NULL;
      listover->next = NULL;
   }
   else
      if(strncmp(str,listover->nam,strlen(str)) == 0)
         return listover;
      else
         listover->next = hashoverinsert(listover->next,rec,str);

   return listover;
}

/* Assign level data to skip list */
list* skip_list(list* l,int k){

   int coinprob,count = 0;
   list* ls;
   list* temp = NULL,*t;

   ls = l;

   for(int i = 1; i <= 4; i++){
      while(ls != NULL){

         coinprob = rand() % 2;

         if(coinprob == 1){ // && l->curlevel <= i){

            ls->level[i] = malloc(sizeof(list));

            if(temp != NULL && ls->next != NULL){
               t = ls;
               ls = temp;
               ls->level[i] = t;
               ls = t;
               ls->level[i] = t;
            }
            else if(temp != NULL && ls->next != NULL)
               ls->level[i] = NULL;
         }

         if(ls->level[i] != NULL)
            temp = ls;
         ls = ls->next;
         count++;

      }
      temp = NULL;
      count = 0;
      //ls = l;
   }

   return l;
}

/* Search id in skip list, if find id then print that command do */
int skip_list_search(list* l,int k,char* command){

   int level,flag = 0;
   list* temp = NULL;
   list* temphead = NULL;
   char* tempchar;

   tempchar = malloc(15*sizeof(char));

   l->head = l;
   temp = l;

   for(level = 3; level >= 0; level--){
      if(level > 0){
         while(l->head->i < k && l->head->i != l->i && l->head->next != NULL){
            l->head = l->head->level[level];
            temp = temp->next;
         }
      }
      else if(level == 0){
         while(temp->i < k && temp->next != NULL){
            temp = temp->next;
         }
         //l->head = temp;
      }
   }

   if(strncmp(command,"/vaccineStatus",14) == 0){
      if(l->head->i == k || temp->i == k){
         strncpy(tempchar,temp->otherdata->date,10);
         tempchar[11] = '\0';
         printf("VACCINATED ON %s \n",tempchar);
         free(tempchar);
         return 0;
      }
      else if(temp->i != k){
         printf("NOT VACCINATED\n");
         free(tempchar);
         return 1;
      }
  }
  else if((strncmp(command,"/insertCitizenrecord",20) == 0) || (strncmp(command,"/vaccinateNow",13) == 0)){
      if(l->head->i == k || temp->i == k){
         strncpy(tempchar,temp->otherdata->date,10);
         tempchar[11] = '\0';
         printf("ERROR: CITIZEN %d ALREADY VACCINATED ON %s\n",k,tempchar);
         free(tempchar);
         return 0;
      }
      else if(temp->i != k){
         printf("Inserted data successfully \n");
         free(tempchar);
         return 1;
      }
   }
   else if((strncmp(command,"vaccinepersons",17) == 0)){
      if(l->head->i == k || temp->i == k){
         strncpy(tempchar,temp->otherdata->date,10);
         tempchar[11] = '\0';
         printf("YES %s \n",tempchar);
         free(tempchar);
         return 0;
      }
      else if(temp->i != k){
         printf("NO \n");
         free(tempchar);
         return 1;
      }
   }

   return 0;
}

/* Remove data from skip list */
list* remove_skip_list_data(list* l,int k){

   list* temp = l,*temp1 = l;
   int flag = 0;

   while(temp == NULL){
      if(temp->i == k){
         for(int i = 0; i < 4; i++)
            temp->level = NULL;
         free(temp->level);
         if(l->i == k){
            l = temp->next;
            temp->otherdata = NULL;
            free(temp);
            return l;
         }
         else{
            temp1->next = temp->next;
            temp->otherdata = NULL;
            free(temp);
            l = temp1;
            return l;
         }
      }
      temp1 = temp;
      temp = temp->next;
   }

   return l;
}

/* Supported function for skip list */
hov* skip_list_level_assign(hov* sl){

   if((sl != NULL) && (strncmp(sl->nam," ",1) != 0)){
      sl->vaccinated = skip_list(sl->vaccinated,3);
      sl->novaccinated = skip_list(sl->novaccinated,3);
   }
   else if((sl != NULL) && (strncmp(sl->nam," ",1) == 0))
      sl->next = skip_list_level_assign(sl->next);

   return sl;
}

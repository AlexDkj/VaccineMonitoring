/* Struct for list of countries and for findsizehash */
typedef struct hasit{
   char* name;
   int iter;
   struct hasit* next;
}ht;

/* Data to the base layer of skip list */
typedef struct datap{
   char* name;
   char* surname;
   ht* countries;
   int age;
   char* virname;
   char* vacflag;
   char* date;
   struct datap* next;
}dr;

/* Architecture of skip list */
typedef struct list{
   int i;
   struct list* next;
   int curlevel;
   struct list** level;
   struct list* head;
   dr* otherdata;
}list;

/* Overflow list for hashtable */
typedef struct hashover{
   char* nam;
   unsigned char* bloomfilter;
   list* novaccinated;
   list* vaccinated;
   struct hashover* next;
}hov;

/* Structure of hash */
typedef struct hashstruct{
   char* nam;
   unsigned char* bloomfilter;
   list* novaccinated;
   list* vaccinated;
   hov* listover;
}hs;

/* Struct supported to other supported function for lists */
typedef struct sup{
   ht* hashv;
   dr* r;
}sp;

/* Only read from file used this structure */
typedef struct persondata{
   int id;
   char name[10];
   char surname[20];
   char country[10];
   int age;
   char virname[10];
   char vacflag[5];
   char date[15];
}datarec;

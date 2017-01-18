/******************************************************************************/
/*wordcount.c                                                                 */
/*Riesenie:    IJC-DU2, priklad 2)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       27.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "htable.h"
#include "io.h"
#define WORD_LIMIT 128 // 127 chars and '\0'
#define HASH_TAB_SIZE 1024

void print_listitem(char *key, unsigned long value);

// error handler:
typedef enum {INITERR,
              LOOKUPERR,
              WORDOF,
              //next
             } TErr;
// error massages:
char *errMsg[] = {"Inicialization of hash table failed.",
                  "Cannot add a new item.",
                  "Size of the word overflow.",
                 // next
                 };

void errprint(TErr err)
{
   fprintf(stderr, "ERROR: %s\n", errMsg[err]);
}

int main()
{
   htab_t *words_tab = htab_init(HASH_TAB_SIZE);
   if (words_tab == NULL) {
      errprint(INITERR);
      return 1;
   }

   // count multiplicity of words
   struct htab_listitem *li;
   char s[WORD_LIMIT];
   int len;
   bool word_OF = false;
   while ((len = fgetw(s, WORD_LIMIT, stdin)) != EOF){
      if (!word_OF && len >= WORD_LIMIT) {
         word_OF = true;
         errprint(WORDOF);
      }

      li = htab_lookup(words_tab, s);
      if (li == NULL) {
         errprint(LOOKUPERR);
         htab_free(words_tab);
         return 1;
      }
      else {
         li->data++;
      }
   }

   // print results
   htab_foreach(words_tab, &print_listitem);
   #ifdef P_STATS
   htab_statistics(words_tab);
   #endif

   htab_free(words_tab);
   return 0;
}

void print_listitem(char *key, unsigned long value)
{
   printf("%s\t%lu\n", key, value);
}

/******************************************************************************/
/*libhtable modul                                                             */
/*Riesenie:    IJC-DU2, priklad 2)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       26.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "htable.h"
#include "hash_function.h"

struct htab_listitem * htab_lookup(htab_t *t, const char *key)
{
   struct htab_listitem *last_item    = NULL;
   struct htab_listitem *current_item = NULL;
   // search for index
   unsigned tab_ind = hash_function(key, t->htab_size);
   // search in linked list
   bool index_empty = true;   // make new list
   if (t->item_ptr[tab_ind] != NULL) {
      index_empty = false;
      current_item = t->item_ptr[tab_ind];
      do {
         if (strcmp(key, current_item->key) == 0)
            return current_item;    // key found
         last_item = current_item;  // last valid item in list
         current_item = current_item->next;
      } while (current_item != NULL);
   }
   // if NOT found ADD new listitem
   // create new listitem
   struct htab_listitem *tmp_item = malloc(sizeof(struct htab_listitem));
   if (tmp_item == NULL)
      return NULL;
   else {
      char *tmp_key = malloc(strlen(key) + 1);  // additional +1 for '\0'
      if (tmp_key == NULL) {
         free(tmp_item);
         return NULL;
      }
      else {
         strcpy(tmp_key, key);
         tmp_item->key  = tmp_key;
         tmp_item->data = 0;
         tmp_item->next = NULL;
      }
   }

   // set link to new listitem
   if (index_empty)
      t->item_ptr[tab_ind] = tmp_item;
   else
      last_item->next   = tmp_item;
   return tmp_item;
}

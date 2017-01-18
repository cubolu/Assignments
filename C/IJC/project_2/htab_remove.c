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

int htab_remove(htab_t *t, char *key)
{
   struct htab_listitem *previous_item = NULL;
   struct htab_listitem *current_item  = NULL;
   // search for index
   unsigned tab_ind = hash_function(key, t->htab_size);
   // search in linked list
   bool found = false;
   if (t->item_ptr[tab_ind] == NULL)
      return -1;  // item with key DO NOT exist
   else {
      current_item = t->item_ptr[tab_ind];
      do {
         if (strcmp(key, current_item->key) == 0)
            found = true;
            break;

         previous_item = current_item;
         current_item = current_item->next;
      } while (current_item != NULL);
   }

   // remove found item
   if (found) {
      // remove link
      if (previous_item == NULL)
         t->item_ptr[tab_ind] = current_item->next;
      else
         previous_item->next = current_item->next;
      // deallocate item
      free(current_item->key);
      free(current_item);
      return 1;
   }
   else
      return -1;

   return 1;
}

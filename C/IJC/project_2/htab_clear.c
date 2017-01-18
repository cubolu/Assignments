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

void htab_clear(htab_t *t)
{
   struct htab_listitem *current_item = NULL;
   struct htab_listitem *next_item    = NULL;

   for (unsigned i = 0; i < t->htab_size; i++) {
      if ((current_item = t->item_ptr[i]) != NULL) {
         // remove all items in linked list
         do {
            next_item = current_item->next;
            free(current_item->key);
            free(current_item);
            current_item = next_item;
         } while (next_item != NULL);
      }
      t->item_ptr[i] = NULL;
   }  // end for
}

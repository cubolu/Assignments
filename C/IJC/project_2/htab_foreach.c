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

void htab_foreach(htab_t *t, void (*function)(char *key, unsigned long value))
{
   struct htab_listitem *current_item = NULL;

   for (unsigned i = 0; i < t->htab_size; i++) {
      if ((current_item = t->item_ptr[i]) != NULL) {
         // call function for each item in list
         do {
            function(current_item->key, current_item->data);
            current_item = current_item->next;
         } while (current_item != NULL);
      }
   }
}

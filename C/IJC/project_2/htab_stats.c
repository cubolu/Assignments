/******************************************************************************/
/*libhtable modul                                                             */
/*Riesenie:    IJC-DU2, priklad 2)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       27.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "htable.h"

void htab_statistics(htab_t *t)
{
   struct htab_listitem *current_item = NULL;
   // stats variables:
   unsigned long max = 0;
   unsigned long min = -1; // -1 is maximal value of min(two's complement)
   unsigned long item_sum = 0;
   unsigned long list_sum = 0;
   unsigned long current_list_len = 0;

   for (unsigned i = 0; i < t->htab_size; i++) {
      list_sum++;
      if ((current_item = t->item_ptr[i]) != NULL) {
         current_list_len = 0;
         // call function for each item in list
         do {
            item_sum++;
            current_list_len++;
            current_item = current_item->next;
         } while (current_item != NULL);
         max = (current_list_len > max ? current_list_len : max);
         min = (current_list_len < min ? current_list_len : min);
      }
   }

   printf("%f\t|%lu\t|%lu\n",(double)(list_sum == 0 ? 0.0 : item_sum/list_sum), min > max ? 0 : min, max);
}

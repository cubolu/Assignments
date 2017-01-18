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

// returns valid ptr to hash table, else returns NULL
htab_t * htab_init(unsigned size)
{
   // allocate size of a table(table data + array of ptr)
   htab_t *tmp_tab = malloc(sizeof(htab_t) + size*sizeof(struct htab_listitem *));
   if (tmp_tab == NULL)
      return NULL;

   // init the allocated table
   for (unsigned i = 0; i < size; i++)
      tmp_tab->item_ptr[i] = NULL;
   tmp_tab->htab_size = size;
   return tmp_tab;
}

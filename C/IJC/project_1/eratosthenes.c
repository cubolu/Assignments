/******************************************************************************/
/*eratosthenes.c                                                              */
/*Riesenie:    IJC-DU1, priklad a)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/
#include "eratosthenes.h"
#include <math.h>

void Eratosthenes(BitArray_t pole) {
   if (BA_size(pole) > 2) {
      BA_set_bit(pole, 0, 1);
      BA_set_bit(pole, 1, 1);
   }
   else {
      return;
   }

   unsigned long sqrt_array_limit = sqrt(BA_size(pole) - 1);   //-1 index udava prvocislo, maximum je BA_size() - 1
   for (unsigned long current_i = 2; current_i <= sqrt_array_limit; current_i++) {
      if (BA_get_bit(pole, current_i) == 0) {
         for (unsigned long i = (current_i*2); i < BA_size(pole); i+=current_i) { //current_i*2 lebo current_i sa nemeni lebo to je prvocislo
            BA_set_bit(pole, i, 1);
         }
      }
   } //end for
}

/******************************************************************************/
/*prvocisla.c                                                                 */
/*Riesenie:    IJC-DU1, priklad a)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/
#include <stdio.h>
#include "eratosthenes.h"

#define ARRAY_SIZE 201000000

int main()
{
   BA_create(primes_array, (ARRAY_SIZE+1));   //+1 indexujeme od nuly a indexy su potencionalne prvocisla
   Eratosthenes(primes_array);
   //najdi poslednych 10 prvocisel od 0 do 201M
   unsigned long last_primes[10];
   int count = 9;
   for (unsigned long i = BA_size(primes_array) - 1; i > 1 && count > -1; i--) {
      if (BA_get_bit(primes_array, i) == 0) {
         last_primes[count--] = i;
      }
   }

   //vypis poslednych 10 prvocisel od 0 do 201M
   for (size_t i = 0; i < sizeof(last_primes)/sizeof(unsigned long); i++) {
      printf("%lu\n", last_primes[i]);
   }

   return 0;
}

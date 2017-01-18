/******************************************************************************/
/*hash_function.c                                                             */
/*Riesenie:    IJC-DU2, priklad 2)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       26.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "hash_function.h"

unsigned int hash_function(const char *str, unsigned htab_size)
{
   unsigned int h = 0;
   const unsigned char *p;
   for(p=(const unsigned char*)str; *p!='\0'; p++)
      h = 65599*h + *p;
   return h % htab_size;
}

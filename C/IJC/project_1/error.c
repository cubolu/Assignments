/******************************************************************************/
/*error.c                                                                     */
/*Riesenie:    IJC-DU1, priklad b)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "error.h"

void Warning(const char *fmt, ...)
{
   fprintf(stderr, "CHYBA: ");
   va_list args;
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
}
void FatalError(const char *fmt, ...)
{
   fprintf(stderr, "CHYBA: ");
   va_list args;
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   exit(1);
}

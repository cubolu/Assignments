/******************************************************************************/
/*io.c                                                                        */
/*Riesenie:    IJC-DU2, priklad 2)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       27.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "io.h"

int fgetw(char *s, int max, FILE *f)
{
   int c;
   // skip isspace characters before word
   while (isspace(c = fgetc(f)) && c != EOF );

   if (c == EOF)
      return EOF;

   // read word
   int i = 0;
   while (!isspace(c) && c != EOF) {
      if (i < max-1)
         s[i] = (char)c;
      c = fgetc(f);
      i++;
   }

   // add '\0' to the end of s
   if (i < max-1)
      s[i] = '\0';
   else
      s[max-1] = '\0';

   return i;
}

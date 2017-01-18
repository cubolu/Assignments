#ifndef PPM_H_
#define PPM_H_

/******************************************************************************/
/*ppm.h                                                                       */
/*Riesenie:    IJC-DU1, priklad b)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "eratosthenes.h"
#include "error.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

struct ppm {
   unsigned xsize;
   unsigned ysize;
   char data[];    // RGB bajty, celkom 3*xsize*ysize
};

struct ppm * ppm_read(const char * filename);
int ppm_write(struct ppm *p, const char * filename);

#endif // PPM_H_

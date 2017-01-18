#ifndef ERROR_H_
#define ERROR_H_

/******************************************************************************/
/*error.h                                                                     */
/*Riesenie:    IJC-DU1, priklad b)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void Warning(const char *fmt, ...);
void FatalError(const char *fmt, ...);

#endif // ERROR_H_

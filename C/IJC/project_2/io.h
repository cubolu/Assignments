/******************************************************************************/
/*io.h                                                                        */
/*Riesenie:    IJC-DU2, priklad 2)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       26.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <ctype.h>

/**
 * function read ONE word from FILE *f
 * maxiamal length of word is max-1
 * word is stored in char *s
 * returns length of stored word or EOF
 **/
int fgetw(char *s, int max, FILE *f);

#endif

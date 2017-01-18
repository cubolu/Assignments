/******************************************************************************/
/*tail.h                                                                      */
/*Riesenie:    IJC-DU2, priklad 1)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       16.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#ifndef TAIL_H_
#define TAIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LINES_LIMIT 510
long sizeStrToNum(const char *str);
int fTail(const char *filename, unsigned long fromLine, unsigned long lastNLines, bool isFile);
long lineCount(FILE *fileptr);

#endif

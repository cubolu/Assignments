#ifndef ERATOSTHENES_H_
#define ERATOSTHENES_H_

/******************************************************************************/
/*eratosthenes.h                                                              */
/*Riesenie:    IJC-DU1, priklad a)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/
#include "bit-array.h"

/**
 * bity na prvociselnych indexoch maju hodnotu 0
 * a ostatne bity nastavi na 1
 */
void Eratosthenes(BitArray_t pole);

#endif // ERATOSTHENES_H_

#ifndef BIT_ARRAY_H_
#define BIT_ARRAY_H_

/******************************************************************************/
/*bit-array.h                                                                 */
/*Riesenie:    IJC-DU1, priklad a)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/
#include <limits.h>
#include "error.h"

/**
 * definicia datoveho typu 'bitove pole'
 */
typedef unsigned long * BitArray_t;

/**
 * BA_create vytvori bitove pole o velkosti s
 * a na index [0] ulozi velkost pola v bitoch
 * pole je mozne indexovat od 0 do s-1
 * a bity su nastavene na 0
 */
#define BA_create(name, s) unsigned long name[((s) % BITS_IN_LONG) ? (s)/BITS_IN_LONG + 2 : (s)/BITS_IN_LONG + 1] = {(unsigned long)(s)}

//pomocne makro, pocita velkost unsigned long v bitoch pre danu architekturu
#define BITS_IN_LONG (sizeof(unsigned long) * CHAR_BIT)
//pomocene makro, pocita velkost typu p v bitoch
#define BITS_IN_TYPE(p)    (sizeof(p[0]) * CHAR_BIT)
//pomocne makro, posunie 1UL na spravnu poziciu bitu s indexom i v poli p
#define SHIFT_ONE(p,i)     (1UL << ( (i)%BITS_IN_TYPE(p)))

/**
 * zmeni hodnotu bitu na indexe i v poli p(nazov pola) na hodnotu b
 * bitove pole je indexovane od 1, na indexe [0] je ulozena jeho velkost
 */
#define DU1_SET_BIT(p,i,b) ( b ? \
                                 ( p[ ((i)/BITS_IN_TYPE(p))+1 ] |= SHIFT_ONE(p,i) ) : \
                                 ( p[ ((i)/BITS_IN_TYPE(p))+1 ] &= ~SHIFT_ONE(p,i))\
                           )
/**
 * urci hodnotu bitu na indexe i v poli p(nazov pola)
 * bitove pole je indexovane od 1, na indexe [0] je ulozena jeho velkost
 */
#define DU1_GET_BIT(p,i)   ((( p[ ((i)/BITS_IN_TYPE(p))+1 ] & SHIFT_ONE(p,i) ) > 0) ? 1 : 0)

#if defined(USE_INLINE)
/**
 * inline fcie pre pracu s bitovym polom
 * makro BA_create, tak ako je definovane sa neda nahradit inline funkciou
 */
inline unsigned long BA_size(BitArray_t name)
{
   return name[0];
}
inline void BA_set_bit(BitArray_t name, unsigned long index, int value)
{
   if (BA_size(name) > index)
      DU1_SET_BIT(name, index, value);
   else
      FatalError("Index %ld mimo rozsah 0..%ld", (long)index, ((long)BA_size(name)-1));
}
inline int BA_get_bit(BitArray_t name, unsigned long index)
{
   if (BA_size(name) > index)
      return DU1_GET_BIT(name, index);
   else
      FatalError("Index %ld mimo rozsah 0..%ld", (long)index, ((long)BA_size(name)-1));
      return -1;
}

#else
/**
 * marka pre pracu s bitovym polom
 */
//urci pocet bitov v poli, pole je indexovane od 0 do BA_size() - 1
#define BA_size(name)      (name[0])
//BA_set_bit a BA_get_bit kontroluju medze bitoveho pola
#define BA_set_bit(name, index, value) (BA_size(name) > (unsigned long)(index) ? DU1_SET_BIT(name, index, value) : \
                                                                                 (FatalError("Index %ld mimo rozsah 0..%ld", (long)index, ((long)BA_size(name)-1)), 1UL))
#define BA_get_bit(name, index)        (BA_size(name) > (unsigned long)(index) ? DU1_GET_BIT(name, index) : \
                                                                                 (FatalError("Index %ld mimo rozsah 0..%ld", (long)index, ((long)BA_size(name)-1)), 1UL))
#endif

#endif // BIT_ARRAY_H_

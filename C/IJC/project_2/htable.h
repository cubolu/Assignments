/******************************************************************************/
/*htable.h                                                                    */
/*Riesenie:    IJC-DU2, priklad 2)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       26.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#ifndef HTABLE_H_
#define HTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Data types in libhtable:
 */
//typedef struct htab_listitem listitem_t;
struct htab_listitem {
   char *key;
   unsigned long data;
   //listitem_t *next;
   struct htab_listitem *next;
};

typedef struct hashtable htab_t;
struct hashtable {
   unsigned htab_size;
   //listitem_t *item_ptr[htab_size];
   struct htab_listitem *item_ptr[];
};

/**
 * Functions in libhtable:
 */
// create and init table
htab_t * htab_init(unsigned size);

// search for listitem with string *key
//listitem_t         * htab_lookup(htab_t *t, const char *key);
struct htab_listitem * htab_lookup(htab_t *t, const char *key);

// function could be used for printing
// function should NOT change values of table and listitems
void htab_foreach(htab_t *t, void (*function)(char *key, unsigned long value));

// search for listitem with string *key and remove this item
int htab_remove(htab_t *t, char *key);

// remove all listitems from table
void htab_clear(htab_t *t);

// remove table, call clear and free htab_t
inline void htab_free(htab_t *t)
{
   htab_clear(t);
   free(t);
}

void htab_statistics(htab_t *t);

#endif

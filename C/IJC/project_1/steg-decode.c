/******************************************************************************/
/*steg-decode.c                                                               */
/*Riesenie:    IJC-DU1, priklad b)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "ppm.h"

#define LIMIT 75000000 //5000*5000*3
#define PRIMES_IN_LIMIT 4394304  //pocet prvocisel mensich ako 75M

BA_create(primes_array, LIMIT);

int main(int argc, const char *argv[])
{
   //nazov suboru ppm je zadany ako argument
   //program hlada zasifrovanu spravu
   //sprava je vratane \0
   //spravu vypise na stdout
   if (argc != 2 || ((strcmp(&argv[1][strlen(argv[1])-4], ".ppm")) != 0))
      FatalError("Neplatny nazov suboru. (priklad: du1-obrazek.ppm)\n");

   // alokovanie struktury
   struct ppm *cipher_ppm = ppm_read(argv[1]);
   if (cipher_ppm == NULL)
      FatalError("Pri nacitavani suboru \"%s\" doslo k chybe.\n", argv[1]);
   else
      //prvociselne indexy maju hodnotu 0
      Eratosthenes(primes_array);

   BA_create(msg_array, PRIMES_IN_LIMIT);

   int ppm_data_size = cipher_ppm->xsize * cipher_ppm->ysize * 3;  //pocet bajtov v ppm->data
   int msg_i = 0;          //index v bitovom poli msg_array
   int cipher_bit;         //bit z bajtov v ppm->data
   int zero_count = 0;
   bool zero_stop = true;
   for (int i = 0; (zero_stop) && (i < ppm_data_size); i++) {
      if (BA_get_bit(primes_array, i) == 0) {
         cipher_bit = (cipher_ppm->data[i] & 1);
         switch (cipher_bit) {
            case 0:
               zero_count++;
               if (zero_count == 8)
                  zero_stop = false;
               break;
            case 1:
               zero_count = 0;
               break;
         }  //end switch
         BA_set_bit(msg_array, msg_i, cipher_bit);
         msg_i++;
      }  //end if
   }  //end for
   //strukturu ppm uz nepotrebujem, data su v msg_array
   free(cipher_ppm);

   if (zero_stop)
      FatalError("Sifrovany retazec v subore \"%s\" nie je ukonceny \"\\0\"!\n", argv[1]);
   else {
      for (char *msg_p = (char *)(&msg_array[1]); *msg_p != '\0'; msg_p++) {
         if (!isprint(*msg_p))
            FatalError("Sifrovany retazec v subore \"%s\" obsahuje riadiace znaky!\n", argv[1]);
      }  //end for
      puts((char *)(&msg_array[1]));
   }  //end else
   return 0;
}

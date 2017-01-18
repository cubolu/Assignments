/******************************************************************************/
/*ppm.c                                                                       */
/*Riesenie:    IJC-DU1, priklad b)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       19.03.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "ppm.h"

struct ppm * ppm_read(const char * filename)
{
   //otvorenie suboru
   FILE *ppm_data;
   ppm_data = fopen(filename, "r");
   if (ppm_data == NULL) {
      Warning("Subor \"%s\" nebolo mozne otvorit!\n", filename);
      return NULL;
   }
   /**
    * nacitanie formatu PPM:
    * "P6" <ws>+
    * <xsizetxt> <ws>+ <ysizetxt> <ws>+
    * "255" <ws>
    * <binarni data, 3*xsize*ysize bajtu RGB>
    * <EOF>
    */
   bool read_head = true;
   int state = 1;
   int char_read;
   int tmp_x = 0;
   bool set_x = true;
   int tmp_y = 0;
   bool set_y = true;
   while (read_head && ((char_read = fgetc(ppm_data)) != EOF)) {
      switch (state) {
         case 1:  //over magicku konstantu "P6"
            if ((char_read == 'P') && (fgetc(ppm_data) == '6'))
               state++;
            else
               read_head = false;
            break;
         case 2:  //citavanie <ws>
            if (isspace(char_read));
               //do nothing
            else if (isdigit(char_read)) {   //nacitanie cisla
               if (set_x) {
                  tmp_x = tmp_x*10 + (char_read - '0');
                  state++;
               }
               else if (set_y) {
                  tmp_y = tmp_y*10 + (char_read - '0');
                  state = 4;
               }
               else if (char_read == '2')
                  state = 5;  //nacitanie "255", a size x, y uz boli nacitane
               else
                  read_head = false;
            } //end else if (isdigit())
            else
               read_head = false;
            break;
         case 3:  //citanie <xsizetxt>
            set_x = false;
            if (isdigit(char_read))
               tmp_x = tmp_x*10 + (char_read - '0');
            else if (isspace(char_read))
               state = 2;
            else
               read_head = false;
            break;
         case 4:  //citanie <ysizetxt>
            set_y = false;
            if (isdigit(char_read))
               tmp_y = tmp_y*10 + (char_read - '0');
            else if (isspace(char_read))
               state = 2;
            else
               read_head = false;
            break;
         case 5:  //overenie konca hlavicky, "255" a jedneho <ws>
            read_head = false;
            if (char_read == '5' && fgetc(ppm_data) == '5' && isspace(fgetc(ppm_data)))
               state = 6;  //6 znamena, ze vsetko co ma byt v hlavicke bolo uspesne nacitane
      }  //end switch
   }  //end while

   //chybne nacitanie hlvicky => ukoncenie fcie
   if (state != 6) {
      Warning("Format suboru \"%s\" neodpoveda formatu PPM!\n", filename);
      fclose(ppm_data);
      return NULL;
   }

   size_t pix_size = tmp_x * tmp_y * 3;
   struct ppm * tmp_p = malloc(sizeof(struct ppm) + pix_size);
   //malloc nebol uspesny alebo alokoval 0 bajtov
   if (tmp_p == NULL || tmp_x == 0 || tmp_y == 0) {
      Warning("Nepodarilo sa nacitat obrazove data suboru \"%s\"!\n", filename);
      fclose(ppm_data);
      free(tmp_p);
      return NULL;
   }
   //data presiahli implementacny limit
   if ((tmp_x * tmp_y) > 5000*5000) {
      Warning("Subor \"%s\" presiahol implementacny limit!\n", filename);
      fclose(ppm_data);
      free(tmp_p);
      return NULL;
   }
   else {
      //nacitanie rozmerov do struct ppm
      tmp_p->xsize = tmp_x;
      tmp_p->ysize = tmp_y;
   }

   //nacitanie obrazovych dat do struktury
   if (fread(tmp_p->data, 1, pix_size, ppm_data) != pix_size) {   //sizeof(char) = 1
      Warning("Zadane rozmery <xsizetxt> a <ysizetxt> neodpovedaju datam v subore \"%s\"!\n", filename);
      fclose(ppm_data);
      free(tmp_p);
      return NULL;
   }

   //zatvorenie suboru pri uspesnom nacitani dat
   fclose(ppm_data);
   return tmp_p;
}

//fcia zapise ppm strukturu do suboru s menom "filename
int ppm_write(struct ppm *p, const char * filename)
{
   FILE *ppm_file;
   ppm_file = fopen(filename, "w");
   if (ppm_file == NULL) {
      Warning("Subor s menom \"%s\" nebolo mozne otvorit/vytvorit!\n", filename);
      return -1;
   }

   //subor je otvorny/existuje
   fprintf(ppm_file,
         "P6\n"
         "%u %u\n"
         "255\n", p->xsize, p->ysize
         );
   size_t pix_size = p->xsize * p->ysize * 3;
   if (fwrite(p->data, 1, pix_size, ppm_file) != pix_size) {
      fclose(ppm_file);
      Warning("Data do suboru \"%s\" nebolo mozne zapisat!", filename);
      return -1;
   }

   fclose(ppm_file);
   return 0;
}

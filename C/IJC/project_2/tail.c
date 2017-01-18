/******************************************************************************/
/*tail.c                                                                      */
/*Riesenie:    IJC-DU2, priklad 1)                                            */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       24.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "tail.h"

// error handler:
typedef enum {BADNUM,
              OPENERR,
              UNDEF,
              LINEOF,
              NOPAR,
              //next
             } TErr;
// error massages:
char *errMsg[] = {"Number must be greater or equal 0.",
                  "File opening failed.",
                  "Program failed.",
                  "Line(s) overflow.",
                  "Option -n requires an parameter.",
                 // next
                 };

void errprint(TErr err)
{
   fprintf(stderr, "ERROR: %s\n", errMsg[err]);
}

int main(int argc, const char *argv[])
{
   // declare variables for line range
   long fromLine = 0;
   long lastNLines = 0;

   int iStartNames = 1;

   // check arguments for parameter and its value
   if (argc > 1 && strcmp(argv[1], "-n") == 0) {
      if (!(argc > 2)) {
         errprint(NOPAR);
         return EXIT_FAILURE;
      }

      if (argv[2][0] == '+') {
         // function return size as unsigned long or exit program
         fromLine = sizeStrToNum(&argv[2][1]); // argv without plus
         if (fromLine == 0) {
            fromLine = 1;
         }
      }
      else {
         if (argv[2][0] == '-') {
            // function return size as unsigned long or exit program
            lastNLines = sizeStrToNum(&argv[2][1]); // argv with minus
         }
         else {
            // function return size as unsigned long or exit program
            lastNLines = sizeStrToNum(argv[2]);
         }

         if (lastNLines == 0) {
            return EXIT_SUCCESS;
         }
      }
      iStartNames = 3;
   }
   else {
      // default value is last ten lines
      lastNLines = 10;
   }

   bool err = false;

   if (argc > iStartNames) {
      for (int i = iStartNames; i < argc; i++) {
         if (argc != iStartNames+1)
            printf("==>  %s  <==\n", argv[i]);
         if (fTail(argv[i], fromLine, lastNLines, true) < 0)
            err = true;
      }
   }
   else if (argc == iStartNames) {
      // no file argument => stdin
      if (fTail("", fromLine, lastNLines, false) < 0)
         err = true;
   }

   if (err)
      return EXIT_FAILURE;

   return EXIT_SUCCESS;
}

long sizeStrToNum(const char *str)
{
   long tmp = 0;
   char *endarg;
   tmp = strtol(str, &endarg, 10);
   if (*endarg != '\0' || tmp < 0 || strlen(str) < 1) {
      errprint(BADNUM);
      exit(EXIT_FAILURE);
   }
   return tmp;
}

int fTail(const char *filename, unsigned long fromLine, unsigned long lastNLines, bool isFile)
{
   FILE *fileptr;
   if (isFile) {
      fileptr = fopen(filename, "r");
      if (fileptr == NULL) {
         errprint(OPENERR);
         return -1;
      }
   }
   else {
      // stdin
      fileptr = tmpfile();
      if (fileptr == NULL) {
         errprint(OPENERR);
         return -1;
      }
      // read and store stdin
      int c;
      while ((c = getc(stdin)) != EOF) {
         fputc(c, fileptr);
      }
      fseek(fileptr, 0, SEEK_SET);
   }

   long startL;
   // set printing start line
   if (lastNLines != 0) {
      // countlines and set starting line
      startL = lineCount(fileptr) - lastNLines + 1; //+1 start with next
   }
   else if (fromLine != 0) {
      startL = fromLine;
   }
   else {
      errprint(UNDEF);
      fclose(fileptr);
      return -1;
   }

   // read line up to LINES_LIMIT
   char line[LINES_LIMIT + 1];
   line[LINES_LIMIT - 1] = '\0'; // set checking character

   long lineIndex = 0;
   int printing = 2;    // state for printing line
   bool OFLine = false; // line overflow flag
   bool errOF = false;  // print error msg, if line OF

   while (fgets(line, (LINES_LIMIT + 1), fileptr) != NULL) {
      if (line[LINES_LIMIT - 1] == '\0' || line[LINES_LIMIT - 1] == '\n') {
         if (OFLine)
            // line OF so do not print
            printing = 0;
         else {
            // line is OK
            lineIndex++;
            printing = 2;
         }
         // OF flag reset
         OFLine = false;
      }
      else {
         if (OFLine)
            printing = 0;
         else {
            // first OF in line
            lineIndex++;
            printing = 1;
            OFLine = true;
         }
         errOF = true;
      }

      switch (printing) {
         case 0:
            break;
         case 1:
            if (lineIndex >= startL)
               printf("%s\n", line);
            break;
         case 2:
            if (lineIndex >= startL)
               printf("%s", line);
            break;
      }

      // reset checking char
      line[LINES_LIMIT - 1] = '\0'; // set checking character
   }  // end while

   fclose(fileptr);
   if (errOF) {
      errprint(LINEOF);
      return -1;
   }
   return 0;
}

long lineCount(FILE *fileptr)
{
   // read line up to LINES_LIMIT
   char line[LINES_LIMIT + 1];
   line[LINES_LIMIT - 1] = '\0'; // set checking character
   long lineC = 0;
   while (fgets(line, (LINES_LIMIT + 1), fileptr) != NULL) {
      if (line[LINES_LIMIT - 1] == '\0' || line[LINES_LIMIT - 1] == '\n') {
         lineC++;
      }
      else {
         line[LINES_LIMIT - 1] = '\0';
      }
   }
   fseek(fileptr, 0, SEEK_SET);
   return lineC;
}

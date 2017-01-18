/******************************************************************************/
/*h2o.c                                                                       */
/*Riesenie:    IOS-projekt2                                                   */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       30.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "buildh2o.h"

// error handler:
typedef enum {
              PMISSING,
              BADNUM,
             } TErr;

// error massages:
char *errMsg[] = {"At least one of the input parameter is missing",
                  "At least one of the input parameter is not valid number",
                 };
void err_print(TErr err)
{
   fprintf(stderr, "Error: %s\n", errMsg[err]);
}

// convert argument(string) to long int
// if limit is true, it returns number from 0 to 5000
unsigned long par_to_number(const char *arg, bool limit)
{
   unsigned long tmp = 0;
   char *endarg = NULL;
   tmp = strtoul(arg, &endarg, 10);
   // limit is TRUE: <0, 5000>
   // limit is FALSE: <0, inf>
   if (*endarg != '\0' || (limit && tmp > 5000)) {
      err_print(BADNUM);
      exit(1);
   }
   return tmp;
}

int main(int argc, const char *argv[])
{
   // programs parameters:
   unsigned long N_molecules;  // N molecule will be made
   unsigned int  GH_time;      // maximal HYDROGEN generating time
   unsigned int  GO_time;      // maximal OXYGEN generating time
   unsigned int  B_time;       // maximal bonding time

   if (argc == 5) {
      N_molecules = par_to_number(argv[1], false);
      if (N_molecules == 0) {
         err_print(BADNUM);
         return 1;
      }
      GH_time    = (unsigned int)par_to_number(argv[2], true);
      GO_time    = (unsigned int)par_to_number(argv[3], true);
      B_time     = (unsigned int)par_to_number(argv[4], true);
   }
   else {
      err_print(PMISSING);
      return 1;
   }

   // alocate shared memory
   sync_t *shared_v = init_shared_mem();
   if (shared_v == NULL) {
      // error was printed by init_shared_mem
      return 2;
   }

   // initialize time values and output file
   shared_v->N_molecules = N_molecules;
   shared_v->max_H_time  = GH_time;
   shared_v->max_O_time  = GO_time;
   shared_v->max_B_time  = B_time;

   // making generators
   pid_t H_gen_pid;
   pid_t O_gen_pid;

   H_gen_pid = fork();
   if (H_gen_pid == 0) {      // hydrogen generator process
      // call hydrogen generator
      H_gen(shared_v);

   }
   else if (H_gen_pid > 0) {  // main process
      O_gen_pid = fork();
      if (O_gen_pid == 0) {      // oxygen generator process
         // call oxygen generator
         O_gen(shared_v);

      }
      else if (O_gen_pid > 0);  // main process
      else {
         perror("Error: ");
         // free shared struct
         free_shared_mem(shared_v);
         // TODO kill H_gen and its forks
         return 2;
      }
   }
   else {
      perror("Error: ");
      // free shared struct
      free_shared_mem(shared_v);
      return 2;
   }

   // main process code

   // wait for forks
   waitpid(H_gen_pid, NULL, 0);
   waitpid(O_gen_pid, NULL, 0);

   // free shared struct
   free_shared_mem(shared_v);
   return 0;
}

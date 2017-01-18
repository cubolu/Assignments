/******************************************************************************/
/*b_output.c                                                                  */
/*Riesenie:    IOS-projekt2                                                   */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       30.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "buildh2o.h"

void fprt_out(at_t atom_type, unsigned long atom_id, char *state, sync_t *shm)
{

   sem_wait(&shm->op_cnt_mutex);
   fprintf(shm->output_file, "%ld\t: %c %ld\t: %s\n",
           ++shm->op_counter,
           (char)atom_type,
           atom_id,
           state
          );
   fflush(shm->output_file);
   sem_post(&shm->op_cnt_mutex);

}

void atom_exit(at_t atom_type, unsigned long atom_id, sync_t *shm)
{
   // synchronize the end of all atoms
   if (shm->N_molecules_bonded == shm->N_molecules)
      sem_post(&shm->finish_mutex); // after last molecule was bonded

   sem_wait(&shm->finish_mutex);

   fprt_out(atom_type, atom_id, "finished", shm);

   sem_post(&shm->finish_mutex);

   exit(0);
}

void wait_for_children(void)
{
   // wait for all child processes
   while (waitpid(-1, NULL, 0)) {
      if (errno == ECHILD) {
         break;
      }
   }
}

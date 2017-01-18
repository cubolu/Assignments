/******************************************************************************/
/*b_init.c                                                                    */
/*Riesenie:    IOS-projekt2                                                   */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       30.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "buildh2o.h"

void init_semaphore(sem_t *mutex, int start_value, int *error);

sync_t * init_shared_mem(void)
{
   int tmp_sh_id;
   sync_t *tmp;

   // get shared memory area
   tmp_sh_id = shmget(IPC_PRIVATE, sizeof(sync_t), IPC_CREAT | SHM_R | SHM_W);
   if (tmp_sh_id == -1) {
      perror("Error: ");
      return NULL;
   }
   // attach shared memory area
   tmp = (sync_t *)shmat(tmp_sh_id, NULL, 0);
   if (tmp == (void *) -1) {
      perror("Error: ");
      shmctl(tmp_sh_id, IPC_RMID, NULL);
      return NULL;
   }

   tmp->sh_id = tmp_sh_id;
   // initialize counters
   tmp->op_counter            = 0;
   tmp->H_queue               = 0;
   tmp->O_queue               = 0;
   tmp->N_molecules_bonded    = 0;
   tmp->N_atoms_bonded        = 0;

   // open output file
   FILE *o_file = fopen(OUTPUT_FILE, "w");
   if (o_file == NULL) {
      perror("Error: ");
      free_shared_mem(tmp);
      return NULL;
   }
   else {
      tmp->output_file = o_file;
   }

   // initialize semaphores
   /** start_value:
    *    n-processes can execute this critical section in same time
    *    sem_wait => start_value--
    *    sem_post => start_value++
    */
   int mutex_error = 0;
   init_semaphore(&tmp->op_cnt_mutex, 1, &mutex_error);
   init_semaphore(&tmp->at_cnt_mutex, 1, &mutex_error);
   init_semaphore(&tmp->check_mutex,  1, &mutex_error);
   init_semaphore(&tmp->bonding_mtx,  0, &mutex_error);
   init_semaphore(&tmp->bonded_mutex, 0, &mutex_error);
   init_semaphore(&tmp->finish_mutex, 0, &mutex_error);
   init_semaphore(&tmp->bond_queue_H, 0, &mutex_error);
   init_semaphore(&tmp->bond_queue_O, 0, &mutex_error);

   // check mutex_error value
   if (mutex_error != 0) {
      free_shared_mem(tmp);
      return NULL;
   }

   return tmp;
}

void init_semaphore(sem_t *mutex, int start_value, int *error)
{
   if (sem_init(mutex, 1, start_value) == -1) {
      perror("Error: ");
      (*error)++;
   }
}

void free_shared_mem(sync_t *removing_shm)
{
   // close output file
   fclose(removing_shm->output_file);

   // destroy all semaphores
   sem_destroy(&removing_shm->op_cnt_mutex);
   sem_destroy(&removing_shm->at_cnt_mutex);
   sem_destroy(&removing_shm->check_mutex);
   sem_destroy(&removing_shm->bonding_mtx);
   sem_destroy(&removing_shm->bonded_mutex);
   sem_destroy(&removing_shm->finish_mutex);
   sem_destroy(&removing_shm->bond_queue_H);
   sem_destroy(&removing_shm->bond_queue_O);

   // finally remove shared memory
   int sh_id = removing_shm->sh_id;
   shmdt(removing_shm);
   shmctl(sh_id, IPC_RMID, NULL);
   return;
}

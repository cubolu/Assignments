/******************************************************************************/
/*b_bond.c                                                                    */
/*Riesenie:    IOS-projekt2                                                   */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       30.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#include "buildh2o.h"

void bond(at_t atom_type, long atom_id, sync_t *shm);
void finish_bond(at_t atom_type, long atom_id, sync_t *shm);

/* functions try bonding atoms, if atoms can be bonded call bond,
 * else add atom(process) to waiting queue
 **/
void try_bond_H(unsigned long atom_id, sync_t *shm)
{
   sem_wait(&shm->check_mutex);
   if (shm->H_queue > 0 && shm->O_queue > 0) {  // at least one H and one O in queues

      // two atoms have already been waiting
      fprt_out(HYDROGEN, atom_id, "ready", shm);

      // post two another processes
      // one H, need one H and one O
      sem_post(&shm->bond_queue_H);
      sem_post(&shm->bond_queue_O);
      // decrement queue indexes
      shm->H_queue -= 1;
      shm->O_queue -= 1;

      bond(HYDROGEN, atom_id, shm);

      finish_bond(HYDROGEN, atom_id, shm);

      // process wait for another two atoms to finish bonding
      sem_wait(&shm->bonded_mutex);
      sem_wait(&shm->bonded_mutex);

      // increment number of successfully bonded molecules
      shm->N_molecules_bonded++;

      sem_post(&shm->check_mutex);

   }  // if ended
   else {

      shm->H_queue++;

      fprt_out(HYDROGEN, atom_id, "waiting", shm);

      // post checking and wait for more atoms
      sem_post(&shm->check_mutex);
      sem_wait(&shm->bond_queue_H);

      // sem_wait return, two H and one O are ready
      bond(HYDROGEN, atom_id, shm);

      finish_bond(HYDROGEN, atom_id, shm);

      // bonding is finished
      sem_post(&shm->bonded_mutex);

   }  // else ended

   return;
}
void try_bond_O(unsigned long atom_id, sync_t *shm)
{
   sem_wait(&shm->check_mutex);
   if (shm->H_queue > 1) {  // at least two H in queue

      // two atoms have already been waiting
      fprt_out(OXYGEN, atom_id, "ready", shm);

      // post two another processes
      // one O, need two H
      sem_post(&shm->bond_queue_H);
      sem_post(&shm->bond_queue_H);
      // decrement queue index
      shm->H_queue -= 2;

      bond(OXYGEN, atom_id, shm);

      finish_bond(OXYGEN, atom_id, shm);

      // process wait for another two atoms to finish bonding
      sem_wait(&shm->bonded_mutex);
      sem_wait(&shm->bonded_mutex);

      // increment number of successfully bonded molecules
      shm->N_molecules_bonded++;

      sem_post(&shm->check_mutex);

   }  // if ended
   else {

      shm->O_queue++;

      fprt_out(OXYGEN, atom_id, "waiting", shm);

      // post checking and wait for more atoms
      sem_post(&shm->check_mutex);
      sem_wait(&shm->bond_queue_O);

      // sem_wait return, two H and one O are ready
      bond(OXYGEN, atom_id, shm);

      finish_bond(OXYGEN, atom_id, shm);

      // bonding is finished
      sem_post(&shm->bonded_mutex);
   }  // else ended

   return;
}

// bonding function
void bond(at_t atom_type, long atom_id, sync_t *shm)
{
   fprt_out(atom_type, atom_id, "begin bonding", shm);

   usleep((rand() % (shm->max_B_time + 1)) * 1000);  // time in microsecconds

   return;
}

// finish bonding function
void finish_bond(at_t atom_type, long atom_id, sync_t *shm)
{
   sem_wait(&shm->at_cnt_mutex);
   shm->N_atoms_bonded++;

   if (shm->N_atoms_bonded == 3) {
      sem_post(&shm->bonding_mtx);
      sem_post(&shm->bonding_mtx);
      sem_post(&shm->bonding_mtx);
      // reset counter
      shm->N_atoms_bonded = 0;
   }

   sem_post(&shm->at_cnt_mutex);

   sem_wait(&shm->bonding_mtx);
   fprt_out(atom_type, atom_id, "bonded", shm);

   return;
}

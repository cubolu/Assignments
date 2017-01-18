/******************************************************************************/
/*buildh2o.h                                                                  */
/*Riesenie:    IOS-projekt2                                                   */
/*Prelozene:   gcc 4.8.2 (ubuntu 14.04)                                       */
/*Autor:       Jakub Lukac                                                    */
/*Login:       xlukac09                                                       */
/*Rocnik:      1. rocnik BIT                                                  */
/*Datum:       30.04.2015                                                     */
/*e-mail:      xlukac09@stud.fit.vutbr.cz                                     */
/******************************************************************************/

#ifndef BUILDH2O_H_
#define BUILDH2O_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>


#define SHM_R   0400    // read permission  (r--------)
#define SHM_W   0200    // write permission (-w-------)

#define OUTPUT_FILE "h2o.out"

// shared counters and semaphores
typedef struct shared_sync_memory sync_t;
struct shared_sync_memory {
   int sh_id;
   FILE *output_file;
   unsigned long N_molecules;
   unsigned int  max_H_time;
   unsigned int  max_O_time;
   unsigned int  max_B_time;

   // operation counters
   unsigned long op_counter;
   unsigned long H_queue;
   unsigned long O_queue;
   unsigned long N_molecules_bonded;
   unsigned long N_atoms_bonded;

   // semaphores for synchronization
   sem_t op_cnt_mutex; // writing to file
   sem_t at_cnt_mutex; // counting bonded atoms while bonding molecule
   sem_t check_mutex;  // checking status
   sem_t bonding_mtx;  // reporting bonded(status) after all atoms finish begin bonding(status)
   sem_t bonded_mutex; // enable checking when two H and one O are bonded
   sem_t finish_mutex; // finished all processes of atoms together
   // waiting queues for atoms
   sem_t bond_queue_H;
   sem_t bond_queue_O;
};

typedef enum {
              HYDROGEN = 'H',
              OXYGEN   = 'O'
             } at_t;

// functions:
//init
sync_t * init_shared_mem(void);
//free
void free_shared_mem(sync_t *removing_shm);
//H_gen
void H_gen(sync_t *shm);
//O_gen
void O_gen(sync_t *shm);
//bond
void try_bond_H(unsigned long atom_id, sync_t *shm);
void try_bond_O(unsigned long atom_id, sync_t *shm);
// report process state to file
void fprt_out(at_t atom_type, unsigned long atom_id, char *state, sync_t *shm);
// atoms exiting together
void atom_exit(at_t atom_type, unsigned long atom_id, sync_t *shm);
// wait children
void wait_for_children(void);

#endif

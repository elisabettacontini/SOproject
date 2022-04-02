#ifndef ASL_H
#define ASL_H

#include "pandos_types.h"
#include "pcb.h"

//funzioni per gestione della ASL
void initASL();
int insertBlocked(int* semAdd, pcb_PTR p);
pcb_PTR removeBlocked(int *semAdd);
pcb_PTR outBlocked(pcb_PTR p);
pcb_PTR headBlocked(int *semAdd);


//funzione ausiliaria che ricerca un SEMD sulla lista di semafori attivi dato l'identificativo al semaforo (ptr a intero)
semd_PTR searchSemd(int* semAdd);



#endif
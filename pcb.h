#ifndef PCB_H
#define PCB_H

#include "pandos_types.h"

// funzioni di inizializzazione/allocazione/deallocazione coda di processi liberi
void initPcbs();
void freePcb(pcb_PTR p);
pcb_PTR allocPcb();

//funzioni per gestione coda di processi liberi
void mkEmptyProcQ(struct list_head* head);
int emptyProcQ(struct list_head* head);
void insertProcQ(struct list_head* head, pcb_PTR p);
pcb_PTR headProcQ(struct list_head* head);
pcb_PTR removeProcQ(struct list_head* head);
pcb_PTR outProcQ(struct list_head* head, pcb_PTR p);

//funzioni per gestione alberi di processi
int emptyChild(pcb_PTR p);
void insertChild(pcb_PTR prnt,pcb_PTR p);
pcb_PTR removeChild(pcb_PTR p);
pcb_PTR outChild(pcb_PTR p);


#endif
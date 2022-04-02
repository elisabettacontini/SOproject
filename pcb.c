#include "pcb.h"

static struct list_head pcbFree_h;        //dichiarazione lista pcbFree_h che indica la coda di processi liberi
static pcb_t pcbFree_table[MAXPROC];

void initPcbs(){
	INIT_LIST_HEAD(&pcbFree_h);      //inizializzazione campi di pcbFree_h (li fa puntare a se stessa)
	
	for(int i = 0; i < MAXPROC; i++)
		list_add_tail(&(pcbFree_table[i].p_list), &pcbFree_h);  //si aggiunge ogni elemento della tabella in pcbFree_h. Inserimento in coda in modo che la testa della lista sia il primo elemento dell'array e la coda l'ultimo
}

void freePcb(pcb_PTR p){
	list_add_tail(&(p->p_list), &pcbFree_h);  //inserisce il pcb_t puntato da p in pcbFree_h
}

pcb_PTR allocPcb(){
	if(list_empty(&pcbFree_h))    // pcbFree_h vuota
		return NULL;
	else{
		pcb_PTR elem = container_of(pcbFree_h.next, pcb_t, p_list);    // si fa puntare "elem" al pcb_t in cui è contenuto il primo elemento di pcbFree_h 
		list_del(&(elem -> p_list));                                   // si rimuove l'emenento puntato da elem dalla lista pcbFree_h
		
		// si inizializzano tutti i campi dell'elemento rimosso a 0/NULL
		INIT_LIST_HEAD(&(elem -> p_list));
		elem -> p_parent = NULL;
		INIT_LIST_HEAD(&(elem -> p_child));
		INIT_LIST_HEAD(&(elem -> p_sib));
		//gestione campo state_t
		elem -> p_s.entry_hi = 0;
		elem -> p_s.cause = 0;
		elem -> p_s.status = 0;
		elem -> p_s.pc_epc = 0;
		for(int i = 0; i < STATE_GPR_LEN; i++)
			elem -> p_s.gpr[i] = 0;
		elem -> p_s.hi = 0;
		elem -> p_s.lo = 0;
		
		elem -> p_supportStruct = NULL;
		elem -> p_time = 0;
		elem -> p_semAdd = NULL;
		
		return elem;
	}
}


void mkEmptyProcQ(struct list_head* head){
	INIT_LIST_HEAD(head);                  //inizializza head come lista vuota (i suoi campi puntano a se stessa)
}


int emptyProcQ(struct list_head* head){
	return list_empty(head);              // se head è vuota ritorna 1, altrimenti 0
}

void insertProcQ(struct list_head* head, pcb_PTR p){   // inserisce il pcb_t puntato da p nella coda dei processi puntata da head
	struct list_head* new = &(p -> p_list);            // si fa puntare new al campo list_head dell'elemento da inserire in lista
	list_add_tail(new, head);                          // inserisce new in coda a head
}

pcb_PTR headProcQ(struct list_head* head){
	if(list_empty(head))
		return NULL;
	
	struct list_head* elem = list_next(head);  //elem punta il campo list_next del primo pcb_t della coda di processi puntata da head
	
	return container_of(elem, pcb_t, p_list);   //restituisce ptr a pcb_t in cui è contenuto list_head puntato da elem
}

pcb_PTR removeProcQ(struct list_head* head){
	if(list_empty(head))
		return NULL;
	
	struct list_head* elem = list_next(head); // elem punta al primo pcb_t della coda dei processi puntata da head
	list_del(list_next(head));                // rimuove il primo pcb_t dalla coda dei processi
	
	return container_of(elem, pcb_t, p_list); // ritorna il ptr a pcb_t rimosso (cioè puntato da elem)
}

pcb_PTR outProcQ(struct list_head* head, pcb_PTR p){
	if (p == NULL || list_empty(head))
        return NULL;
	
	pcb_PTR pos;                               // ptr che itera sul contenuto della lista
	list_for_each_entry(pos, head, p_list){    //ciclo su ogni elemento pcb_t della lista puntata da head
		if(pos == p){                          //se l'iteratore è uguale all'elemento da rimuovere p, allora lo elimina e lo restituisce
			list_del(&(p -> p_list));
			return p;
		} 
	}
	
	return NULL;                               //nessun elemento della lista è uguale a p
}

int emptyChild(pcb_PTR p){
	return list_empty(&(p -> p_child));       //ritorna TRUE se il campo p_child del pcb_t puntato da p punta a se stesso (cioè è vuoto). pcb_t puntato da p in questo caso è un nodo foglia
}

void insertChild(pcb_PTR prnt,pcb_PTR p){                // p figlio di prnt
	p -> p_parent = prnt;                                // assegna prnt al campo genitore di p
	list_add_tail(&(p -> p_sib), &(prnt -> p_child));    // si aggiunge p in coda alla lista di figli di prnt (si aggiunge quindi in coda alla lista dei fratelli)
}

pcb_PTR removeChild(pcb_PTR p){
	if(emptyChild(p)) 
		return NULL;
	else{
		struct list_head* remove_list_head = (p -> p_child).next;          // L'elemento da rimuovere è quello puntato dal campo next di p_child
		pcb_PTR remove_pcb = container_of(remove_list_head, pcb_t, p_sib); // si memorizza il ptr all'elemento di tipo pcb_t da rimuovere
		list_del(remove_list_head); 									   //si rimuove tale elemento e si restituisce
		return remove_pcb;
	}
}

pcb_PTR outChild(pcb_PTR p){
	if(p == NULL || (p -> p_parent == NULL))                 //ritorna NULL se p punta a un nodo radice (cioè pcb_t che non ha padre)
		return NULL;
	
	pcb_PTR pos;                                             //iteratore su elementi della lista
	struct list_head* head = &(p -> p_parent -> p_child);    //puntatore alla lista di figli del padre di p. Cioè puntatore alla lista in cui è contenuto l'elem puntato da p (se presente nell'albero)
	list_for_each_entry(pos, head, p_sib){                   //p_sib -> campo che contiene la list_head sentinella della lista in cui si cerca il pcb da rimuovere
		if(p == pos){
			list_del(&(p -> p_sib));                         //rimozione elemento
			return p;
		}
	}
	
	return NULL;                                             //nessun elemento della lista uguale a p 
}








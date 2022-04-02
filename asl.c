#include "asl.h"

static struct list_head semdFree_h;       //dichiarazione lista di SEMD liberi o inutilizzati (pronti per essere allocati)
static struct list_head semd_h;           //dichiarazione lista di semafori attivi (cioè lista di SEMD associati a semafori su cui sono bloccati processi)
static semd_t semd_table[MAXPROC]; 

void initASL(){
	INIT_LIST_HEAD(&semdFree_h);          // inizializzazione delle variabili globali
	INIT_LIST_HEAD(&semd_h);
	
	for(int i = 0; i < MAXPROC; i++){
		INIT_LIST_HEAD(&semd_table[i].s_link);                 //coda di semafori su ogni SEMD inizializzata come vuota
		INIT_LIST_HEAD(&semd_table[i].s_procq);                //coda di processi bloccati su ogni SEMD inizializzata come vuota
		
		/* si inserisce ogni SEMD della tabella in semdFree_h. 
		Quindi il campo s_link di ogni SEMD da ora concatenerà elementi sulla lista di semafori liberi. semd_h rimane vuota */
		list_add_tail(&(semd_table[i].s_link), &semdFree_h);   
	}
}

int insertBlocked(int* semAdd, pcb_PTR p){
	semd_PTR toInsert = searchSemd(semAdd);         //ricerca il semd a cui è associata la chiave semAdd
	
	if(toInsert == NULL){                           //semd cercato non presente nella ASL
		
		if(list_empty(&semdFree_h))                 
			return TRUE;                            //non più SEMD allocabili, cioè semdFree_h vuota
		else{
			//si estrae un SEMD da semdFree_h e lo si inserisce in semd_h, cioè nella ASL. Si settano i campi del nuovo SEMD
			
			struct list_head* semd_alloc = list_next(&semdFree_h); //semd_alloc punta al campo list_head dell'elemento da rimuovere
			list_del(semd_alloc);                                  //si rimuove il SEMD da allocare da semdFree_h
			toInsert = container_of(semd_alloc, semd_t, s_link);   //toInsert ora punta al SEMD da allocare 
			
			//se ne settano i campi 
			toInsert -> s_key = semAdd;
			insertProcQ(&(toInsert -> s_procq), p);				   //si inserisce il processo bloccato "p" nella coda s_procq del nuovo SEMD
			
			list_add_tail(semd_alloc, &semd_h);                    //si inserisce il nuovo SEMD in semd_h
		}
		
	}else{
		//semd cercato presente nella ASL e sarà quello puntato da toInsert. si inserisce il processo bloccato "p" nella sua coda s_procqù
		insertProcQ(&(toInsert -> s_procq), p);
	}
	
	p -> p_semAdd = semAdd;    //si assegna l'identificativo associato al SEMD anche nel corrispondente campo del pcb inserito
	
	return FALSE;              //inserimento non bloccato -> processo inserito nel SEMD associato correttamente
}


pcb_PTR removeBlocked(int* semAdd){
	semd_PTR toSearch = searchSemd(semAdd);             //ricerca il semd a cui è associata la chiave semAdd
	
	if(toSearch == NULL)                                //semd cercato non presente nella ASL
		return NULL;            
	
	pcb_PTR removedPcb = removeProcQ(&(toSearch -> s_procq));  //si rimuove il primo pcb dalla coda dei processi bloccati
	
	if(list_empty(&(toSearch -> s_procq))){                     //se la coda dei processi bloccati associata al SEMD è vuota bisogna rendere teale SEMD libero
		list_del(&(toSearch -> s_link));        			   //si elimina il SEMD dalla ASL e si aggiunge alla semdFree_h
		list_add(&(toSearch -> s_link), &(semdFree_h));        
		toSearch -> s_key = NULL;                              //SEMD vuoto quindi non associato più a nessuna chiave      
	} 
	
	return removedPcb;
}


pcb_PTR outBlocked(pcb_PTR p){
	if(p -> p_semAdd == NULL)                               //pcb in input non bloccato su nessuna coda di processo
		return NULL;
	
	semd_PTR toSearch = searchSemd(p -> p_semAdd);          //ricerco il SEMD su cui eventualmente p è bloccato attraverso il suo campo p_semAdd
	
	if(toSearch == NULL)                                    //semd cercato non presente nella ASL
		return NULL;
	
	pcb_PTR pcbOut = outProcQ(&(toSearch -> s_procq), p);   // ricerca ed elimina p dalla coda dei processi bloccati di toSearch. outProcQ in 'pcb.h'
	
	if(pcbOut == NULL)                                      // il pcb non compare in tale coda
		return NULL;
	
	if(list_empty(&(toSearch -> s_procq))){                  //se la coda dei processi bloccati associata al SEMD è vuota bisogna rendere teale SEMD libero
		list_del(&(toSearch -> s_link));        			//si elimina il SEMD dalla ASL e si aggiunge alla semdFree_h
		list_add(&(toSearch -> s_link), &(semdFree_h));        
		toSearch -> s_key = NULL;                           //SEMD vuoto quindi non associato più a nessuna chiave      
	}
    
	return pcbOut;
}


pcb_PTR headBlocked(int *semAdd){
	semd_PTR toSearch = searchSemd(semAdd);                   //ricerca il semd a cui è associata la chiave semAdd
	
	if(toSearch == NULL || list_empty(&(toSearch -> s_procq)))   //semd cercato non presente nella ASL oppura la sua coda di processi bloccati è vuota
		return NULL;    
	
	return headProcQ(&(toSearch -> s_procq));                 //ritorna il ptr al pcb in testa a s_procq di toSearch. headProcQ funz in 'pcb.h'
}


semd_PTR searchSemd(int* semAdd){                 //restituisce ptr al SEMD della ASL la cui chiave è uguale a semAdd. Return NULL se elem non trovato
	semd_PTR pos;                                 //iteratore
	list_for_each_entry(pos, &semd_h, s_link){    //itera sulla lista dei semafori attivi il cui concatenatore è s_link
		if(pos -> s_key == semAdd)                //se si trova il semaforo identificato dalla chiave passata allora se ne ritorna il puntatore 
			return pos;
	}
	
	return NULL;                                  //semaforo non trovato
}

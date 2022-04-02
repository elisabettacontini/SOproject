
//include


int processCount;    //n° di processi iniziati non ancora terminati
int softBlockCount;  //n° di processi bloccati a causa di un I/O o una timer request
int idProcCount;     //contatore incrementale per id processi creati

//code di pcb nello stato 'ready', rispettivamente per processi ad alta e bassa priorità
struct list_head highPrioQueue;
struct list_head lowPrioQueue;

pcb_PTR currentProc;  //ptr al processo correntemente in esecuzione

int deviceSemaphores[TOT_SEMD]; //array di semafori (49), uno per ogni dispositivo in uMps3 + 1 per lo Pseudo-Clock

passupvector_t* pass_up_vector;

//estensione visibilità funzioni presenti nel file p2test0.1.c
extern void uTLB_RefillHandler();
extern void test();

int main(){
	
	//inizializzazione variabili globali
	processCount = 0;
	softBlockCount = 0;
	idProcCount = 0;
	mkEmptyProcQ(&highPrioQueue);
	mkEmptyProcQ(&lowPrioQueue);
	currentProc = NULL;
	for(int i = 0; i < TOT_SEMD; i++){
		deviceSemaphores[i] = 0;
	}
	
	//inizializzazione strutture dati asl e pcb
	initPcbs();
	initASL();
	
	//popolare passUpVector
	pass_up_vector = (passupvector_t*) PASSUPVECTOR;
	pass_up_vector -> tlb_refill_handler = (memaddr) uTLB_RefillHandler;
	pass_up_vector -> tlb_refill_stackPtr = (memaddr) KERNELSTACK;
	pass_up_vector -> exception_handler = (memaddr) exceptionHandler;    //TODO: implementare funz exceptionHandler nel file per gestine eccezioni
	pass_up_vector -> exception_stackPtr = (memaddr) KERNELSTACK;
	
	//caricamente Interval Timer a 100 ms
	LDIT(PSECOND);
	
	//allocazione primo processo (kernel mode) che punterà alla funzione test()
	pcb_PTR firstPcb = allocPcb();
	
	if(firstPcb != NULL){
		
		firstPcb -> p_prio = PROCESS_PRIO_LOW;
		firstPcb -> p_pid = idProcCount;
		idProcCount++;
		
		
		// inizializzazione 'stato processore' del pcb allocato 
		/* si modificano i campi di bit dello 'status register':
		-si settano tutti a 0 (ALLOFF)
		-si settano i previus bit (2-3) per abilitare i (global) interrupt (IEPON) e kernel mode on (gia a 0) 
		-si settano i bit 8-15 per abilitare l'interrupt mask (IMON)
		-si setta il bit 27 per abilitare il Local timer (TEBITON)*/
		firstPcb -> p_s.status  = ALLOFF | IEPON | IMON | TEBITON;
		
		RAMTOP(firstPcb -> p_s.reg_sp);  //SP settato a RAMTOP. reg_sp -> gpr[26]
		//si fa puntare PC a test()  
		firstPcb -> p_s.pc_epc = (memaddr) test;
		firstPcb -> p_s.reg_t9 = (memaddr) test;   // reg_t9 -> gpr[24]
		
		//si inserisce il primo pcb nella coda ready a bassa priorità
		insertProcQ(&lowPrioQueue, firstPcb);
		processCount++;
		
		
		scheduler();
		
	}else{
		
		PANIC();
	}
	
	return 0;
}
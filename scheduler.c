#include "scheduler.h"

extern pcb_PTR currentProc;
extern int processCount;
extern int softBlockCount;

void scheduler(){
	
	if(!emptyProcQ(&highPrioQueue)){ // Coda dei processi ad alta priorità NON vuota

		currentProc = removeProcQ(&highPrioQueue);
        LDST(&(currentProc->p_s));

	}else if(!emptyProcQ(&lowPrioQueue)){ // Coda dei processi a bassa priorità NON vuota

		currentProc = removeProcQ(&lowPrioQueue);
		setTIMER(5000);
		LDST(&(currentProc->p_s));

	}else if(processCount == 0){ // Code dei processi alta/bassa priorità vuote e processoCount uguale a zero

		HALT();

	}else if(processCount > 0 && softBlockCount > 0){

		// Abilitiamo gli interrupt e disabilitiamo il PLT attraverso lo Status Register
		currentProc -> p_s.status = ALLOFF | IEPON; 

		WAIT();

	}else if(processCount > 0 && softBlockCount == 0){ //Rilevatore deadlock

		PANIC();
	}
}
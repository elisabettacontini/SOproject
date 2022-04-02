#include "scheduler.h"
#include "pandos_const.h"
#include "pandos_types.h"

extern pcb_PTR currentProc;

void scheduler(){
	
	if(!emptyProcQ(&highPrioQueue)){
		currentProc = removeProcQ(&highPrioQueue);
        LDST(&(currentProc->p_s));
	}else if(!emptyProcQ(&lowPrioQueue)){
		currentProc = removeProcQ(&lowPrioQueue);
		setTIMER(5000);
		LDST(&(currentProc->p_s));
	}else{
		
	}
}
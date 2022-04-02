#include "scheduler.h"

void scheduler(){
	
	if(!emptyProcQ(&highPrioQueue)){
		
	}else if(!emptyProcQ(&lowPrioQueue)){
		currentProc = removeProcQ(&lowPrioQueue);
		setTIMER(5000);
		
	}else{
		
	}
}
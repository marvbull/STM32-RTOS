#include "kernel.h"
#include "main.h"

//global kernel variables/structs
uint32_t current;
uint32_t* MSP_INIT_VAL;
thread kernel_thread; // -> init kernel_thread.sp; kernel_thread.thread_function;

thread threadQueue[128]; // tabelle mit 2 spalten sp und function!

uint32_t overallThreads = 0;
uint32_t overallPStacks = 0;
uint32_t currentThread;

void osKernelInitialize(void){
	MSP_INIT_VAL = *(uint32_t**)0x0;
	current = (uint32_t)MSP_INIT_VAL;

	//set the priority of PendSV to almost the weakest
	SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV
}

void SVC_Handler_Main( unsigned int *svc_args )
{
	unsigned int svc_number;
	/*
	 * Stack contains:
	 * r0, r1, r2, r3, r12, r14, the return address and xPSR
	 * First argument (r0) is svc_args[0]
	 */
	svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
	switch( svc_number )
	{
		case 17: //17 is sort of arbitrarily chosen
			printf("Success!\r\n");
			break;

		case 0:
			__set_PSP((uint32_t)threadQueue[0].sp);
			runFirstThread();
			break;

		case 99:
			//Pend an interrupt to do the context switch
			_ICSR |= 1<<28;
			__asm("isb");
		break;

		default: /* unknown SVC */
			break;
	}
}

//Wrapper functions
void osYield(void)
{
	__asm("SVC #99");
}

void osKernelStart(void)
{
	__asm("SVC #0");
}

uint32_t* allocate_stack(void){
	if (current > (uint32_t)MSP_INIT_VAL - (0x4000 - 0x400)){
		uint32_t PSP_val = current - (0x400 * (overallPStacks+1)); //0x400 soll die main memory größe sein
		current = PSP_val;
		overallPStacks++;
		return (uint32_t*)PSP_val;
	}
	else{
		return NULL;
	}
}

bool osCreateThread(uint32_t* functionptr, void* arg){ // -> init kernel_thread.sp; kernel_thread.thread_function;
	threadInput* threadArgs = (threadInput*)arg;
	threadQueue[overallThreads].sp = allocate_stack();
	if (threadQueue[overallThreads].sp == NULL){
		return false;
	}
	else{ // setting sequence: xPSR, PC, LR, R12, R3, R2, R1, R0, and then R11 through R4
		threadQueue[overallThreads].timeslice = 5; // in ms
		threadQueue[overallThreads].runtime = 5;
		threadQueue[overallThreads].thread_function = functionptr; //thread queue -> damit ich weiß wo ich hinspringen muss
		*(--threadQueue[overallThreads].sp) = 1<<24; //A magic number, this is xPSR
		*(--threadQueue[overallThreads].sp) = (uint32_t)threadQueue[overallThreads].thread_function; //the function name
		for (int i = 0; i < 14; i++) {
			if (i == 6){
				*(--threadQueue[overallThreads].sp) = (uint32_t)threadArgs->arg1;
			}
			else if (i == 5){
				*(--threadQueue[overallThreads].sp) = (uint32_t)threadArgs->arg2;
			}
			else if (i == 4){
				*(--threadQueue[overallThreads].sp) = (uint32_t)threadArgs->arg3;
			}
			else{
			*(--threadQueue[overallThreads].sp) = 0xA; //An arbitrary number
			}
		}
		overallThreads++;
		return true;
	}
}

bool osCreateThreadWithDeadline(uint32_t* functionptr, void* arg, void* runtime, void* timeslice){ // -> init kernel_thread.sp; kernel_thread.thread_function;
	threadInput* threadArgs = (threadInput*)arg; // dont really matter
	threadQueue[overallThreads].sp = allocate_stack();
	if (threadQueue[overallThreads].sp == NULL){
		return false;
	}
	else{ // setting sequence: xPSR, PC, LR, R12, R3, R2, R1, R0, and then R11 through R4
		threadQueue[overallThreads].runtime = runtime; // in ms
		threadQueue[overallThreads].timeslice = timeslice;
		threadQueue[overallThreads].thread_function = functionptr; //thread queue -> damit ich weiß wo ich hinspringen muss
		*(--threadQueue[overallThreads].sp) = 1<<24; //A magic number, this is xPSR
		*(--threadQueue[overallThreads].sp) = (uint32_t)threadQueue[overallThreads].thread_function; //the function name
		for (int i = 0; i < 14; i++) {
			if (i == 6){
				*(--threadQueue[overallThreads].sp) = (uint32_t)threadArgs->arg1;
			}
			else if (i == 5){
				*(--threadQueue[overallThreads].sp) = (uint32_t)threadArgs->arg2;
			}
			else if (i == 4){
				*(--threadQueue[overallThreads].sp) = (uint32_t)threadArgs->arg3;
			}
			else{
			*(--threadQueue[overallThreads].sp) = 0xA; //An arbitrary number
			}
		}
		overallThreads++;
		return true;
	}
}

void osSched(){
	threadQueue[currentThread].sp = (uint32_t*)(__get_PSP() - 8*4);
	currentThread = (currentThread+1)%overallThreads;
	__set_PSP(threadQueue[currentThread].sp);
	return;
} // round robin immer

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <stdbool.h>

#define STACKSIZE 0x400
#define TIMESLICE 5
#define RUNTIME

#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 // PendSV is bits 23-16
#define _ICSR *(uint32_t*)0xE000ED04 //This lets us trigger PendSV

void SVC_Handler_Main( unsigned int *svc_args );
void print_success(void);
void print_187(void);
void print_76(void);
void osKernelStart(void);
extern uint32_t* stackptr;

uint32_t* allocate_stack(void);
bool osCreateThread (uint32_t* functionptr, void* arg);
bool osCreateThreadWithDeadline(uint32_t* functionptr, void* arg, void* runtime, void* timeslice);
void osKernelInitialize(void);

void osSched();
void osYield(void);

typedef struct k_thread{
	uint32_t* sp; //stack pointer
	void (*thread_function)(void*); //function pointer
	uint32_t* timeslice;
	uint32_t* runtime;
}thread;

#endif /* INC_KERNEL_H_ */

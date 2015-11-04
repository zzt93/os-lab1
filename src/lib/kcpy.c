#include "kernel/process.h"
#include "lib/string.h"

void copy_from_kernel(PCB* pcb, void* dest, void* src, int len){
	memcpy(dest, src, len);
}

void copy_to_kernel(PCB* pcb, void* dest, void* src, int len){
	memcpy(dest, src, len);
}

void strcpy_to_kernel(PCB* pcb, char* dest, char* src){
	strcpy(dest, src);
}

void strcpy_from_kernel(PCB* pcb, char* dest, char* src){
	strcpy(dest, src);
}

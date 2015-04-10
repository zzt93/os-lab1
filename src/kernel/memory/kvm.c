#include "x86/x86.h"
#include "memory.h"
#include "string.h"

/* This source file involves some hardware details. Please refer to 
 *  _ ____   ___    __    __  __                         _ 
 * (_)___ \ / _ \  / /   |  \/  |                       | |
 *  _  __) | (_) |/ /_   | \  / | __ _ _ __  _   _  __ _| |
 * | ||__ < > _ <| '_ \  | |\/| |/ _` | '_ \| | | |/ _` | |
 * | |___) | (_) | (_) | | |  | | (_| | | | | |_| | (_| | |
 * |_|____/ \___/ \___/  |_|  |_|\__,_|_| |_|\__,_|\__,_|_|
 */                                                               

/* These data structures are shared by all kernel threads. */
static CR3 kcr3;											// kernel CR3
static PDE kpdir[NR_PDE] align_to_page;						// kernel page directory
static PTE kptable[PHY_MEM / PAGE_SIZE] align_to_page;		// kernel page tables

/* You may use these interfaces in the future */
inline CR3* get_kcr3() {
	return &kcr3;
}

inline PDE* get_kpdir() {
	return kpdir;
}

inline PTE* get_kptable() {
	return kptable;
}

/* Build a page table for the kernel */
void
init_page(void) {
	CR0 cr0;
	CR3 cr3;
	PDE *pdir = (PDE *)va_to_pa(kpdir);
	PTE *ptable = (PTE *)va_to_pa(kptable);
	uint32_t pdir_idx, ptable_idx, pframe_idx;


	for (pdir_idx = 0; pdir_idx < NR_PDE; pdir_idx ++) {
		make_invalid_pde(&pdir[pdir_idx]);
	}

	pframe_idx = 0;
	for (pdir_idx = 0; pdir_idx < PHY_MEM / PD_SIZE; pdir_idx ++) {
		make_pde(&pdir[pdir_idx], ptable);
		make_pde(&pdir[pdir_idx + KOFFSET / PD_SIZE], ptable);
		for (ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx ++) {
			make_pte(ptable, (void*)(pframe_idx << 12));
			pframe_idx ++;
			ptable ++;
		}
	}

	/* make CR3 to be the entry of page directory */
	cr3.val = 0;
	cr3.page_directory_base = ((uint32_t)pdir) >> 12;
	write_cr3(&cr3);

	/* set PG bit in CR0 to enable paging */
	cr0.val = read_cr0();
	cr0.paging = 1;
	write_cr0(&cr0);

	/* Now we can access global variables! 
	 * Store CR3 in the global variable for future use. */
	kcr3.val = cr3.val;
}

/* One TSS will be enough for all processes in ring 3. It will be used in Lab3. */
static TSS tss; 

inline static void
set_tss(SegDesc *ptr) {
	tss.ss0 = SELECTOR_KERNEL(SEG_KERNEL_DATA);		// only one ring 0 stack segment

	uint32_t base = (uint32_t)&tss;
	uint32_t limit = sizeof(TSS) - 1;
	ptr->limit_15_0  = limit & 0xffff;
	ptr->base_15_0   = base & 0xffff;
	ptr->base_23_16  = (base >> 16) & 0xff;
	ptr->type = SEG_TSS_32BIT;
	ptr->segment_type = 0;
	ptr->privilege_level = DPL_USER;
	ptr->present = 1;
	ptr->limit_19_16 = limit >> 16;
	ptr->soft_use = 0;
	ptr->operation_size = 0;
	ptr->pad0 = 1;
	ptr->granularity = 0;
	ptr->base_31_24  = base >> 24;
}

inline void set_tss_esp0(uint32_t esp) {
	tss.esp0 = esp;
}

/* GDT in the kernel's memory, whose virtual memory is greater than 0xC0000000. */
static SegDesc gdt[NR_SEGMENTS];

static void
set_segment(SegDesc *ptr, uint32_t pl, uint32_t type) {
	ptr->limit_15_0  = 0xFFFF;
	ptr->base_15_0   = 0x0;
	ptr->base_23_16  = 0x0;
	ptr->type = type;
	ptr->segment_type = 1;
	ptr->privilege_level = pl;
	ptr->present = 1;
	ptr->limit_19_16 = 0xF;
	ptr->soft_use = 0;
	ptr->operation_size = 0;
	ptr->pad0 = 1;
	ptr->granularity = 1;
	ptr->base_31_24  = 0x0;
}

/* This is similar with the one in the bootloader. However the
   previous one cannot be accessed in user process, because its virtual
   address below 0xC0000000, and is not in the process' address space. */

void
init_segment(void) {
	memset(gdt, 0, sizeof(gdt));
	set_segment(&gdt[SEG_KERNEL_CODE], DPL_KERNEL, SEG_EXECUTABLE | SEG_READABLE);
	set_segment(&gdt[SEG_KERNEL_DATA], DPL_KERNEL, SEG_WRITABLE );
	set_segment(&gdt[SEG_USER_CODE], DPL_USER, SEG_EXECUTABLE | SEG_READABLE);
	set_segment(&gdt[SEG_USER_DATA], DPL_USER, SEG_WRITABLE );

	write_gdtr(gdt, sizeof(gdt));

	set_tss(&gdt[SEG_TSS]);
	write_tr( SELECTOR_USER(SEG_TSS) );
}


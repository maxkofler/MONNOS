
#include "hardcodes/kernel.h"

#include "kernel/vga.h"
#include "kernel/interrupts.h"

#include "kernel/io.h"
#include "kernel/monnos.h"
#include "kernel/keyboard.h"

#include "kernel/string.h"
#include "kernel/memory.h"

void keyboard_handler(registers_t *);
void timer_handler(registers_t *);

uint8_t* _kernel_len = (uint8_t*)KERNEL_LEN_ADDR;

/**
 * @brief	This is the kernel entry point, the kernel never returns,
 * 			hence the loop at the end. The control gets put to events for now, 
 * 			so this function enables external interrupts and puts the OS to sleep
 * 			until an interrupt raises, it then processes it in the handler, works
 * 			and then returns to the loop "while (1)", where it halts itself again,
 *			over and over again, this presents the kernel as a "service" to use 			
 */
void kernel_main(void){
	vga_setup();

	printk("Character test:\n-----\n");
	for (uint8_t i = 0; i < UINT8_MAX; i++)
		printk("%c", i);
	printk("\n-----\n");

	printk("Kernel length: %i blocks of 512 B, totalling to %i bytes\n", *_kernel_len, *_kernel_len*512);
	printk("MONNOS, press ESC to quit\n\n");

	load_keymap(LAYOUT_DE);

	//Start the dynamic memory allocation from the end of the kernel memory
	mem_init_dynamic_memory(KERNEL_LEN_ADDR+1);

	//Setup interrupts
	kernel_setup_interrupts();

	//Register the keypress handler
	register_int_handler(33, key_event);

	//Give control to the interrupts
	asm volatile("sti");

	//If an interrupt ends, halt the CPU again
	while (1)
		asm volatile ("hlt");

	outw(0x604, 0x2000);
}
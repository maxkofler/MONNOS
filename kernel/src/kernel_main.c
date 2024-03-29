#include "hardcodes/kernel.h"

#include "kernel/vga.h"
#include "kernel/interrupts.h"

#include "kernel/io.h"
#include "kernel/monnos.h"
#include "kernel/keyboard.h"

#include "kernel/string.h"
#include "kernel/memory.h"
#include "kernel/syscall.h"

#include "kernel/serial.h"

#define LOG_PREFIX "[KMAIN] "

void keyboard_handler(registers_t *);
void timer_handler(registers_t *);

void serial_handler(registers_t *);

void sample_syscall(registers_t *);

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

	loglevel_current = K_L_DEBUG;

	printk(K_INFO LOG_PREFIX "Character test:\n-----\n");
	for (uint8_t i = 0; i < UINT8_MAX; i++)
		printk("%c", i);
	printk("\n-----\n");

	//Load the german keymap
	load_keymap(LAYOUT_DE);

	//Start the dynamic memory allocation from the end of the kernel memory
	mem_init_dynamic_memory(KERNEL_LEN_ADDR+1);

	//Setup interrupts
	kernel_setup_interrupts();

	//Register the keypress handler
	register_int_handler(33, key_event);
	register_int_handler(36, serial_handler);

	serial_set_divisor(SERIAL_1_PORT, BAUD_110);
	printk(K_DEBUG LOG_PREFIX "Set divisor: 0x%x\n", serial_get_divisor(SERIAL_1_PORT));
	serial_set_reg_bit(SERIAL_1_PORT, SERIAL_REG_IER, SERIAL_INT_DA, 1);

	//Initialize the syscall interface on interrupt 20
	syscall_init(20);
	printk(K_INFO LOG_PREFIX "Testing the syscall interface...\n");

	asm volatile("mov $20, %eax");
	asm volatile("int $20");

	syscall_register(0, sample_syscall);
	syscall_register(MAX_SYSCALLS, sample_syscall);

	asm volatile("mov $0, %eax");
	asm volatile("int $20");

	//Print OS information
	printk("\nMONNOS, press ESC to quit\n\n");

	//Give control to the interrupts
	asm volatile("sti");

	//If an interrupt ends, halt the CPU again
	while (1)
		asm volatile ("hlt");

	outw(0x604, 0x2000);
}

void sample_syscall(registers_t* r){
	printk(K_INFO LOG_PREFIX "Sample syscall got called, id: 0x%x\n", r->eax);
	return;
}

void serial_handler(registers_t *){
	uint8_t rec = inb(SERIAL_1_PORT);

	printk(K_DEBUG LOG_PREFIX "Serial recv: %x (%c)\n", rec, rec);

	outb(SERIAL_1_PORT, rec);
}

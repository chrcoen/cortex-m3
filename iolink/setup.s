.thumb
.global _start
.global _invalid_exception
.global __disable_irq
.global __enable_irq
.global _stop

init_vectors:
.word 0x20001000                @ Initial SP
.word _start + 1                @ Starting Program address
.word _invalid_exception + 1	@ NMI_Handler
.word _invalid_exception + 1	@ HardFault_Handler
.word _invalid_exception + 1    @ MemManage_Handler
.word _invalid_exception + 1    @ BusFault_Handler
.word _invalid_exception + 1	@ UsageFault_Handler
.word 0                         @ 7
.word 0                         @ To
.word 0                         @ 10
.word 0                         @ Reserved
.word _invalid_exception + 1	@ SVC_Handler
.word _invalid_exception + 1	@ DebugMon_Handler
.word 0                         @ Reserved
.word _invalid_exception + 1	@ PendSV_Handler
.word systick_isr           	@ SysTick_Handler
.word _invalid_exception + 1	@ IRQ0
.word _invalid_exception + 1	@ IRQ1
.word _invalid_exception + 1	@ IRQ2
.word _invalid_exception + 1	@ IRQ3
.word _invalid_exception + 1	@ IRQ4
.word _invalid_exception + 1  @ IRQ5
.word _invalid_exception + 1	@ IRQ6
.word _invalid_exception + 1	@ IRQ7
.rept 56
.word _invalid_exception + 1    @ IRQ8 to IRQ63
.endr

_start:
bl c_entry
bl _idle

_invalid_exception:
bl invalid_excp
bl _stop

_stop:
bl __disable_irq
b _idle

__disable_irq:
cpsid i
bx lr

__enable_irq:
cpsie i
bx lr

_idle:
wfi
b _idle


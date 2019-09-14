#include <stdint.h>

typedef uint32_t TickType_t;

#define configCPU_CLOCK_HZ                        (100000000) /* CPU clock frequency is 100 Mhz */
#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
#define configTICK_RATE_HZ                        ((TickType_t) 10000) /* frequency of tick interrupt in Hz*/
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15
#define configPRIO_BITS           4
#define configKERNEL_INTERRUPT_PRIORITY           (configLIBRARY_LOWEST_INTERRUPT_PRIORITY<<(8-configPRIO_BITS))

uint32_t ulPortSetInterruptMask( void );
void vPortClearInterruptMask( uint32_t ulNewMaskValue );
#define portDISABLE_INTERRUPTS()        ulPortSetInterruptMask()
#define portENABLE_INTERRUPTS()         vPortClearInterruptMask(0)
#define portNVIC_SYSPRI2_REG        ( * ( ( volatile uint32_t * ) 0xe000ed20 ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 3

#define portNVIC_SYSTICK_PRI        ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )
#define portNVIC_SYSTICK_CTRL_REG     ( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG     ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_VALUE_REG     ( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SYSTICK_INT_BIT      ( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT     ( 1UL << 0UL )
  #define portNVIC_SYSTICK_CLK_BIT  ( 1UL << 2UL )



#define MODE   ((0x20-0x20)*4)
#define OVLD   ((0x21-0x20)*4)
#define SHRT   ((0x22-0x20)*4)
#define SIO    ((0x23-0x20)*4)
#define UART   ((0x24-0x20)*4)
#define FHC    ((0x25-0x20)*4)
#define OD     ((0x26-0x20)*4)
#define MPD    ((0x27-0x20)*4)
#define DPD    ((0x28-0x20)*4)
#define CYCT   ((0x29-0x20)*4)
#define FHD    ((0x2A-0x20)*4)
#define BLVL   ((0x2B-0x20)*4)
#define IMSK   ((0x2C-0x20)*4)
#define LSEQ   ((0x2D-0x20)*4)
#define LHLD   ((0x2E-0x20)*4)
#define CFG    ((0x2F-0x20)*4)
#define TRSH   ((0x30-0x20)*4)
#define STAT   ((0x60-0x20)*4)
#define SMSK   ((0x61-0x20)*4)
#define SYNC   ((0x62-0x20)*4)
#define PROT   ((0x63-0x20)*4)
#define INT    ((0x64-0x20)*4)
#define REV    ((0x70-0x20)*4)

volatile uint8_t * const CCE4510 = (uint8_t *)0x4000C000;

void master_main(void);
void device_main(void);

void c_entry(void) {

#ifdef IOL_MASTER
    for (int i = 0; i < 0x1000; i++) {
        asm("nop");
    }
#endif

    /* Set systick priority */
    portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

    /* configure and start timer */
    portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );

    CCE4510[MODE] = (3 << 2); // COM3


#ifdef IOL_MASTER
    CCE4510[FHC] = (1 << 2);  // Master
    CCE4510[FHD] = 0x12; // send test data
    CCE4510[FHD] = 0x34;
#else
    CCE4510[FHC] = 0;  // Device
#endif    

    for (;;){
        __asm__ __volatile__("wfi");
    }
}

void invalid_excp(void)
{
}

void systick_isr()
{
  static uint16_t count = 0;
  if (CCE4510[BLVL] >= 2) {
    CCE4510[FHD]; // read one byte
    CCE4510[FHD]; // read one byte
    CCE4510[FHD] = count & 0xff; // write low byte of counter
    CCE4510[FHD] = (count >> 8) & 0xff; // write high byte of counter
    count++;
  }
}

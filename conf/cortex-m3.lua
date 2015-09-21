rom = {
    size = 256,
    target_port = {
        base_addr = 0x00000000,
        high_addr = 0x00040000
    }
}

ram = {
    size = 64,
    target_port = {
        base_addr = 0x20000000,
        high_addr = 0x20010000
    }
}

uart0 = {
    irq_number = 5,
    target_port = {
        base_addr = 0x4000C000,
        high_addr = 0x4000CFFF
    }
}

spi = {
    irq_number = 6,
    target_port = {
        base_addr = 0x4000D000,
        high_addr = 0x4000DFFF
    }
}

timer0 = {
    irq_timerint_number = 7,
    timer_frequency = 1000000,
    target_port = {
        base_addr = 0x40000000,
        high_addr = 0x40000FFF
    }
}

timer1 = {
    irq_timerint_number = 8,
    timer_frequency = 1000000,
    target_port = {
        base_addr = 0x40004000,
        high_addr = 0x40004FFF
    }
}

dualtimer = {
    irq_timint1_number = 9,
    irq_timint2_number = 10,
    timer_frequency = 1000000,
    target_port = {
        base_addr = 0x40003000,
        high_addr = 0x40003FFF
    }
}

gpio = {
    irq_number = 11,
    target_port = {
        base_addr = 0x40050000,
        high_addr = 0x40050079
    }
}

watchdog = {
    wdogint_number = 12,
    wdogclk_frequency = 1000000,
    target_port = {
        base_addr = 0x40006000,
        high_addr = 0x40006FFF
    }
}

CPU = {
    library = "libqbox-cortex-m3.so",
    kernel = "images/testbench.elf",
    quantum = 10000,
    gdb_port = 1234,
    extra_parameters = ""
}

tcp_serial0 = {
    tcp_port = 1236,
    baudrate = 96000
}

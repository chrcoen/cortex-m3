rom_master = {
    size = 256,
    target_port = {
        base_addr = 0x00000000,
        high_addr = 0x00040000
    }
}

rom_slave = {
    size = 256,
    target_port = {
        base_addr = 0x00000000,
        high_addr = 0x00040000
    }
}

ram_master = {
    size = 64,
    target_port = {
        base_addr = 0x20000000,
        high_addr = 0x20010000
    }
}

ram_slave = {
    size = 64,
    target_port = {
        base_addr = 0x20000000,
        high_addr = 0x20010000
    }
}

cce4510_master = {
    target_port = {
        base_addr = 0x4000C000,
        high_addr = 0x4000CFFF
    }
}

cce4510_slave = {
    target_port = {
        base_addr = 0x4000C000,
        high_addr = 0x4000CFFF
    }
}

timer_master = {
    irq_timerint_number = 28,
    timer_frequency = 1000000,
    target_port = {
        base_addr = 0x40000000,
        high_addr = 0x40000FFF
    }
}

timer_slave = {
    irq_timerint_number = 28,
    timer_frequency = 1000000,
    target_port = {
        base_addr = 0x40000000,
        high_addr = 0x40000FFF
    }
}

CPU_master = {
    library = "libqbox-cortex-m3.so",
    kernel = "iolink/master.elf",
    quantum = 10000,
    gdb_port = 1234,
    extra_arguments = "-icount 8,nosleep" -- "-singlestep -D qemu-m3.log -d in_asm,exec,nochain,int,cpu,guest_errors"
    -- extra_arguments = ""
}

CPU_slave = {
    library = "libqbox-cortex-m3-2.so",
    kernel = "iolink/device.elf",
    -- kernel = "/home/chr/repos/cortex-m3/images/testbench/testbench1.elf",
    quantum = 10000,
    gdb_port = 1235,
    extra_arguments = "-icount 8,nosleep" -- "-singlestep -D qemu-m3.log -d in_asm,exec,nochain,int,cpu,guest_errors"
    -- extra_arguments = ""
}


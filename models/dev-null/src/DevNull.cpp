/*
 * DevNull.cpp
 *
 * Copyright (C) 2015, GreenSocs Ltd.
 *
 * Developed by Guillaume Delbergue <guillaume.delbergue@greensocs.com>
 */

#include "DevNull/DevNull.h"
#include "DevNull/IRQ.h"

GS_GUARD_ONLY_EXTENSION(OUT);

/**
 * DevNull SystemC module
 *
 * @param nm Instance name
 * @param irq_socket IRQ socket
 * @param out_socket OUT socket
 */
DevNull::DevNull(sc_module_name nm):
IRQSocket("irq_socket"),
OUTSocket("out_socket")
{
    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types>
        IRQConfig;
    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types>
        OUTConfig;
    IRQConfig.use_mandatory_extension<IRQ_LINE_EXTENSION>();
    IRQSocket.set_config(IRQConfig);
    OUTConfig.use_mandatory_extension<OUT>();
    OUTSocket.set_config(OUTConfig);
}
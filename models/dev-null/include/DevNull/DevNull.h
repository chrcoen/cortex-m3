/*
 * DevNull.h
 *
 * Copyright (C) 2015, GreenSocs Ltd.
 *
 * Developed by Guillaume Delbergue <guillaume.delbergue@greensocs.com>
 *
 */

#ifndef DEV_NULL_H
#define DEV_NULL_H

#include <systemc.h>

#include <greenreg/greenreg.h>
#include <greenreg/greenreg_socket.h>
#include "greensignalsocket/green_signal.h"

class DevNull : public sc_module
{
public:
    SC_HAS_PROCESS(DevNull);
    DevNull(sc_core::sc_module_name nm);

    gs_generic_signal::target_signal_multi_socket<DevNull> IRQSocket;
    gs_generic_signal::target_signal_socket<DevNull> OUTSocket;
};

#endif /* !DEV_NULL_H */

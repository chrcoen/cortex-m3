/*
 * IRQ.h
 *
 * Copyright (C) 2015, GreenSocs Ltd.
 *
 * Developed by Guillaume Delbergue <guillaume.delbergue@greensocs.com>
 *
 */

#ifndef IRQ_H
#define IRQ_H

/*
 * This just defines an IRQ extension for GreenSignal to pass the IRQ number and
 * the value.
 */
GS_GUARD_ONLY_EXTENSION(IRQ_LINE_EXTENSION);

typedef struct IRQ_ext_data
{
    uint32_t value;
    uint32_t irq_line;
} IRQ_ext_data;

#endif /* !IRQ_H */

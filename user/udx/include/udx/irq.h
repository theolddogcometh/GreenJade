/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Threaded IRQ model (Linux threaded IRQ mental model).
 *
 * Soft path (PCI skeleton):
 *   request_irq (table bind only)
 *     → host_fire_irq / NOTIFY_WAIT badge
 *     → udx_irq_dispatch (respects soft mask)
 *     → handler: read status, ack MMIO, schedule_work
 *   free_irq / disable_irq in quiesce+remove
 */
#pragma once

#include <udx/types.h>

struct udx_device;

typedef int (*udx_irq_handler_t)(int nIrq, void *pDevId);

#define UDX_IRQF_SHARED  (1u << 0)

/* Linux irqreturn_t mental model for handler return. */
#define UDX_IRQ_NONE     0
#define UDX_IRQ_HANDLED  1

/**
 * Register threaded handler in the UDX table (table-only bind).
 * Delivery is pumped from udx_run (NOTIFY_WAIT freestanding, or
 * udx_host_fire_irq on host). Handlers run in host thread context.
 */
udx_status_t udx_request_irq(int nIrq, udx_irq_handler_t pfnHandler,
                             u32 u32Flags, const char *szName, void *pDevId);

void udx_free_irq(int nIrq, void *pDevId);

/**
 * Soft-mask line (Linux disable_irq mental model, process context).
 * Masked lines skip handler invoke in udx_irq_dispatch. Use in quiesce.
 */
void udx_disable_irq(int nIrq);
void udx_enable_irq(int nIrq);

/** Non-zero if line is soft-masked. */
int udx_irq_is_disabled(int nIrq);

/* Called by UDX core when a notification badge bit is set (internal). */
void udx_irq_dispatch(int nIrq);

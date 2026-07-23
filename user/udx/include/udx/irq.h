/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Threaded IRQ model (Linux threaded IRQ mental model).
 *
 * Soft path (PCI skeleton):
 *   request_irq (table bind only)
 *     → host_fire_irq / NOTIFY_WAIT badge
 *     → udx_irq_dispatch (respects soft mask / nested disable)
 *     → handler: read status, ack MMIO, schedule_work
 *   free_irq / disable_irq in quiesce+remove
 *
 * Soft mask depth:
 *   disable_irq increments a per-line depth; enable_irq decrements.
 *   Fire while depth>0 latches a soft pending bit delivered on final enable.
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
 * Nested: each disable pairs with enable. Depth>0 skips dispatch.
 * A fire while masked latches soft pending; delivered on depth→0.
 */
void udx_disable_irq(int nIrq);
void udx_enable_irq(int nIrq);

/** Non-zero if line is soft-masked (depth > 0). */
int udx_irq_is_disabled(int nIrq);

/** Current soft-mask depth (0 = enabled). */
int udx_irq_disable_depth(int nIrq);

/** Non-zero if a fire was latched while soft-masked. */
int udx_irq_is_pending(int nIrq);

/**
 * Host soft synchronize: no-op on host (handlers run synchronously).
 * Exists so porters can call it on remove like Linux synchronize_irq.
 */
void udx_synchronize_irq(int nIrq);

/** First action name on the line, or NULL if unbound. */
const char *udx_irq_name(int nIrq);

/** Count of bound actions on the line (shared lines may be >1). */
int udx_irq_action_count(int nIrq);

/* Called by UDX core / host when a notification badge bit is set. */
void udx_irq_dispatch(int nIrq);

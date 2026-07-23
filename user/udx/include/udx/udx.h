/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Umbrella header for Linux-shaped UDX (docs/UDX_LINUX_PORTER.md).
 *
 * Product: GREENJADE_UDX / UDX_PRODUCT (see types.h + README markers).
 *
 * Best combo for Linux porters:
 *   Linux probe/remove/irq/dma/mmio names  →  quick mental map
 *   DDI-style quiesce                     →  clean teardown
 *   Caps / IOMMU / hard IRQ hidden        →  GreenJade security
 *   Host inject (udx/host.h)              →  develop before kernel ready
 */
#pragma once

#include <udx/types.h>
#include <udx/device.h>
#include <udx/mmio.h>
#include <udx/irq.h>
#include <udx/dma.h>
#include <udx/work.h>
#include <udx/pci.h>
#include <udx/host.h>
#include <udx/virtq.h>

/**
 * Host entry: initialize UDX runtime (work threads, irq routing stubs).
 * Call once from driver host main before registering drivers.
 * Linux mental model: module_init.
 */
udx_status_t udx_init(void);

/** Shut down UDX runtime. Linux mental model: module_exit path. */
void udx_exit(void);

/**
 * Run event loop until idle budget or udx_request_stop().
 * Full GJ: non-blocking NOTIFY_WAIT → udx_irq_dispatch + work flush.
 * Host: drain work queue (IRQs come from udx_host_fire_irq).
 */
void udx_run(void);

void udx_request_stop(void);

/* Log — host console / GreenJade debug_log. */
void udx_printk(const char *szFmt, ...);

/*
 * Optional convenience: many Linux drivers use module_init(fn).
 * On UDX, call from main — these macros document intent only.
 */
#define udx_module_init(fn)  /* call (fn) from main after udx_init */
#define udx_module_exit(fn)  /* call (fn) before udx_exit */

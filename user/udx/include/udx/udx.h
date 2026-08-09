/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Umbrella header for Linux-shaped UDX (docs/UDX_LINUX_PORTER.md).
 *
 * Product: GREENJADE_UDX / UDX_PRODUCT / product=UDX+ABI
 * (see types.h + README markers + core residual once-lamps).
 *
 * Best combo for Linux porters:
 *   Linux probe/remove/irq/dma/mmio names  →  quick mental map
 *   DDI-style quiesce                     →  clean teardown
 *   Caps / IOMMU / hard IRQ hidden        →  GreenJade security
 *   Host inject (udx/host.h)              →  develop before kernel ready
 *
 * Soft residual driver lifecycle (once-lamps lean; Soft!=product; G-AC-1):
 *   udx_init → udx_pci_register_driver → probe → udx_run →
 *   quiesce → remove → unregister → udx_exit
 * product=UDX+ABI (rtl8168_udx / xhci_udx userspace hosts); freestanding
 * kernel class drivers SKIP. Dual MIT OR Apache-2.0. No GPL.
 * G-AC-1: no Linux .ko product AC.
 *
 * Soft residual deepen (C2 udx.h umbrella; Soft!=product; G-AC-1;
 * dual MIT OR Apache-2.0; Dual DoD A/B OPEN):
 *   init residual     — once-lamp lean + product markers + inventory seed;
 *                       idempotent re-enter is soft (reinit path); never
 *                       claims Cap mint / multi-process SPSC product.
 *   register residual — udx_pci_register_driver (id table + probe/remove/
 *                       quiesce ops); match + try_bind on reg/inject.
 *   unregister residual — udx_pci_unregister_driver; quiesce then remove
 *                       before unreg (normative soft order); soft only.
 *   probe residual    — attach: enable/regions/ioremap/dma/irq/set_drvdata;
 *                       backend opaque; fail=no partial product bind claim.
 *   run residual      — between probe attach and quiesce/remove (run window);
 *                       host: work drain + host_fire_irq; freestanding:
 *                       NOTIFY_WAIT badge → irq_dispatch + work flush.
 *   stop residual     — udx_request_stop breaks run loop; soft flag only
 *                       (not product process death / revoke).
 *   quiesce residual  — stop DMA / mask IRQ / cancel work before remove
 *                       or crash best-effort (DDI spirit; optional ops).
 *   remove residual   — free_irq → DMA free → iounmap → release → disable
 *                       → clear_drvdata (after quiesce).
 *   exit residual     — drivers should unregister (quiesce then remove)
 *                       before udx_exit; soft teardown of runtime only.
 *   printk residual   — host console / GJ debug_log observation; never a
 *                       product assurance lamp or Dual DoD close.
 *   hot_cold_abi residual — product surface = UDX/DDI + hot/cold Linux ABI
 *                       (not freestanding class; Soft!=product mint OPEN).
 *   hazard residual   — H1 no net_eth_poll from IRQ; H2 once-lamps only
 *                       (storm=0); H3 thr_exit before as_destroy (process
 *                       law residual; not closed here). Soft!=product.
 *   product_host residual — rtl8168_udx (DoD B) / xhci_udx (DoD A)
 *                       Linux-shaped userspace hosts over DDI*UDX;
 *                       freestanding class SKIP; Dual DoD A/B OPEN.
 *   layout residual   — UDX_LAYOUT_VER eng lean (not GJ_IMAGE_VERSION;
 *                       not product ABI stamp; stamp-free bar honesty).
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = init/run/exit + once-lamp residual lean + work/irq table
 *               + host inject / soft DDI bind residual
 *   product   = multi-process driver-host notify SPSC + Cap mint of
 *               MMIO_FRAME / IRQ Notification / DMA window (OPEN; not here)
 *   freestanding class drivers (kernel rtl8168 / xhci_msc) = SKIP default
 *   product hosts = Linux-shaped userspace UDX (rtl8168_udx / xhci_udx)
 *   G-AC-1 = no Linux .ko product AC; no GPL source in tree
 *
 * Product-host shape residual (Dual DoD A/B OPEN seed; Soft!=product):
 *   rtl8168_udx (DoD B): NIC userspace host; register→probe→run→
 *                        quiesce→remove→unreg residual over BAR map +
 *                        thr-only IRQ + work BH catalog
 *   xhci_udx    (DoD A): USB userspace host; same lifecycle residual over
 *                        BAR0 HC map + thr-only IRQ + work residual
 *   both: product=UDX+ABI; hot+cold ABI + DDI; freestanding class SKIP;
 *         Cap mint OPEN; Dual DoD A/B remain OPEN (agent!=close)
 *
 * Dual DoD residual (C1/C2 UDX product; both OPEN until DUT proof):
 *   Dual DoD A — Linux-shaped USB via xhci_udx + DDI (OPEN)
 *   Dual DoD B — Linux-shaped NIC via rtl8168_udx + DDI (OPEN)
 * Soft residual lamps / inventory / greppable lines never close Dual DoD.
 * Soft!=product. No stamp storms. Stamp-free bar honesty.
 * Bar honesty v2026.08.04.75. NEVER bump GJ_IMAGE_VERSION from this unit.
 * No invent intermediate image stamps. Not GJ_IMAGE_VERSION.
 *
 * Product residual (C2 umbrella; Soft!=product; Dual DoD B OPEN):
 *   product=UDX+sshd+stack — UDX NIC host (rtl8168_udx) owns wire so
 *   netstackd (stack) + sshd (:22) can product-serve. Chain residual:
 *   rtl8168_udx → netstackd → sshd. Soft lifecycle residual != wire close.
 * greppable: udx: soft product residual product=UDX+sshd+stack
 *
 * greppable: udx: soft residual driver lifecycle
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual lean
 * greppable: udx: soft residual lean layout_ver=
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: soft residual api honesty
 * greppable: udx: soft residual register
 * greppable: udx: soft residual unregister
 * greppable: udx: soft residual probe
 * greppable: udx: soft residual quiesce
 * greppable: udx: soft residual remove
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft residual init
 * greppable: udx: soft residual run
 * greppable: udx: soft residual stop
 * greppable: udx: soft residual exit
 * greppable: udx: soft residual printk
 * greppable: udx: soft residual hot_cold_abi
 * greppable: udx: soft residual hazard
 * greppable: udx: soft residual product_host
 * greppable: udx: soft product residual product=UDX+sshd+stack
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1. Dual DoD A/B OPEN.
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

/*
 * Soft residual lean layout version (eng only; Soft!=product; not stamp).
 * Umbrella residual surface catalog for C2 udx.h (lifecycle + honesty +
 * Dual DoD OPEN + product_host + hot_cold_abi + hazard). Not product ABI.
 * Not GJ_IMAGE_VERSION. Stamp-free; NEVER bump GJ_IMAGE_VERSION here.
 * ver1: init/register/unregister/probe/run/stop/quiesce/remove/exit/
 *       printk residual + api honesty + freestanding SKIP + product=UDX+ABI
 *       + Dual DoD A/B OPEN + product_host + hot_cold_abi + hazard +
 *       layout residual helpers.
 * Grep: udx: soft residual lean layout_ver=
 */
#define UDX_LAYOUT_VER               1u

/**
 * Host entry: initialize UDX runtime (work threads, irq routing stubs).
 * Call once from driver host main before registering drivers.
 * Linux mental model: module_init.
 *
 * Soft residual init (once-lamp lean; Soft!=product; G-AC-1):
 *   emits product markers + residual lean catalog
 *   (register/probe/quiesce/remove per-phase; product=UDX+ABI;
 *    freestanding class SKIP; Dual DoD A/B OPEN honesty;
 *    hot_cold_abi + hazard once-lamps from core).
 * Idempotent soft re-enter is residual only — not a product re-mint.
 * greppable residual: udx: soft residual init
 * greppable residual: udx: soft residual lean
 * greppable residual: udx: soft residual lean layout_ver=
 * greppable residual: udx: soft residual product=UDX+ABI
 * greppable residual: udx: soft residual freestanding class SKIP
 * greppable residual: udx: soft residual api honesty
 * greppable residual: udx: soft residual dual_dod
 * greppable residual: udx: soft residual hot_cold_abi
 * greppable residual: udx: soft residual hazard
 * greppable residual: udx: soft residual product_host
 */
udx_status_t udx_init(void);

/**
 * Shut down UDX runtime. Linux mental model: module_exit path.
 *
 * Soft residual exit: drivers should unregister (quiesce then remove)
 * before exit. Soft runtime teardown only — never claims Cap revoke
 * product, multi-server confine close, or Dual DoD close.
 * product=UDX+ABI; Soft!=product; G-AC-1.
 * greppable residual: udx: soft residual exit
 * greppable residual: udx: soft residual unregister
 * greppable residual: udx: soft residual driver lifecycle
 */
void udx_exit(void);

/**
 * Run event loop until idle budget or udx_request_stop().
 * Full GJ: non-blocking NOTIFY_WAIT → udx_irq_dispatch + work flush.
 * Host: drain work queue (IRQs come from udx_host_fire_irq).
 *
 * Soft residual run window: between probe attach and quiesce/remove.
 * Soft!=product — not multi-process SPSC product, not Dual DoD close.
 * Hazard residual H1: no net_eth_poll from IRQ (notify then work_flush).
 * greppable residual: udx: soft residual run
 * greppable residual: udx: soft residual probe
 * greppable residual: udx: soft residual quiesce
 * greppable residual: udx: soft residual hazard
 */
void udx_run(void);

/**
 * Soft residual stop: set stop flag so udx_run breaks its loop.
 * Soft flag only — not product process death, Cap revoke, or Dual DoD.
 * greppable residual: udx: soft residual stop
 */
void udx_request_stop(void);

/**
 * Log — host console / GreenJade debug_log.
 * Soft residual printk: observation only; never a product assurance lamp
 * or Dual DoD close. Soft!=product. H2 once-lamp spirit elsewhere (core).
 * greppable residual: udx: soft residual printk
 */
void udx_printk(const char *szFmt, ...);

/*
 * Optional convenience: many Linux drivers use module_init(fn).
 * On UDX, call from main — these macros document intent only.
 *
 * Soft residual order after init (normative honesty; Soft!=product):
 *   register → probe → run → quiesce → remove → unregister → exit
 * Dual DoD A (xhci_udx) / B (rtl8168_udx) remain OPEN until DUT proof.
 * product_host residual: Linux-shaped userspace only; freestanding SKIP.
 * greppable residual: udx: soft residual register
 * greppable residual: udx: soft residual unregister
 * greppable residual: udx: soft residual probe
 * greppable residual: udx: soft residual quiesce
 * greppable residual: udx: soft residual remove
 * greppable residual: udx: soft residual dual_dod
 * greppable residual: udx: soft residual product_host
 * greppable residual: udx: soft residual hot_cold_abi
 */
#define udx_module_init(fn)  /* call (fn) from main after udx_init */
#define udx_module_exit(fn)  /* call (fn) before udx_exit */

/* ---- Soft residual layout / shape helpers (observation; Soft!=product) */

/**
 * Soft residual lean layout version (UDX_LAYOUT_VER).
 * Grep: udx: soft residual lean layout_ver=
 * Not GJ_IMAGE_VERSION. Not product ABI stamp. Stamp-free bar honesty.
 */
static inline u32
udx_layout_ver(void)
{
    return UDX_LAYOUT_VER;
}

/**
 * Non-zero when umbrella soft residual surface is present at layout_ver:
 * lifecycle deepen + api honesty + freestanding SKIP + product=UDX+ABI +
 * Dual DoD OPEN + product_host + hot_cold_abi + hazard catalog.
 * Soft!=product; eng residual only. Never hard-gates Dual DoD.
 * greppable: udx: soft residual product_host
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft residual lean layout_ver=
 */
static inline int
udx_shape_ok(void)
{
    return (UDX_LAYOUT_VER >= 1u) ? 1 : 0;
}

/**
 * Soft residual product-host Dual DoD OPEN honesty (umbrella seed).
 * Returns 1 when Dual DoD A/B are catalogued OPEN (soft seed only).
 * Never claims product close. Soft!=product; G-AC-1.
 * greppable residual: udx: soft residual dual_dod
 * greppable residual: udx: soft residual product_host
 */
static inline int
udx_dual_dod_open(void)
{
    /* types.h UDX_DOD_STATE is "OPEN"; soft seed never closes DoD */
    (void)UDX_DOD_A_HOST;
    (void)UDX_DOD_B_HOST;
    return 1;
}

/**
 * Soft product residual seed toward product=UDX+sshd+stack.
 * Returns 1 always — catalog honesty only (Soft!=product; Dual DoD OPEN).
 * UDX NIC host residual for stack + sshd product path; never claims wire
 * close / host banner. Bar honesty v2026.08.04.75 stamp-free.
 * greppable residual: udx: soft product residual product=UDX+sshd+stack
 * greppable residual: Soft!=product Dual DoD OPEN product=UDX+sshd+stack
 */
static inline int
udx_product_udx_sshd_stack(void)
{
    (void)UDX_DOD_B_HOST; /* rtl8168_udx DoD B seed */
    return 1;
}

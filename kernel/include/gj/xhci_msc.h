/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal clean-room xHCI + USB MSC BOT stick log path (product T1).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux paste.
 *
 * Public surface:
 *   xhci_msc_init()              — probe first xHCI, bring up MSC BOT if stick
 *   xhci_msc_stick_log_ready()   — 1 if BOT capacity known
 *   xhci_msc_stick_log_write()   — EFI/GREENJADE/KLOG.TXT or raw LBA fallback
 *   xhci_msc_last_stage()        — bring-up stage for panel / serial
 *   xhci_msc_last_cc()           — last xHCI completion code (sticky)
 *
 * Greppable serial markers (soft; never claim HID/bar3):
 *   xhci: init PASS|FAIL|SKIP
 *   xhci: port connect ...
 *   xhci: SS port pick ...
 *   xhci: SS-capable ports ...
 *   xhci: ctrl req=0x.. wlen=.. cc=.. residual=.. slot=.. port=.. spd=..
 *   msc: BOT ready capacity=...
 *   stick: log write PASS|FAIL bytes=... path=KLOG.TXT|raw
 */
#pragma once

#include <gj/types.h>

/**
 * Scan PCI for first xHCI (class 0C:03 prog-if 0x30), map BAR0, HC reset/run,
 * address first connected MSC BOT stick, READ CAPACITY(10). Soft SKIP if no HC.
 *
 * Safe when no xHCI (Multiboot QEMU): returns 0, logs xhci: init SKIP.
 * Returns 0 always for bring-up (never hard-gates boot); readiness is separate.
 *
 * Port scan prefers SuperSpeed-capable ports (protocol xECP / PORTSC speed)
 * so HS internal devices (webcam/HID/BT) do not monopolize bring-up.
 */
int xhci_msc_init(void);

/** Non-zero if MSC BOT path is up and sector capacity is known. */
int xhci_msc_stick_log_ready(void);

/**
 * Last bring-up stage (panel / greppable diagnostics).
 *  0 = not tried
 *  1 = no xHCI
 *  2 = HC reset fail
 *  3 = rings fail
 *  4 = HC up (probing)
 *  5 = no CCS port
 *  6 = port reset fail
 *  7 = Enable Slot fail
 *  8 = enum MSC fail (generic)
 *  9 = READ CAPACITY fail
 * 10 = MSC ready
 * 11 = GET_DESCRIPTOR (device) control fail  ← G752 panel stage-11
 * 12 = Evaluate Context EP0 MPS fail
 * 13 = full device descriptor fail
 * 14 = USB hub skipped (no hub support)
 * 15 = config descriptor fail
 * 16 = no MSC BOT interface
 * 17 = SET_CONFIGURATION fail
 * 18 = Configure Endpoint fail
 * 19 = Address Device command fail (BSR0; real xHCI address path)
 * 20 = legacy software SET_ADDRESS control fail (no longer used; xHCI
 *      addresses via Address Device BSR=0 only — see xhci_msc.c)
 */
u32 xhci_msc_last_stage(void);

/**
 * Sticky last xHCI Transfer/Command Event Completion Code (spec table 6-90).
 * 0 = none / timeout; 1 = Success; 13 = Short Packet; other = fail reason.
 *
 * Panel honesty (path-specific sticky, not overwritten by later arm Success):
 *   stages 19/20       → address-path fail cc (g_u32AddrFailCc; 0=timeout)
 *   stages 11/12/13/15/17 → control-path fail cc (g_u32CtrlFailCc; 0=timeout)
 *   other stages       → last observed completion code
 */
u32 xhci_msc_last_cc(void);

/** Address path used/tried: 0=none 1=BSR0 2=BSR+soft SET_ADDRESS. */
u32 xhci_msc_addr_path(void);

/** Last tried/assigned root-hub port id (1-based); 0 if none. */
u8 xhci_msc_last_port(void);

/**
 * Last PORTSC speed field for that port (xHCI: 1=FS 2=LS 3=HS 4=SS …).
 * 0 if unknown / never sampled.
 */
u8 xhci_msc_last_speed(void);

/**
 * Write log payload to pre-sized EFI/GREENJADE/KLOG.TXT on LUN0 partition 1
 * (FAT32 ESP) when possible; else raw high-LBA area with magic "GJUSBLOG1".
 *
 * @param pBuf  payload bytes
 * @param cb    length (truncated to file/raw budget)
 * @return 0 on success, -1 on failure / not ready
 */
int xhci_msc_stick_log_write(const void *pBuf, u32 cb);

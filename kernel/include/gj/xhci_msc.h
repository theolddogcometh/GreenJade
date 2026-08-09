/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal clean-room xHCI + USB MSC BOT stick log path (lab soft residual /
 * Dual DoD A optional). Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL /
 * no Linux paste. Soft!=product · G-AC-1 (no usb_storage.ko / xhci_pci.ko
 * product AC).
 *
 * Freestanding MSC SKIP default (GJ_XHCI_MSC_PROBE stays 0 · net first):
 *   init returns without HC claim so freestanding net/sshd STATUS can prove.
 *   NEVER re-enable freestanding MSC thrash as product (product USB = xhci_udx
 *   + hot+cold ABI + DDI). Soft residual lean once-shot on SKIP:
 *   product=UDX+ABI path=xhci_udx dual_dod_a=OPEN_UDX (honesty only; not stick
 *   ready; not product T1). Soft!=product.
 *
 * Public surface:
 *   xhci_msc_init()              - probe first xHCI, bring up MSC BOT if stick
 *   xhci_msc_stick_log_ready()   - 1 if BOT capacity known
 *   xhci_msc_stick_log_write()   - EFI/GREENJADE/KLOG.TXT or raw LBA fallback
 *   xhci_msc_last_stage()        - bring-up stage for panel / serial
 *   xhci_msc_last_cc()           - last xHCI completion code (sticky)
 *
 * Greppable serial markers (soft; never claim HID):
 *   xhci: init PASS|FAIL|SKIP
 *   xhci: freestanding MSC SKIP GJ_XHCI_MSC_PROBE=0
 *   xhci: soft residual product=UDX+ABI
 *   xhci: soft residual lean ...
 *   msc: soft residual lean ...
 *   stick: soft residual lean ...
 *   xhci: port connect ...
 *   xhci: SS port pick ...
 *   xhci: SS-capable ports ...
 *   xhci: ctrl req=0x.. wlen=.. cc=.. residual=.. slot=.. port=.. spd=..
 *   msc: BOT ready capacity=...
 *   msc: not_ready reason=...
 *   stick: log write OPEN|PASS|FAIL bytes=... path=none|KLOG.TXT|raw
 *   stick: persist STATUS OPEN ... (MSC not ready; soft fail-closed)
 *   stick: raw log smoke write+read PASS | write OPEN|FAIL
 *
 * Lean residual policy (Soft!=product · Dual DoD A optional · G-AC-1):
 *   Freestanding MSC is lab residual only; product USB = userspace xhci_udx /
 *   UDX+ABI Linux-shaped path. Default SKIP keeps net first. Residual lamps
 *   are once-shot (no stamp storms). No version stamp from this unit.
 * greppable: xhci: soft residual product=UDX+ABI | path=xhci_udx
 * greppable: xhci: soft residual lean | msc: soft residual lean
 * greppable: stick: soft residual lean | dual_dod_a=OPEN_UDX
 * greppable: GJ_XHCI_MSC_PROBE=0 freestanding MSC SKIP Soft!=product
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
 *  1 = no xHCI  (also freestanding SKIP default: GJ_XHCI_MSC_PROBE=0)
 *  2 = HC reset fail
 *  3 = rings fail
 *  4 = HC up (probing)
 *  5 = no CCS port
 *  6 = port reset fail
 *  7 = Enable Slot fail
 *  8 = enum MSC fail (generic)
 *  9 = READ CAPACITY fail
 * 10 = MSC ready
 * 11 = GET_DESCRIPTOR (device) control fail  <- G752 panel stage-11
 * 12 = Evaluate Context EP0 MPS fail
 * 13 = full device descriptor fail
 * 14 = USB hub skipped (no hub support)
 * 15 = GET_DESCRIPTOR(config) fail (G752 high-water; cc=0 = timeout)
 * 16 = no MSC BOT interface
 * 17 = SET_CONFIGURATION fail
 * 18 = Configure Endpoint fail
 * 19 = Address Device command fail (BSR0; real xHCI address path)
 * 20 = legacy software SET_ADDRESS control fail (no longer used; xHCI
 *      addresses via Address Device BSR=0 only - see xhci_msc.c)
 *
 * Panel / serial (soft; Soft!=product) - lean greps only; full stage thrash
 * ladder is not public surface. Opt-in probe=1 residual lives in xhci_msc.c:
 *   XHCI stage=11|15 ... TO  |  msc: not_ready reason=...
 *   msc: ready reason=bot_capacity
 *   msc: progress get_config|set_config|get_desc|...
 *   xhci: get config PASS|FAIL | get device desc PASS|FAIL
 *   xhci: ep0 soft-continue|soft-sync|soft-recover|hard-resync (lab only)
 *   stick: raw log smoke write+read PASS
 *
 * Soft residual lean (freestanding MSC SKIP default · net first · Dual DoD A):
 *   Default stage=1 via SKIP (no HC claim). Once-shot residual inventory lamps
 *   on SKIP/init: product=UDX+ABI path=xhci_udx dual_dod_a=OPEN_UDX.
 *   Stage count never product T1. NEVER freestanding MSC thrash as product.
 * Soft!=product · product USB = userspace xhci_udx + ABI.
 * greppable: xhci: soft residual product=UDX+ABI | path=xhci_udx
 * greppable: xhci: soft residual lean | msc: soft residual lean | stick: soft residual lean
 */
u32 xhci_msc_last_stage(void);

/**
 * Sticky last xHCI Transfer/Command Event Completion Code (spec table 6-90).
 * 0 = none / timeout; 1 = Success; 13 = Short Packet; other = fail reason.
 *
 * Panel honesty (path-specific sticky, not overwritten by later arm Success):
 *   stages 19/20       -> address-path fail cc (g_u32AddrFailCc; 0=timeout)
 *   stages 11/12/13/15/17 -> control-path fail cc (g_u32CtrlFailCc; 0=timeout)
 *   other stages       -> last observed completion code
 */
u32 xhci_msc_last_cc(void);

/** Address path used/tried: 0=none 1=BSR0 2=BSR+soft SET_ADDRESS. */
u32 xhci_msc_addr_path(void);

/** Last tried/assigned root-hub port id (1-based); 0 if none. */
u8 xhci_msc_last_port(void);

/**
 * Last PORTSC speed field for that port (xHCI: 1=FS 2=LS 3=HS 4=SS ...).
 * 0 if unknown / never sampled.
 */
u8 xhci_msc_last_speed(void);

/**
 * Write log payload to pre-sized EFI/GREENJADE/KLOG.TXT on LUN0 partition 1
 * (FAT32 ESP) when possible; else raw high-LBA area with magic "GJUSBLOG1".
 *
 * Soft fail-closed when MSC not ready: returns -1 and emits greppable
 * `stick: log write OPEN ...` (never silent). Ready path unchanged.
 * Soft!=product; not a store_door substitute (store_door = virtio/scsi).
 *
 * @param pBuf  payload bytes
 * @param cb    length (truncated to file/raw budget)
 * @return 0 on success, -1 on failure / not ready (OPEN)
 */
int xhci_msc_stick_log_write(const void *pBuf, u32 cb);

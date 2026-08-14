/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early GOP split-screen text console (panel path without COM1).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0.
 *
 * Layout (wide panels - left | right):
 *   LEFT  - static "hold" status (milestones, XHCI, timer, result)
 *   RIGHT - flying scroll log (kprintf / console_putchar stream)
 *
 * Narrow panels fall back to top (hold) / bottom (scroll).
 * Soft: never hard-gates boot. No-op until fb_console_init succeeds.
 * Colour progress bars are intentionally unused (text-first panel path).
 */
#pragma once

#include <gj/boot_info.h>
#include <gj/types.h>

/**
 * Max fixed status lines on the hold (static) pane.
 * Visual row 0 is the image-version title (test what you fly):
 *   "STATUS (static) v" GJ_IMAGE_VERSION
 *   "STATUS FAULT PINNED v" GJ_IMAGE_VERSION  (after trap pin)
 * Content uses lines 0..N-1. 16 is enough for milestones + Linux module
 * path (7-12) + dual-DoD holds + NET/R residual spare.
 *
 * Hold index map (boot / panel path - see main.c / net_eth / xhci / linux_netdev_soft):
 *   0  headline (bright) / FAULT  — do not clobber title via hold0 after boot
 *   1  phase
 *   2  kernel TE/identity persist (after iommu_vtd_te_arm ~main TE path PASS)
 *      e.g. TE mode=hw tes=1 tt=ML slpt=1 rdy bus3 id1g
 *      once-pin only; te_disarm once-updates same row (not UDX hold14)
 *      early boot: "phase: PCI / IOMMU / xHCI" until TE bring-up
 *      user-kill trap overwrites hold2 with rip/thr/tag (FAULT photo)
 *   3  USB MSC detail
 *   4  USB MSC ready/fail
 *   5  timer / M0
 *   6  NET counters + force refresh (dual DoD B residual lamp; Soft!=product)
 *      e.g. NET name IP UP t0/f0/b0/r0 :22  - t/f/b/r freestanding diagnose only;
 *      /r is R0 (RX dead vs climb). Live STATUS != Dual DoD B close (product=UDX).
 *   7  ksym n=N                 (Linux module path soft)
 *   8  mod r8169 init=...|FAIL|SKIP  (freestanding SKIP lamp; Soft!=product; dim)
 *   9  netdev soft N | unres=...  · on FAULT + short pane: NET residual fallback
 *  10  probe 10ec:8168 soft|miss
 *  11  pci reg=N match=M        (r8169 soft path lamp; Soft!=product)
 *  12  mod xhci_pci ... | SKIP builtin  (freestanding SKIP lamp; Soft!=product; dim)
 *  13  USB / usb_storage lamp (dual DoD A residual; Soft!=product; never product PASS)
 *      e.g. USB linux OPEN builtin | usb_storage need=usbcore | LOAD ok | probe a12f
 *  14  L2 br / freestanding R mirror + live UDX product pins (do not clobber)
 *      e.g. l2 br rx=N tx=M · UDX te_disarm fovw|wire · UDX mac / mdio
 *      kernel TE snapshot lives on hold2 — never steal UDX hold14
 *  15  live UDX product pins / HYBRID wire=... · FAULT: last NET t/f/b/r
 *
 * Dual DoD honesty lean (product=UDX; Soft!=product; G-AC-1):
 *   Product Dual DoD A = Linux-shaped USB via UDX/DDI  - OPEN until DUT proof
 *   Product Dual DoD B = Linux-shaped NIC via UDX/DDI  - OPEN until DUT proof
 *   Soft/freestanding STATUS lamps never close Dual DoD A/B (not freestanding stage).
 * Grep dual DoD residual lamps (honesty only; never product PASS):
 *   dual DoD hold2  - kernel TE/identity persist (mode tes tt slpt bus3 id1g)
 *   dual DoD hold6  - net refresh (t/f/b/r / :22); freestanding diagnose != B close
 *   dual DoD hold13 - USB/usb_storage soft path; LOAD/OPEN != stick datapath PASS
 *   dual DoD hold14 - soft L2 bridge + freestanding R mirror / UDX te_disarm pins
 * Freestanding SKIP honesty lamps (dim paint; Soft!=product):
 *   hold8  mod r8169 SKIP...  - freestanding NIC class residual (GJ_RTL8168_PROBE=0)
 *   hold12 mod xhci_pci SKIP... - freestanding USB class residual (not xhci_udx close)
 *   Any hold text containing " SKIP" / "SKIP " paints dim (never warm PASS look).
 * product=UDX (user drivers): dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX;
 * soft SKIP + hold6/13/14 are honesty residual only (never product DoD close).
 * Long honesty stamps stay on kprintf (LOG filters soft flood); STATUS stays short.
 * Title keeps "STATUS (static) v" GJ_IMAGE_VERSION once (test what you fly).
 * No stamp storms: single title string + short hold lines only (no version spam).
 *
 * NET/l2/USB clip residual (Dual DoD residual diagnose): paint/store prefer
 * t/f/b/r over name/IP head; drop trailing " :22" before losing /r (R0);
 * USB / usb_storage / l2 tails keep counters under clip. Grep: fb_hold_net_clip
 *
 * FAULT STATUS pin (fb_console_trap / g_fFaultHold - product halt only; lean):
 *   Sticky after trap: rows 0 + 6..9 refuse later hold overwrite; LOG putchar
 *   drops so soft/product flood cannot fight FAULT visibility (G752 no COM1).
 *   Title becomes "STATUS FAULT PINNED v" GJ_IMAGE_VERSION; fault rows paint red.
 *   Soft!=product. hold1 (name/PF from trap.c) and dual DoD hold13/14 stay writable.
 *   Dual DoD B residual: last hold6 NET ... t/f/b/r -> hold15 when visible; else
 *   hold9 (short STATUS pane). R0 diagnose survives hold6 clobber.
 * Grep: g_fFaultHold | FAULT PINNED | KERNEL FAULT | dual DoD B R0 | NET residual
 * Grep: STATUS (static) v | GJ_IMAGE_VERSION | product=UDX | Soft!=product
 * Grep: freestanding SKIP | dual_dod_a=OPEN_UDX | dual_dod_b=OPEN_UDX
 * Grep: iommu: vtd TE hold2 | TE mode=
 */
#define FB_HOLD_LINES 16u

/**
 * Hold line buffer width in bytes (includes trailing NUL).
 * Stores at most (FB_HOLD_CHARS-1) printable chars. Dual-DoD short STATUS
 * strings (NET ... t/f/b/r, usb_storage need=usbcore, l2 br rx=/tx=) fit with
 * headroom; longer input is truncated. Dual DoD NET/l2 store+paint prefer
 * counters so /r (R0) stays honest when clipped. Grep: FB_HOLD_CHARS dual DoD
 */
#define FB_HOLD_CHARS 96u

/**
 * Bind to boot_info GOP (32 bpp linear). Clears the screen and draws the
 * split panes. Safe to call more than once.
 */
void fb_console_init(const struct gj_boot_info *pInfo);

/** Non-zero if putchar will paint the scroll pane. */
int fb_console_ready(void);

/** One character to the flying scroll pane (CRLF for '\\n'). */
void fb_console_putchar(char chOut);

/** NUL-terminated string to the scroll pane. */
void fb_console_write(const char *szText);

/**
 * Set one static hold line (0 .. FB_HOLD_LINES-1). Redraws that line only.
 * Copies at most FB_HOLD_CHARS-1 printable chars (stops at CR/LF); NUL-terminates.
 * Dual DoD residual NET/l2/USB store+paint prefer counters under clip so
 * R0 / need= / rx= stay honest. NULL clears the line.
 * Soft!=product: dual-DoD short strings on 6/13/14 are honesty lamps only -
 * never Dual DoD A/B close (product=UDX OPEN). Freestanding SKIP text paints dim.
 * After fb_console_trap: rows 0 + 6..9 sticky; hold15 sticky if NET residual.
 */
void fb_console_hold(u32 u32Line, const char *szText);

/**
 * Convenience: set hold line 0 as a bright headline (same as hold, yellow).
 * No-op after FAULT pin (headline stays FAULT vec=...). Soft!=product.
 */
void fb_console_status(const char *szLine);

/**
 * Pin a kernel trap onto the static STATUS pane (product halt path; lean).
 * Sticky g_fFaultHold: hold 0 + 6..9 refuse later overwrite; LOG paint drops
 * so soft flood cannot bury FAULT. Last dual DoD B NET t/f/b/r on hold6 is
 * snapshotted to hold15 when visible, else hold9 (short pane R0 residual).
 * Safe if console not ready. Soft!=product.
 * Grep: g_fFaultHold | FAULT PINNED | KERNEL FAULT | NET residual
 */
void fb_console_trap(u32 u32Vec, u64 u64Err, u64 u64Rip, u64 u64Cr2,
                     u32 u32Thr, u32 u32State);

/*
 * Legacy layout helpers (no colour bars). Kept so older call sites link.
 * text_y0 always 0 (full-height split). bar_pitch returns 0 (bars disabled).
 */
u32 fb_panel_bar_pitch(u32 u32FbHeight);
u32 fb_panel_text_y0(u32 u32FbHeight);

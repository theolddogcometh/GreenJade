/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early GOP split-screen text console (panel path without COM1).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0.
 *
 * Layout (wide panels - left | right):
 *   LEFT  - static pane, split in half horizontally:
 *             top    STATUS holds (title + live product rows)
 *             bottom STATE (boot) — high-level boot journal
 *   RIGHT - flying scroll log (kprintf / console_putchar stream)
 *
 * Narrow panels: top (holds) / mid (STATE) / bottom (scroll).
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
 * Hold index map (glass — live product only; abandoned .ko/rtl/xhci_msc gone):
 *   0  empty / FAULT vec= / user kill
 *   1  M0 OK dash SKIP isolate  · FAULT/user-kill rip+cr2
 *   2  TE (iommu_vtd once; te_disarm updates)
 *   3  UDX xhci (scratchpad SKIP / product)
 *   4  UDX inj= tx= lnk=        (rtl8168_udx live)
 *   5  UDX own rok fovw c=      (rtl8168_udx glass)
 *   6  UDX mac_rclm / te_re
 *   7  IP 10.200.125.50 :22
 *   8  DoD A=OPEN B=OPEN
 *   9..15 unused (do not write abandoned r8169/ksym/xhci_pci lamps)
 *
 * Dual DoD honesty lean (product=UDX; Soft!=product; G-AC-1):
 *   Product Dual DoD A = Linux-shaped USB via UDX/DDI  - OPEN until host USB path
 *   Product Dual DoD B = Linux-shaped NIC via UDX/DDI  - OPEN until interactive SSH login
 *   Soft/freestanding STATUS lamps never close Dual DoD A/B (not freestanding stage).
 * Grep dual DoD residual lamps (honesty only; never product PASS):
 *   dual DoD hold2  - kernel TE/identity persist (mode tes tt slpt bus3 id1g)
 *   dual DoD hold6  - UDX mac_rclm residual; freestanding diagnose != B close
 * leftover MAP (not live 178 panel): historical hold13 USB / hold14 L2 /
 * hold8 r8169 SKIP / hold12 xhci_pci SKIP. Live 178 uses holds 1/3/4/5/7/8.
 *   Any hold text containing " SKIP" / "SKIP " paints dim (never warm PASS look).
 * product=UDX (user drivers): dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX;
 * leftover hold13/14 MAP is honesty residual only (never product DoD close).
 * Long honesty stamps stay on kprintf (LOG filters soft flood); STATUS stays short.
 * Title keeps "STATUS (static) v" GJ_IMAGE_VERSION once (test what you fly).
 * No stamp storms: single title string + short hold lines only (no version spam).
 *
 * NET/l2/USB clip residual (Dual DoD residual diagnose): paint/store prefer
 * t/f/b/r over name/IP head; drop trailing " :22" before losing /r (R0);
 * USB / usb_storage / l2 tails keep counters under clip. Grep: fb_hold_net_clip
 *
 * FAULT STATUS pin (fb_console_trap / g_fFaultHold - product halt only; lean):
 *   Sticky after trap: rows 0 + 1 refuse later hold overwrite; LOG putchar
 *   drops so soft/product flood cannot fight FAULT visibility (G752 no COM1).
 *   Title becomes "STATUS FAULT PINNED v" GJ_IMAGE_VERSION; fault rows paint red.
 *   Soft!=product. Live pin is 0+1 (leftover comments used to say 0+6..9).
 *   leftover MAP: hold6 NET snap to hold15/hold9 is leftover diagnose, not live pin.
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
 * Append one high-level boot-state line on the bottom of the static pane.
 * Consecutive duplicates are dropped. No-op until fb_console_init succeeds.
 * This is not Dual DoD close. Soft!=product.
 */
void fb_console_state(const char *szLine);

/**
 * Set one static hold line (0 .. FB_HOLD_LINES-1). Redraws that line only.
 * Copies at most FB_HOLD_CHARS-1 printable chars (stops at CR/LF); NUL-terminates.
 * Dual DoD residual NET/l2/USB store+paint prefer counters under clip so
 * R0 / need= / rx= stay honest. NULL clears the line.
 * Soft!=product: dual-DoD short strings on 6/13/14 are honesty lamps only -
 * never Dual DoD A/B close (product=UDX OPEN). Freestanding SKIP text paints dim.
 * After fb_console_trap: rows 0 + 1 sticky (live pin).
 */
void fb_console_hold(u32 u32Line, const char *szText);

/**
 * Convenience: set hold line 0 as a bright headline (same as hold, yellow).
 * No-op after FAULT pin (headline stays FAULT vec=...). Soft!=product.
 */
void fb_console_status(const char *szLine);

/**
 * Pin a kernel trap onto the static STATUS pane (product halt path; lean).
 * Sticky g_fFaultHold: hold 0 + 1 refuse later overwrite; LOG paint drops
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

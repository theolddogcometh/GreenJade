/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early GOP split-screen text console (panel path without COM1).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0.
 *
 * Layout (wide panels — left | right):
 *   LEFT  — static "hold" status (milestones, XHCI, timer, result)
 *   RIGHT — flying scroll log (kprintf / console_putchar stream)
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
 * Visual row 0 is the "STATUS (static)" title; content uses lines 0..N-1.
 * 16 is enough for milestones + Linux module path (7–12) + spare.
 *
 * Hold index map (boot / panel path — see main.c / net_eth / xhci):
 *   0  headline (bright) / FAULT
 *   1  phase
 *   2  phase / soft note
 *   3  USB MSC detail
 *   4  USB MSC ready/fail
 *   5  timer / M0
 *   6  NET counters (product L2)
 *   7  ksym n=N                 (Linux module path soft)
 *   8  mod r8169 init=…|FAIL|SKIP
 *   9  netdev soft N | unres=…
 *  10  probe 10ec:8168 soft|miss
 *  11  pci reg=N match=M        (r8169 PASS path)
 *  12  mod xhci_pci …           (soft xhci path)
 *  13  probe 8086:a12f / skip note
 *  14–15  spare; trap may overwrite 6–10 on fault
 *
 * Soft≠product honesty lives in kprintf, not on these short STATUS lines.
 */
#define FB_HOLD_LINES 16u

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
 * Truncates to the hold pane width. NULL clears the line.
 */
void fb_console_hold(u32 u32Line, const char *szText);

/**
 * Convenience: set hold line 0 as a bright headline (same as hold, yellow).
 */
void fb_console_status(const char *szLine);

/**
 * Pin a kernel trap onto the static STATUS pane so it is not lost when the
 * scroll log advances. Safe if console not ready.
 */
void fb_console_trap(u32 u32Vec, u64 u64Err, u64 u64Rip, u64 u64Cr2,
                     u32 u32Thr, u32 u32State);

/*
 * Legacy layout helpers (no colour bars). Kept so older call sites link.
 * text_y0 always 0 (full-height split). bar_pitch returns 0 (bars disabled).
 */
u32 fb_panel_bar_pitch(u32 u32FbHeight);
u32 fb_panel_text_y0(u32 u32FbHeight);

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch981: CPU pause hint (freestanding no-op).
 *
 * Surface (unique symbols):
 *   void gj_cpu_pause_hint(void);
 *     — Soft hint that a busy-wait may yield a cycle. Freestanding
 *       pure-C: empty volatile touch (no PAUSE insn, no libc).
 *   void __gj_cpu_pause_hint  (alias)
 *   __libcgj_batch981_marker = "libcgj-batch981"
 *
 * Does NOT redefine pause / __libc_pause / __pause_nocancel.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch981_marker[] = "libcgj-batch981";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpu_pause_hint — optional spin-loop yield hint.
 *
 * Empty volatile access so the call is a real side effect and cannot be
 * fully DCE'd when used in a tight wait loop. Not a full memory barrier
 * and not an architecture PAUSE instruction (portable freestanding).
 */
void
gj_cpu_pause_hint(void)
{
	volatile uint32_t u32Tmp;

	u32Tmp = 0u;
	(void)u32Tmp;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_cpu_pause_hint(void)
    __attribute__((alias("gj_cpu_pause_hint")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5485: launcher hook armed-flags predicate.
 *
 * Surface (unique symbols):
 *   int gj_launcher_hook_armed_5485(uint32_t flags);
 *     - Return 1 if flags has required launcher-hook arm bits
 *       (bit0 = install, bit1 = prelaunch, bit2 = postexit), else 0.
 *   int __gj_launcher_hook_armed_5485  (alias)
 *   __libcgj_batch5485_marker = "libcgj-batch5485"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_launcher_hook_armed_5485 surface only;
 * no multi-def. Distinct from gj_launcher_hook_id_5484. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5485_marker[] = "libcgj-batch5485";

/* bit0 install | bit1 prelaunch | bit2 postexit */
#define B5485_HOOK_ARM_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5485_hook_armed(uint32_t uFlags)
{
	if ((uFlags & B5485_HOOK_ARM_MASK) == B5485_HOOK_ARM_MASK) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_launcher_hook_armed_5485 - soft check that launcher hooks are armed.
 *
 * flags: bitmask of launcher hook stages (low three bits required)
 * Returns 1 when bits 0..2 are all set, else 0.
 * Does not call libc. No parent wires.
 */
int
gj_launcher_hook_armed_5485(uint32_t uFlags)
{
	(void)NULL;
	return b5485_hook_armed(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_launcher_hook_armed_5485(uint32_t uFlags)
    __attribute__((alias("gj_launcher_hook_armed_5485")));

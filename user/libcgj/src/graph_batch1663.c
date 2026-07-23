/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1663: /proc-style process state char check.
 *
 * Surface (unique symbols):
 *   int gj_proc_state_char_ok(int ch);
 *     — Return 1 if ch is a recognized Linux /proc/<pid>/stat state
 *       letter (R/S/D/Z/T/t/X/x/K/W/P/I), else 0. Pure predicate.
 *   int __gj_proc_state_char_ok  (alias)
 *   __libcgj_batch1663_marker = "libcgj-batch1663"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1663_marker[] = "libcgj-batch1663";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Accepted state letters (historical + current Linux task states):
 *   R running, S interruptible sleep, D uninterruptible sleep,
 *   Z zombie, T stopped, t tracing stop, X/x dead, K wakekill,
 *   W waking/paging, P parked, I idle.
 */
static int
b1663_is_state(int iCh)
{
	switch (iCh) {
	case 'R':
	case 'S':
	case 'D':
	case 'Z':
	case 'T':
	case 't':
	case 'X':
	case 'x':
	case 'K':
	case 'W':
	case 'P':
	case 'I':
		return 1;
	default:
		return 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proc_state_char_ok — 1 if ch is a known process-state letter.
 *
 * Intended for pure validation of parsed /proc stat fields without
 * opening files or calling the kernel.
 */
int
gj_proc_state_char_ok(int iCh)
{
	return b1663_is_state(iCh);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_proc_state_char_ok(int iCh)
    __attribute__((alias("gj_proc_state_char_ok")));

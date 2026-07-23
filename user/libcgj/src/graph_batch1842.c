/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1842: control-key / C0+DEL predicate.
 *
 * Surface (unique symbols):
 *   int gj_key_is_control(unsigned key);
 *     — Return 1 if key is a US-ASCII control code (C0: 0x00..0x1F)
 *       or DEL (0x7F), else 0. Pure data; no locale / ctype tables.
 *   int __gj_key_is_control  (alias)
 *   __libcgj_batch1842_marker = "libcgj-batch1842"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850). Distinct from
 * gj_key_is_printable_ascii (batch1841). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1842_marker[] = "libcgj-batch1842";

/* C0 control upper bound; DEL is the lone C1-adjacent control in ASCII. */
#define B1842_C0_MAX 0x1fu
#define B1842_DEL    0x7fu

/* ---- freestanding helpers ---------------------------------------------- */

/* True when uKey is C0 (0x00..0x1F) or DEL (0x7F). */
static int
b1842_is_control(unsigned uKey)
{
	if (uKey <= B1842_C0_MAX) {
		return 1;
	}
	if (uKey == B1842_DEL) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_key_is_control — 1 if key is a US-ASCII control code.
 *
 * uKey: key value as unsigned code point
 * Returns 1 for 0x00..0x1F and 0x7F, else 0.
 */
int
gj_key_is_control(unsigned uKey)
{
	return b1842_is_control(uKey);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_key_is_control(unsigned uKey)
    __attribute__((alias("gj_key_is_control")));

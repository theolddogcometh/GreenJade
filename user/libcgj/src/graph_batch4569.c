/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4569: test base64 pad character '=' (_u).
 *
 * Surface (unique symbols):
 *   int gj_b64_is_pad_u(unsigned c);
 *     - 1 if c is the pad mark '='; else 0.
 *   int __gj_b64_is_pad_u  (alias)
 *   __libcgj_batch4569_marker = "libcgj-batch4569"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570). Distinct from
 * gj_b64_is_char_u (batch4563) — pad predicate only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4569_marker[] = "libcgj-batch4569";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4569_is_pad(unsigned uC)
{
	return (uC == (unsigned)'=') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_is_pad_u - 1 if c is the base64 pad character '='.
 *
 * c: code unit to classify
 *
 * Returns 1 for '=', else 0. Self-contained; no parent wires.
 */
int
gj_b64_is_pad_u(unsigned c)
{
	(void)NULL;
	return b4569_is_pad(c);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_b64_is_pad_u(unsigned c)
    __attribute__((alias("gj_b64_is_pad_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5087: debug non-null pointer predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_ptr_ok_u(const void *p);
 *     - Return 1 if p is non-null, else 0. Soft pointer-validity probe
 *       for freestanding debug/assert paths (null check only).
 *   uint32_t __gj_dbg_ptr_ok_u  (alias)
 *   __libcgj_batch5087_marker = "libcgj-batch5087"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique).
 * Unique gj_dbg_ptr_ok_u surface only; no multi-def. Distinct from
 * gj_ptr_nonnull_u (batch4533 continuum). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5087_marker[] = "libcgj-batch5087";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5087_ptr_ok(const void *p)
{
	if (p != NULL) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_ptr_ok_u - 1 if p is non-null, else 0.
 *
 * p: pointer under test
 *
 * Null-only validity; does not probe dereferenceability. No parent wires.
 */
uint32_t
gj_dbg_ptr_ok_u(const void *p)
{
	(void)NULL;
	return b5087_ptr_ok(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_ptr_ok_u(const void *p)
    __attribute__((alias("gj_dbg_ptr_ok_u")));

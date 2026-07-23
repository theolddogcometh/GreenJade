/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7988: atexit kind validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_kind_ok_u_7988(uint32_t kind);
 *     - Return 1 if kind is a live handler kind (1, 2, or 3), else 0.
 *   uint32_t __gj_atexit_kind_ok_u_7988  (alias)
 *   __libcgj_batch7988_marker = "libcgj-batch7988"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_kind_ok_u_7988 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7988_marker[] = "libcgj-batch7988";

/* Live kinds: 1=atexit, 2=cxa, 3=on_exit (0=empty is not live). */
#define B7988_KIND_ATEXIT  ((uint32_t)1u)
#define B7988_KIND_CXA     ((uint32_t)2u)
#define B7988_KIND_ONEXIT  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7988_kind_ok(uint32_t u32Kind)
{
	if (u32Kind == B7988_KIND_ATEXIT)
		return 1u;
	if (u32Kind == B7988_KIND_CXA)
		return 1u;
	if (u32Kind == B7988_KIND_ONEXIT)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_kind_ok_u_7988 - 1 if kind is a registered-handler kind.
 *
 * kind: soft atexit slot kind integer
 *
 * Returns 1 for {1, 2, 3}, else 0 (including empty kind 0). Catalog
 * gate only; does not inspect the real table. No parent wires.
 */
uint32_t
gj_atexit_kind_ok_u_7988(uint32_t u32Kind)
{
	(void)NULL;
	return b7988_kind_ok(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_kind_ok_u_7988(uint32_t u32Kind)
    __attribute__((alias("gj_atexit_kind_ok_u_7988")));

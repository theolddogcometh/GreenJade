/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11127: Iris soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iris_ok_u_11127(void);
 *     - Returns 0 (Iris soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live iris/Intel OpenGL DRI
 *       probe.
 *   uint32_t __gj_iris_ok_u_11127  (alias)
 *   __libcgj_batch11127_marker = "libcgj-batch11127"
 *
 * Exclusive continuum CREATE-ONLY (11121-11130: mesa soft stubs —
 * all soft lamps →0; batch_id→11130). Unique gj_iris_ok_u_11127
 * surface only; no multi-def. Distinct from gj_iris_ok_u_10927,
 * gj_iris_ok_u_10727, gj_iris_ok_u_10027 and sibling mesa soft ok_u stubs in this wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11127_marker[] = "libcgj-batch11127";

/* Soft lamp: Iris ok stub (not asserted). */
#define B11127_IRIS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11127_iris_ok(void)
{
	return B11127_IRIS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iris_ok_u_11127 - Iris soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load iris, open DRI
 * nodes, or probe Intel GL paths. Does not call libc. No parent wires.
 */
uint32_t
gj_iris_ok_u_11127(void)
{
	(void)NULL;
	return b11127_iris_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iris_ok_u_11127(void)
    __attribute__((alias("gj_iris_ok_u_11127")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7984: atexit kind cxa id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_kind_cxa_7984(void);
 *     - Return soft __cxa_atexit handler kind id (2).
 *   uint32_t __gj_atexit_kind_cxa_7984  (alias)
 *   __libcgj_batch7984_marker = "libcgj-batch7984"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_kind_cxa_7984 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7984_marker[] = "libcgj-batch7984";

/* Soft __cxa_atexit kind (2=cxa in atexit table catalog). */
#define B7984_KIND_CXA  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7984_kind_cxa(void)
{
	return B7984_KIND_CXA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_kind_cxa_7984 - soft __cxa_atexit handler kind id.
 *
 * Always returns 2. Catalog constant only; does not call __cxa_atexit.
 * No parent wires.
 */
uint32_t
gj_atexit_kind_cxa_7984(void)
{
	(void)NULL;
	return b7984_kind_cxa();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_kind_cxa_7984(void)
    __attribute__((alias("gj_atexit_kind_cxa_7984")));

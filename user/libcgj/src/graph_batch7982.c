/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7982: atexit kind empty id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_kind_empty_7982(void);
 *     - Return soft empty-slot kind id (0).
 *   uint32_t __gj_atexit_kind_empty_7982  (alias)
 *   __libcgj_batch7982_marker = "libcgj-batch7982"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_kind_empty_7982 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7982_marker[] = "libcgj-batch7982";

/* Soft empty-slot kind (0=empty in atexit table catalog). */
#define B7982_KIND_EMPTY  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7982_kind_empty(void)
{
	return B7982_KIND_EMPTY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_kind_empty_7982 - soft empty handler-slot kind id.
 *
 * Always returns 0. Catalog constant only; does not touch the real
 * atexit table. No parent wires.
 */
uint32_t
gj_atexit_kind_empty_7982(void)
{
	(void)NULL;
	return b7982_kind_empty();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_kind_empty_7982(void)
    __attribute__((alias("gj_atexit_kind_empty_7982")));

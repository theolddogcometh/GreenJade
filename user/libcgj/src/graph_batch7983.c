/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7983: atexit kind atexit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_kind_atexit_7983(void);
 *     - Return soft atexit() handler kind id (1).
 *   uint32_t __gj_atexit_kind_atexit_7983  (alias)
 *   __libcgj_batch7983_marker = "libcgj-batch7983"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_kind_atexit_7983 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7983_marker[] = "libcgj-batch7983";

/* Soft atexit() kind (1=atexit in atexit table catalog). */
#define B7983_KIND_ATEXIT  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7983_kind_atexit(void)
{
	return B7983_KIND_ATEXIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_kind_atexit_7983 - soft atexit() handler kind id.
 *
 * Always returns 1. Catalog constant only; does not call atexit(3).
 * No parent wires.
 */
uint32_t
gj_atexit_kind_atexit_7983(void)
{
	(void)NULL;
	return b7983_kind_atexit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_kind_atexit_7983(void)
    __attribute__((alias("gj_atexit_kind_atexit_7983")));

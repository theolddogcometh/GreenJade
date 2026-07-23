/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7985: atexit kind on_exit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_kind_onexit_7985(void);
 *     - Return soft on_exit handler kind id (3).
 *   uint32_t __gj_atexit_kind_onexit_7985  (alias)
 *   __libcgj_batch7985_marker = "libcgj-batch7985"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_kind_onexit_7985 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7985_marker[] = "libcgj-batch7985";

/* Soft on_exit kind (3=on_exit in atexit table catalog). */
#define B7985_KIND_ONEXIT  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7985_kind_onexit(void)
{
	return B7985_KIND_ONEXIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_kind_onexit_7985 - soft on_exit handler kind id.
 *
 * Always returns 3. Catalog constant only; does not call on_exit(3).
 * No parent wires.
 */
uint32_t
gj_atexit_kind_onexit_7985(void)
{
	(void)NULL;
	return b7985_kind_onexit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_kind_onexit_7985(void)
    __attribute__((alias("gj_atexit_kind_onexit_7985")));

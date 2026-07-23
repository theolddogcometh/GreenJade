/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7981: atexit max handler count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atexit_max_7981(void);
 *     - Return soft atexit table capacity (32).
 *   uint32_t __gj_atexit_max_7981  (alias)
 *   __libcgj_batch7981_marker = "libcgj-batch7981"
 *
 * Exclusive continuum CREATE-ONLY (7981-7990: atexit/handler count stubs —
 * max, kind_empty, kind_atexit, kind_cxa, kind_onexit, slots_ok,
 * slots_full, kind_ok, remaining, batch_id_7990).
 * Unique gj_atexit_max_7981 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7981_marker[] = "libcgj-batch7981";

/* Soft atexit handler table capacity (matches CGJ_ATEXIT_MAX catalog). */
#define B7981_ATEXIT_MAX  ((uint32_t)32u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7981_atexit_max(void)
{
	return B7981_ATEXIT_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atexit_max_7981 - soft atexit max handler count.
 *
 * Always returns 32 (catalog capacity). Does not inspect or register
 * real atexit handlers. No parent wires.
 */
uint32_t
gj_atexit_max_7981(void)
{
	(void)NULL;
	return b7981_atexit_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atexit_max_7981(void)
    __attribute__((alias("gj_atexit_max_7981")));

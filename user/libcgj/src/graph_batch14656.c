/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14656: large-ram 768G class tag soft marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_largeram_768g_class_u_14656(void);
 *     - Returns 768 (soft class tag for 768G large-RAM soak continuum).
 *       Compile-time product tag; not a live GJ_MEM parse.
 *   uint32_t __gj_largeram_768g_class_u_14656  (alias)
 *   __libcgj_batch14656_marker = "libcgj-batch14656"
 *
 * Exclusive continuum CREATE-ONLY (14651-14660: large-ram 768G soak soft
 * markers). Unique surface only; no multi-def. Distinct from sibling pmm
 * order soft tags and prior large-RAM soft markers. No parent wires.
 * Soft markers only — not a claim that desktop 1 TiB bar is met.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14656_marker[] = "libcgj-batch14656";

/* Large-RAM 768G soft class tag. */
#define B14656_LR768  768u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14656_ok(void)
{
	return B14656_LR768;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_largeram_768g_class_u_14656 - soft continuum surface
 *
 * Always returns 768u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_largeram_768g_class_u_14656(void)
{
	(void)NULL;
	return b14656_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_largeram_768g_class_u_14656(void)
    __attribute__((alias("gj_largeram_768g_class_u_14656")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1590: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1590(void);
 *     — Returns the compile-time graph batch number for this TU (1590).
 *   uint32_t __gj_batch_id_1590  (alias)
 *   __libcgj_batch1590_marker = "libcgj-batch1590"
 *
 * Milestone for the units/SI continuum (batches 1581–1589):
 *   KiB/MiB/GiB ↔ bytes, ms/us/s ↔ ns floor/scale helpers.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1590_marker[] = "libcgj-batch1590";

/* ---- freestanding helpers ---------------------------------------------- */

/* Compile-time batch number for this TU. */
static uint32_t
b1590_id(void)
{
	return 1590u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1590 — report this TU's graph batch number.
 *
 * Always returns 1590.
 */
uint32_t
gj_batch_id_1590(void)
{
	return b1590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1590(void)
    __attribute__((alias("gj_batch_id_1590")));

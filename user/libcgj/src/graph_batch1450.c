/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1450: batch identity milestone marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1450(void);
 *     — Returns the compile-time graph batch number for this TU (1450).
 *   uint32_t __gj_batch_id_1450  (alias)
 *   __libcgj_batch1450_marker = "libcgj-batch1450"
 *
 * Does NOT redefine prior batch symbols (gj_batch_id_1360, etc.) —
 * avoid multi-def. Milestone for RNG/hash-mix continuum 1441..1450.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1450_marker[] = "libcgj-batch1450";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1450 — report this TU's graph batch number.
 *
 * Always returns 1450.
 */
uint32_t
gj_batch_id_1450(void)
{
	return 1450u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1450(void)
    __attribute__((alias("gj_batch_id_1450")));

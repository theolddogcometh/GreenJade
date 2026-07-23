/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1520: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1520(void);
 *     — Returns the compile-time graph batch number for this TU (1520).
 *   uint32_t __gj_batch_id_1520  (alias)
 *   __libcgj_batch1520_marker = "libcgj-batch1520"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1520_marker[] = "libcgj-batch1520";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1520 — report this TU's graph batch number.
 *
 * Always returns 1520. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1520(void)
{
	return 1520u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1520(void)
    __attribute__((alias("gj_batch_id_1520")));

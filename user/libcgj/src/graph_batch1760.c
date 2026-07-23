/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1760: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1760(void);
 *     — Returns the compile-time graph batch number for this TU (1760).
 *       Milestone for the exclusive lightweight DNS/wire series
 *       (batches 1751–1759: label/name len, compression ptr/offset,
 *       TYPE A/AAAA, QR response, RCODE NOERROR, CLASS IN).
 *   uint32_t __gj_batch_id_1760  (alias)
 *   __libcgj_batch1760_marker = "libcgj-batch1760"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1760_marker[] = "libcgj-batch1760";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1760_id(void)
{
	return 1760u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1760 — report this TU's graph batch number.
 *
 * Always returns 1760. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1760(void)
{
	return b1760_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1760(void)
    __attribute__((alias("gj_batch_id_1760")));

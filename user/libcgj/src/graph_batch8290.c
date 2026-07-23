/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8290: UTF-8 byte-class probe continuum id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8290(void);
 *     - Returns the compile-time graph batch number for this TU (8290).
 *   uint32_t __gj_batch_id_8290  (alias)
 *   __libcgj_batch8290_marker = "libcgj-batch8290"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8290_marker[] = "libcgj-batch8290";

#define B8290_BATCH_ID  8290u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8290_id(void)
{
	return B8290_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8290 - report this TU's graph batch number.
 *
 * Always returns 8290. Link-time presence tags the UTF-8 / byte class
 * probe exclusive continuum (8281-8290). No parent wires.
 */
uint32_t
gj_batch_id_8290(void)
{
	(void)NULL;
	return b8290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8290(void)
    __attribute__((alias("gj_batch_id_8290")));

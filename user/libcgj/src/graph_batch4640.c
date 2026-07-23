/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4640: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4640(void);
 *     - Returns the compile-time graph batch number for this TU (4640).
 *   uint32_t __gj_batch_id_4640  (alias)
 *   __libcgj_batch4640_marker = "libcgj-batch4640"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, si_kb_to_bytes_u, si_mb_to_bytes_u, bytes_round_page_u,
 * batch_id_4640). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4640_marker[] = "libcgj-batch4640";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4640_id(void)
{
	return 4640u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4640 - report this TU's graph batch number.
 *
 * Always returns 4640. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4640(void)
{
	(void)NULL;
	return b4640_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4640(void)
    __attribute__((alias("gj_batch_id_4640")));

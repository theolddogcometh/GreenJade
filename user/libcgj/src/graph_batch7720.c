/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7720: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7720(void);
 *     - Returns the compile-time graph batch number for this TU (7720).
 *   uint32_t __gj_batch_id_7720  (alias)
 *   __libcgj_batch7720_marker = "libcgj-batch7720"
 *
 * Exclusive continuum CREATE-ONLY (7711-7720: getcwd path max stubs —
 * path_max, name_max, min_buf, buf_ok, root_len, slash, path_len_ok,
 * score, continuum_ready, batch_id_7720). Does NOT redefine gj_batch_id
 * / gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7720_marker[] = "libcgj-batch7720";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7720_id(void)
{
	return 7720u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7720 - report this TU's graph batch number.
 *
 * Always returns 7720. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7720(void)
{
	(void)NULL;
	return b7720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7720(void)
    __attribute__((alias("gj_batch_id_7720")));

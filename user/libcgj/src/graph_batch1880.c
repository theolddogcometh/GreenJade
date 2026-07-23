/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1880: milestone 1880 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1880(void);
 *     — Returns the compile-time graph batch number for this TU (1880).
 *       Milestone for the filesystem exclusive pure-data series
 *       (batches 1871–1879: mode type hints S_IFREG/DIR/LNK/SOCK/FIFO/
 *       CHR/BLK, perms_octal low-12, setuid_p).
 *   uint32_t __gj_batch_id_1880  (alias)
 *   __libcgj_batch1880_marker = "libcgj-batch1880"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1880_marker[] = "libcgj-batch1880";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1880_id(void)
{
	return 1880u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1880 — report this TU's graph batch number.
 *
 * Always returns 1880. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1880(void)
{
	return b1880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1880(void)
    __attribute__((alias("gj_batch_id_1880")));

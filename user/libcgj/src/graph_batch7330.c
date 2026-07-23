/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7330: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7330(void);
 *     - Returns the compile-time graph batch number for this TU (7330).
 *   uint32_t __gj_batch_id_7330  (alias)
 *   __libcgj_batch7330_marker = "libcgj-batch7330"
 *
 * Exclusive continuum CREATE-ONLY (7321-7330: fcntl cmd stubs —
 * dupfd_id, getfd_id, setfd_id, getfl_id, setfl_id, getlk_id,
 * setlk_id, is_getfd, cmd_ok, batch_id_7330).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7330_marker[] = "libcgj-batch7330";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7330_id(void)
{
	return 7330u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7330 - report this TU's graph batch number.
 *
 * Always returns 7330. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7330(void)
{
	(void)NULL;
	return b7330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7330(void)
    __attribute__((alias("gj_batch_id_7330")));

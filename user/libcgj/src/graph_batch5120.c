/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5120: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5120(void);
 *     - Returns the compile-time graph batch number for this TU (5120).
 *   uint32_t __gj_batch_id_5120  (alias)
 *   __libcgj_batch5120_marker = "libcgj-batch5120"
 *
 * Exclusive continuum CREATE-ONLY (5111-5120: sys_nr_read_u, sys_nr_write_u,
 * sys_nr_open_u, sys_nr_close_u, sys_nr_exit_u, sys_is_io_u, sys_is_exit_u,
 * sys_arg_count_u, sys_errno_ok_u, batch_id_5120). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5120_marker[] = "libcgj-batch5120";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5120_id(void)
{
	return 5120u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5120 - report this TU's graph batch number.
 *
 * Always returns 5120. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5120(void)
{
	(void)NULL;
	return b5120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5120(void)
    __attribute__((alias("gj_batch_id_5120")));

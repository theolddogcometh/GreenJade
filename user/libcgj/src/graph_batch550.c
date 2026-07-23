/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch550: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_550(void);
 *     — Returns the compile-time graph batch number for this TU (550).
 *   uint32_t gj_graph_milestone(void);
 *     — Returns the current graph milestone revision (550).
 *   uint32_t __gj_batch_id_550  (alias)
 *   uint32_t __gj_graph_milestone  (alias)
 *   __libcgj_batch550_marker = "libcgj-batch550"
 *
 * Does NOT define gj_batch_id — batch500 owns that (avoid multi-def).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch550_marker[] = "libcgj-batch550";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_550 — report this TU's graph batch number.
 *
 * Always returns 550. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_550(void)
{
	return 550u;
}

/*
 * gj_graph_milestone — report the current graph milestone revision.
 *
 * Always returns 550 (this milestone TU). Distinct from gj_batch_id
 * (batch500) so both can coexist at link time.
 */
uint32_t
gj_graph_milestone(void)
{
	return 550u;
}

uint32_t __gj_batch_id_550(void)
    __attribute__((alias("gj_batch_id_550")));

uint32_t __gj_graph_milestone(void)
    __attribute__((alias("gj_graph_milestone")));

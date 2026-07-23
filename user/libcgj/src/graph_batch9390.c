/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9390: container/oci soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9390(void);
 *     - Returns the compile-time graph batch number for this TU (9390).
 *   uint32_t __gj_batch_id_9390  (alias)
 *   __libcgj_batch9390_marker = "libcgj-batch9390"
 *
 * Exclusive continuum CREATE-ONLY (9381-9390: container/oci soft id
 * stubs — oci_config_ok_u_9381, oci_rootfs_ok_u_9382,
 * oci_runtime_ok_u_9383, oci_bundle_ok_u_9384, ctr_create_ok_u_9385,
 * ctr_start_ok_u_9386, ctr_stop_ok_u_9387, ctr_delete_ok_u_9388,
 * container_ready_u_9389, batch_id_9390). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9390_marker[] = "libcgj-batch9390";

#define B9390_BATCH_ID  9390u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9390_id(void)
{
	return B9390_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9390 - report this TU's graph batch number.
 *
 * Always returns 9390.
 */
uint32_t
gj_batch_id_9390(void)
{
	(void)NULL;
	return b9390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9390(void)
    __attribute__((alias("gj_batch_id_9390")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8660: epoll/eventfd id continuum batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8660(void);
 *     - Returns the compile-time graph batch number for this TU (8660).
 *   uint32_t __gj_batch_id_8660  (alias)
 *   __libcgj_batch8660_marker = "libcgj-batch8660"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique surfaces only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8660_marker[] = "libcgj-batch8660";

#define B8660_BATCH_ID  8660u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8660_id(void)
{
	return B8660_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8660 - report this TU's graph batch number.
 *
 * Always returns 8660. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8660(void)
{
	(void)NULL;
	return b8660_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8660(void)
    __attribute__((alias("gj_batch_id_8660")));

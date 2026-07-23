/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9748: md resync ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_md_resync_ok_u_9748(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       MD resync / recovery progress probe.
 *   uint32_t __gj_md_resync_ok_u_9748  (alias)
 *   __libcgj_batch9748_marker = "libcgj-batch9748"
 *
 * Exclusive continuum CREATE-ONLY (9741-9750: mdadm soft id
 * stubs — md_array_ok_u_9741, md_raid0_id_9742, md_raid1_id_9743,
 * md_raid5_id_9744, md_raid6_id_9745, md_assemble_ok_u_9746,
 * md_stop_ok_u_9747, md_resync_ok_u_9748, mdadm_ready_u_9749,
 * batch_id_9750). Unique surface only; no multi-def. No parent wires.
 * No __int128. No mdadm/MD implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9748_marker[] = "libcgj-batch9748";

/* Soft md-resync-ok lamp: always off (not a real resync probe). */
#define B9748_MD_RESYNC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9748_md_resync_ok(void)
{
	return B9748_MD_RESYNC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_md_resync_ok_u_9748 - MD resync ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not read
 * /proc/mdstat / sysfs resync state or call libc. No parent wires.
 */
uint32_t
gj_md_resync_ok_u_9748(void)
{
	(void)NULL;
	return b9748_md_resync_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_md_resync_ok_u_9748(void)
    __attribute__((alias("gj_md_resync_ok_u_9748")));

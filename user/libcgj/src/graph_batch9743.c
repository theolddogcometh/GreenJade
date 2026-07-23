/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9743: md RAID1 soft level id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_md_raid1_id_9743(void);
 *     - Returns 1 (soft RAID1 level catalog id). Pure-data product
 *       tag; not a live MD superblock / array layout probe.
 *   uint32_t __gj_md_raid1_id_9743  (alias)
 *   __libcgj_batch9743_marker = "libcgj-batch9743"
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

const char __libcgj_batch9743_marker[] = "libcgj-batch9743";

/* Soft RAID1 level catalog id (Linux MD level 1). */
#define B9743_MD_RAID1_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9743_md_raid1_id(void)
{
	return B9743_MD_RAID1_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_md_raid1_id_9743 - soft MD RAID1 level catalog id.
 *
 * Always returns 1 (RAID1 level tag). Soft pure-data only; does not
 * read MD superblocks or call libc. No parent wires.
 */
uint32_t
gj_md_raid1_id_9743(void)
{
	(void)NULL;
	return b9743_md_raid1_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_md_raid1_id_9743(void)
    __attribute__((alias("gj_md_raid1_id_9743")));

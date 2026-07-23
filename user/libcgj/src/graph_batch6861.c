/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6861: mount_setattr mount_attr layout sizes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_sizeof_6861(void);
 *     - Returns 32 (soft struct mount_attr size: 4 x u64 fields).
 *   uint32_t gj_mount_attr_field_count_6861(void);
 *     - Returns 4 (attr_set, attr_clr, propagation, userns_fd).
 *   uint32_t __gj_mount_attr_sizeof_6861  (alias)
 *   uint32_t __gj_mount_attr_field_count_6861  (alias)
 *   __libcgj_batch6861_marker = "libcgj-batch6861"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs —
 * sizeof/field_count_6861, basic_flags_6862, atime_flags_6863,
 * ext_flags_6864, attr_set_6865, attr_clr_6866, attr_apply_6867,
 * has_all/has_any_6868, propagation_6869, continuum + batch_id_6870).
 * Unique surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6861_marker[] = "libcgj-batch6861";

/* Soft mount_attr: attr_set, attr_clr, propagation, userns_fd (4 x u64). */
#define B6861_SIZEOF       32u
#define B6861_FIELD_COUNT  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6861_sizeof(void)
{
	return B6861_SIZEOF;
}

static uint32_t
b6861_field_count(void)
{
	return B6861_FIELD_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_sizeof_6861 - soft mount_attr byte size.
 *
 * Always returns 32. Soft pure-data layout tag for struct mount_attr
 * (4 x uint64 fields). Does not call mount_setattr. No parent wires.
 */
uint32_t
gj_mount_attr_sizeof_6861(void)
{
	(void)NULL;
	return b6861_sizeof();
}

/*
 * gj_mount_attr_field_count_6861 - soft mount_attr field count.
 *
 * Always returns 4 (attr_set / attr_clr / propagation / userns_fd).
 */
uint32_t
gj_mount_attr_field_count_6861(void)
{
	return b6861_field_count();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_mount_attr_sizeof_6861(void)
    __attribute__((alias("gj_mount_attr_sizeof_6861")));

uint32_t __gj_mount_attr_field_count_6861(void)
    __attribute__((alias("gj_mount_attr_field_count_6861")));

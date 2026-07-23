/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7031: sched_setattr sched_attr layout sizes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_attr_sizeof_ver0_7031(void);
 *     - Returns 48 (soft SCHED_ATTR_SIZE_VER0 base layout).
 *   uint32_t gj_sched_attr_sizeof_ver1_7031(void);
 *     - Returns 56 (soft VER1 with util_min/util_max).
 *   uint32_t gj_sched_attr_field_count_7031(void);
 *     - Returns 10 (soft core field count of struct sched_attr).
 *   uint32_t __gj_sched_attr_*_7031  (aliases)
 *   __libcgj_batch7031_marker = "libcgj-batch7031"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs —
 * sizeof/fields_7031, basic_policy_7032, ext_policy_7033, flags_7034,
 * flag_set_7035, flag_clr_7036, flag_apply_7037, has_all/has_any_7038,
 * nice_prio_clamp_7039, continuum + batch_id_7040). Unique surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7031_marker[] = "libcgj-batch7031";

/*
 * Soft sched_attr sizes (mirror linux/sched/types.h evolution):
 *   VER0 48 bytes (size..period, no util clamp)
 *   VER1 56 bytes (+ sched_util_min, sched_util_max)
 * Soft field count covers size, policy, flags, nice, priority, runtime,
 * deadline, period, util_min, util_max (10).
 */
#define B7031_SIZEOF_VER0  48u
#define B7031_SIZEOF_VER1  56u
#define B7031_FIELD_COUNT  10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7031_sizeof_ver0(void)
{
	return B7031_SIZEOF_VER0;
}

static uint32_t
b7031_sizeof_ver1(void)
{
	return B7031_SIZEOF_VER1;
}

static uint32_t
b7031_field_count(void)
{
	return B7031_FIELD_COUNT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_sizeof_ver0_7031 - soft SCHED_ATTR_SIZE_VER0 byte size.
 *
 * Always returns 48. Soft pure-data layout tag; does not call
 * sched_setattr. No parent wires.
 */
uint32_t
gj_sched_attr_sizeof_ver0_7031(void)
{
	(void)NULL;
	return b7031_sizeof_ver0();
}

/*
 * gj_sched_attr_sizeof_ver1_7031 - soft SCHED_ATTR_SIZE_VER1 byte size.
 *
 * Always returns 56 (base + util clamp pair).
 */
uint32_t
gj_sched_attr_sizeof_ver1_7031(void)
{
	return b7031_sizeof_ver1();
}

/*
 * gj_sched_attr_field_count_7031 - soft sched_attr field count.
 *
 * Always returns 10 (size/policy/flags/nice/priority/runtime/deadline/
 * period/util_min/util_max).
 */
uint32_t
gj_sched_attr_field_count_7031(void)
{
	return b7031_field_count();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sched_attr_sizeof_ver0_7031(void)
    __attribute__((alias("gj_sched_attr_sizeof_ver0_7031")));

uint32_t __gj_sched_attr_sizeof_ver1_7031(void)
    __attribute__((alias("gj_sched_attr_sizeof_ver1_7031")));

uint32_t __gj_sched_attr_field_count_7031(void)
    __attribute__((alias("gj_sched_attr_field_count_7031")));

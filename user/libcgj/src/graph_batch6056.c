/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6056: thermal trip type is hot stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_type_is_hot_6056(uint32_t type);
 *     - Return 1 if type equals the soft HOT trip type code (1),
 *       else 0. Pure-data type tag only; not a live ACPI thermal table
 *       parse.
 *   uint32_t __gj_trip_type_is_hot_6056  (alias)
 *   __libcgj_batch6056_marker = "libcgj-batch6056"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs). Unique gj_trip_type_is_hot_6056 surface only; no multi-def.
 * Soft type codes: CRITICAL=0, HOT=1, PASSIVE=2, ACTIVE=3.
 * Distinct from gj_trip_hot_c_6053 and gj_trip_type_is_critical_6055.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6056_marker[] = "libcgj-batch6056";

/* Soft trip type: HOT. */
#define B6056_TYPE_HOT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6056_is_hot(uint32_t uType)
{
	if (uType == B6056_TYPE_HOT) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_type_is_hot_6056 - 1 if type is HOT (1).
 *
 * type: soft thermal trip type code
 *
 * Returns 1 when type == 1, else 0. Integer-only. No parent wires.
 */
uint32_t
gj_trip_type_is_hot_6056(uint32_t uType)
{
	(void)NULL;
	return b6056_is_hot(uType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_type_is_hot_6056(uint32_t uType)
    __attribute__((alias("gj_trip_type_is_hot_6056")));

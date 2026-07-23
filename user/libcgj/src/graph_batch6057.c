/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6057: thermal trip type is passive stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_type_is_passive_6057(uint32_t type);
 *     - Return 1 if type equals the soft PASSIVE trip type code (2),
 *       else 0. Pure-data type tag only; not a live ACPI thermal table
 *       parse.
 *   uint32_t __gj_trip_type_is_passive_6057  (alias)
 *   __libcgj_batch6057_marker = "libcgj-batch6057"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs). Unique gj_trip_type_is_passive_6057 surface only; no
 * multi-def. Soft type codes: CRITICAL=0, HOT=1, PASSIVE=2, ACTIVE=3.
 * Distinct from gj_trip_passive_c_6051 and gj_trip_type_is_hot_6056.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6057_marker[] = "libcgj-batch6057";

/* Soft trip type: PASSIVE. */
#define B6057_TYPE_PASSIVE  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6057_is_passive(uint32_t uType)
{
	if (uType == B6057_TYPE_PASSIVE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_type_is_passive_6057 - 1 if type is PASSIVE (2).
 *
 * type: soft thermal trip type code
 *
 * Returns 1 when type == 2, else 0. Integer-only. No parent wires.
 */
uint32_t
gj_trip_type_is_passive_6057(uint32_t uType)
{
	(void)NULL;
	return b6057_is_passive(uType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_type_is_passive_6057(uint32_t uType)
    __attribute__((alias("gj_trip_type_is_passive_6057")));

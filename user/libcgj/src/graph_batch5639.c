/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5639: measured-boot event type ok predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_measured_boot_event_ok_5639(uint32_t ev_type,
 *                                           uint32_t ev_size);
 *     - Soft TCG event-log style gate: ev_type in [1, 0x80000000) and
 *       ev_size in [1, 4096]. Zero type/size or oversize → 0.
 *   uint32_t __gj_measured_boot_event_ok_5639  (alias)
 *   __libcgj_batch5639_marker = "libcgj-batch5639"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_measured_boot_event_ok_5639 surface only; no multi-def. Distinct
 * from gj_measured_boot_extend_5638 (batch5638). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5639_marker[] = "libcgj-batch5639";

/* Soft upper bound for a single measured event payload (bytes). */
#define B5639_EV_SIZE_MAX   4096u
/* Types with high bit set are soft-rejected (vendor/reserved zone). */
#define B5639_EV_TYPE_HIBIT 0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5639_event_ok(uint32_t u32Type, uint32_t u32Size)
{
	if (u32Type == 0u) {
		return 0u;
	}
	if ((u32Type & B5639_EV_TYPE_HIBIT) != 0u) {
		return 0u;
	}
	if (u32Size == 0u) {
		return 0u;
	}
	if (u32Size > B5639_EV_SIZE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_measured_boot_event_ok_5639 - validate soft event-log record bounds.
 *
 * ev_type: event type code (1..0x7fffffff accepted)
 * ev_size: payload length in bytes (1..4096 accepted)
 *
 * Returns 1 when both fields are in range, else 0. No I/O.
 * No parent wires.
 */
uint32_t
gj_measured_boot_event_ok_5639(uint32_t u32EvType, uint32_t u32EvSize)
{
	(void)NULL;
	return b5639_event_ok(u32EvType, u32EvSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_measured_boot_event_ok_5639(uint32_t u32EvType,
    uint32_t u32EvSize)
    __attribute__((alias("gj_measured_boot_event_ok_5639")));

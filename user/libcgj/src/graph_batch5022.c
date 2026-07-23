/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5022: scheduler prio to weight (table-less).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_prio_to_weight_u(uint32_t prio);
 *     - Map a soft scheduler priority index (0..39, nice+20 style) to a
 *       relative weight via a table-less linear formula. prio is clamped
 *       into 0..39. Nice-0 (prio 20) yields weight 1024.
 *   uint32_t __gj_sched_prio_to_weight_u  (alias)
 *   __libcgj_batch5022_marker = "libcgj-batch5022"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Unique
 * gj_sched_prio_to_weight_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5022_marker[] = "libcgj-batch5022";

/* Soft prio range and nice-0 weight unit. */
#define B5022_PRIO_MAX     39u
#define B5022_NICE0_PRIO   20u
#define B5022_NICE0_WEIGHT 1024u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5022_prio_to_weight(uint32_t u32Prio)
{
	uint32_t u32P;
	uint32_t u32Num;
	uint32_t u32W;

	/* Clamp prio into the soft 0..39 band. */
	u32P = (u32Prio > B5022_PRIO_MAX) ? B5022_PRIO_MAX : u32Prio;

	/*
	 * Table-less linear weight:
	 *   weight = (40 - prio) * NICE0_WEIGHT / 20
	 * prio 0  -> 2048; prio 20 -> 1024; prio 39 -> 51.
	 */
	u32Num = (B5022_PRIO_MAX + 1u) - u32P; /* 40..1 */
	u32W = (u32Num * B5022_NICE0_WEIGHT) / B5022_NICE0_PRIO;
	if (u32W == 0u) {
		return 1u;
	}
	return u32W;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_prio_to_weight_u - map soft prio 0..39 to a relative weight.
 *
 * prio: soft priority index (nice + 20 style); values above 39 clamp.
 *
 * Returns a positive weight. Nice-0 (prio 20) returns 1024. Pure data;
 * no CFS weight table, no parent wires.
 */
uint32_t
gj_sched_prio_to_weight_u(uint32_t u32Prio)
{
	(void)NULL;
	return b5022_prio_to_weight(u32Prio);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_prio_to_weight_u(uint32_t u32Prio)
    __attribute__((alias("gj_sched_prio_to_weight_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9840: smbios soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9840(void);
 *     - Returns the compile-time graph batch number for this TU (9840).
 *   uint32_t __gj_batch_id_9840  (alias)
 *   __libcgj_batch9840_marker = "libcgj-batch9840"
 *
 * Exclusive continuum CREATE-ONLY (9831-9840: smbios soft id stubs —
 * entry_ok_u_9831, bios_ok_u_9832, system_ok_u_9833,
 * baseboard_ok_u_9834, chassis_ok_u_9835, cpu_ok_u_9836, mem_ok_u_9837,
 * parse_ok_u_9838, ready_u_9839, batch_id_9840). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9840_marker[] = "libcgj-batch9840";

#define B9840_BATCH_ID  9840u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9840_id(void)
{
	return B9840_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9840 - report this TU's graph batch number.
 *
 * Always returns 9840.
 */
uint32_t
gj_batch_id_9840(void)
{
	(void)NULL;
	return b9840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9840(void)
    __attribute__((alias("gj_batch_id_9840")));

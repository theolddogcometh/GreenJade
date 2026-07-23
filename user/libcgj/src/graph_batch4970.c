/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4970: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4970(void);
 *     - Returns the compile-time graph batch number for this TU (4970).
 *   uint32_t __gj_batch_id_4970  (alias)
 *   __libcgj_batch4970_marker = "libcgj-batch4970"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4970_marker[] = "libcgj-batch4970";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4970_id(void)
{
	return 4970u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4970 - report this TU's graph batch number.
 *
 * Always returns 4970. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4970(void)
{
	(void)NULL;
	return b4970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4970(void)
    __attribute__((alias("gj_batch_id_4970")));

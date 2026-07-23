/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2980: milestone 2980 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2980(void);
 *     - Returns the compile-time graph batch number for this TU (2980).
 *   uint32_t __gj_batch_id_2980  (alias)
 *   __libcgj_batch2980_marker = "libcgj-batch2980"
 *
 * Milestone for the UUID/ELF/PE exclusive continuum wave
 * (batches 2971-2979: uuid_nil_p_u, uuid_v4_version_p_u,
 * uuid_variant_ok_u, elf_is_elf_u, elf_is_64_u, elf_is_le_u,
 * elf_machine_u, elf_type_u, pe_is_mz_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2980_marker[] = "libcgj-batch2980";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2980_id(void)
{
	return 2980u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2980 - report this TU's graph batch number.
 *
 * Always returns 2980. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2980(void)
{
	(void)NULL;
	return b2980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2980(void)
    __attribute__((alias("gj_batch_id_2980")));

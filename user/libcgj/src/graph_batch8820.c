/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8820: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8820(void);
 *     - Returns the compile-time graph batch number for this TU (8820).
 *   uint32_t __gj_batch_id_8820  (alias)
 *   __libcgj_batch8820_marker = "libcgj-batch8820"
 *
 * Exclusive continuum CREATE-ONLY (8811-8820: bpf/map id stubs —
 * prog_type_socket_id, prog_type_kprobe_id, map_hash_id, map_array_id,
 * map_lru_id, cmd_map_create_id, cmd_prog_load_id, bpf_ready_u,
 * bpf_fd_ok_u, batch_id_8820). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Soft pure-data catalog only; no bpf syscalls. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8820_marker[] = "libcgj-batch8820";

#define B8820_BATCH_ID  8820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8820_id(void)
{
	return B8820_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8820 - report this TU's graph batch number.
 *
 * Always returns 8820. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8820(void)
{
	(void)NULL;
	return b8820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8820(void)
    __attribute__((alias("gj_batch_id_8820")));

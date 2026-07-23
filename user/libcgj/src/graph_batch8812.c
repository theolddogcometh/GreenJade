/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8812: bpf BPF_PROG_TYPE_KPROBE id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_prog_type_kprobe_id_8812(void);
 *     - Return soft BPF_PROG_TYPE_KPROBE prog type id (2).
 *   uint32_t __gj_bpf_prog_type_kprobe_id_8812  (alias)
 *   __libcgj_batch8812_marker = "libcgj-batch8812"
 *
 * Exclusive continuum CREATE-ONLY (8811-8820: bpf/map id stubs —
 * prog_type_socket_id, prog_type_kprobe_id, map_hash_id, map_array_id,
 * map_lru_id, cmd_map_create_id, cmd_prog_load_id, bpf_ready_u,
 * bpf_fd_ok_u, batch_id_8820). Unique gj_bpf_prog_type_kprobe_id_8812
 * surface only; no multi-def. Catalog id only; not a live bpf syscall.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8812_marker[] = "libcgj-batch8812";

/* Soft: BPF_PROG_TYPE_KPROBE. */
#define B8812_BPF_PROG_TYPE_KPROBE  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8812_prog_type_kprobe_id(void)
{
	return B8812_BPF_PROG_TYPE_KPROBE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_prog_type_kprobe_id_8812 - soft KPROBE prog type id.
 *
 * Always returns 2. Pure-data catalog stub; does not issue bpf(2) or
 * attach kprobes. No parent wires.
 */
uint32_t
gj_bpf_prog_type_kprobe_id_8812(void)
{
	(void)NULL;
	return b8812_prog_type_kprobe_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_prog_type_kprobe_id_8812(void)
    __attribute__((alias("gj_bpf_prog_type_kprobe_id_8812")));

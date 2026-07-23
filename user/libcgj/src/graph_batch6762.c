/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6762: bpf BPF_MAP_LOOKUP_ELEM cmd stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_cmd_map_lookup_elem_6762(void);
 *     - Returns 1 (soft BPF_MAP_LOOKUP_ELEM bpf(2) command id).
 *       Pure-data catalog stub; not a live bpf syscall.
 *   uint32_t __gj_bpf_cmd_map_lookup_elem_6762  (alias)
 *   __libcgj_batch6762_marker = "libcgj-batch6762"
 *
 * Exclusive continuum CREATE-ONLY (6761-6770: bpf cmd stubs). Unique
 * gj_bpf_cmd_map_lookup_elem_6762 surface only; no multi-def.
 * Distinct from map_create (batch6761) and map_update_elem (batch6763).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6762_marker[] = "libcgj-batch6762";

/* Soft BPF_MAP_LOOKUP_ELEM (lookup key in map). */
#define B6762_BPF_MAP_LOOKUP_ELEM  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6762_map_lookup_elem(void)
{
	return B6762_BPF_MAP_LOOKUP_ELEM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_cmd_map_lookup_elem_6762 - soft BPF_MAP_LOOKUP_ELEM cmd id.
 *
 * Always returns 1. Pure-data stub for product bpf-cmd catalogs;
 * does not issue bpf(2) or touch map fds. No parent wires.
 */
uint32_t
gj_bpf_cmd_map_lookup_elem_6762(void)
{
	(void)NULL;
	return b6762_map_lookup_elem();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_cmd_map_lookup_elem_6762(void)
    __attribute__((alias("gj_bpf_cmd_map_lookup_elem_6762")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8819: bpf fd soft validity (unsigned form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bpf_fd_ok_u_8819(uint32_t fd);
 *     - Return 1 if fd is not the soft invalid sentinel (UINT32_MAX),
 *       else 0. Unsigned form of a map/prog fd validity gate.
 *   uint32_t __gj_bpf_fd_ok_u_8819  (alias)
 *   __libcgj_batch8819_marker = "libcgj-batch8819"
 *
 * Exclusive continuum CREATE-ONLY (8811-8820: bpf/map id stubs —
 * prog_type_socket_id, prog_type_kprobe_id, map_hash_id, map_array_id,
 * map_lru_id, cmd_map_create_id, cmd_prog_load_id, bpf_ready_u,
 * bpf_fd_ok_u, batch_id_8820). Unique gj_bpf_fd_ok_u_8819 surface only;
 * no multi-def. Soft pure-data gate only; does not call fcntl or
 * bpf(2). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8819_marker[] = "libcgj-batch8819";

/* Soft invalid-fd sentinel (unsigned form of -1). */
#define B8819_FD_INVALID  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8819_fd_ok_u(uint32_t uFd)
{
	return (uFd != B8819_FD_INVALID) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bpf_fd_ok_u_8819 - soft non-invalid bpf map/prog fd check.
 *
 * fd: soft file-descriptor as unsigned (map or prog candidate)
 *
 * Returns 1 when fd is not UINT32_MAX, else 0. Pure-data gate; does
 * not call fstat, close, or bpf(2). No parent wires.
 */
uint32_t
gj_bpf_fd_ok_u_8819(uint32_t uFd)
{
	(void)NULL;
	return b8819_fd_ok_u(uFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bpf_fd_ok_u_8819(uint32_t uFd)
    __attribute__((alias("gj_bpf_fd_ok_u_8819")));

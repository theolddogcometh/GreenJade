/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7077: ptrace PTRACE_ATTACH request stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ptrace_req_attach_u_7077(uint32_t request);
 *     - Return 1 if request is PTRACE_ATTACH (16), else 0.
 *   uint32_t __gj_ptrace_req_attach_u_7077  (alias)
 *   __libcgj_batch7077_marker = "libcgj-batch7077"
 *
 * Exclusive continuum CREATE-ONLY (7071-7080: ptrace request stubs —
 * traceme, peektext, peekdata, poketext, pokedata, cont, attach,
 * detach, req_ok, batch_id_7080). Unique
 * gj_ptrace_req_attach_u_7077 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7077_marker[] = "libcgj-batch7077";

/* PTRACE_ATTACH: attach to a running process (value 16). */
#define B7077_PTRACE_ATTACH ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7077_ptrace_req_attach(uint32_t u32Req)
{
	return (u32Req == B7077_PTRACE_ATTACH) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptrace_req_attach_u_7077 - 1 if request is PTRACE_ATTACH.
 *
 * request: ptrace(2) request argument
 *
 * Returns 1 when request equals PTRACE_ATTACH (16), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_ptrace_req_attach_u_7077(uint32_t u32Request)
{
	(void)NULL;
	return b7077_ptrace_req_attach(u32Request);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ptrace_req_attach_u_7077(uint32_t u32Request)
    __attribute__((alias("gj_ptrace_req_attach_u_7077")));

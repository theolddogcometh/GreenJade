/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7071: ptrace PTRACE_TRACEME request stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ptrace_req_traceme_u_7071(uint32_t request);
 *     - Return 1 if request is PTRACE_TRACEME (0), else 0.
 *   uint32_t __gj_ptrace_req_traceme_u_7071  (alias)
 *   __libcgj_batch7071_marker = "libcgj-batch7071"
 *
 * Exclusive continuum CREATE-ONLY (7071-7080: ptrace request stubs —
 * traceme, peektext, peekdata, poketext, pokedata, cont, attach,
 * detach, req_ok, batch_id_7080). Unique
 * gj_ptrace_req_traceme_u_7071 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7071_marker[] = "libcgj-batch7071";

/* PTRACE_TRACEME: this process is traced by its parent (value 0). */
#define B7071_PTRACE_TRACEME ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7071_ptrace_req_traceme(uint32_t u32Req)
{
	return (u32Req == B7071_PTRACE_TRACEME) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptrace_req_traceme_u_7071 - 1 if request is PTRACE_TRACEME.
 *
 * request: ptrace(2) request argument
 *
 * Returns 1 when request equals PTRACE_TRACEME (0), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_ptrace_req_traceme_u_7071(uint32_t u32Request)
{
	(void)NULL;
	return b7071_ptrace_req_traceme(u32Request);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ptrace_req_traceme_u_7071(uint32_t u32Request)
    __attribute__((alias("gj_ptrace_req_traceme_u_7071")));

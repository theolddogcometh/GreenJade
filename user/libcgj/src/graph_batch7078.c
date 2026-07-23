/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7078: ptrace PTRACE_DETACH request stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ptrace_req_detach_u_7078(uint32_t request);
 *     - Return 1 if request is PTRACE_DETACH (17), else 0.
 *   uint32_t __gj_ptrace_req_detach_u_7078  (alias)
 *   __libcgj_batch7078_marker = "libcgj-batch7078"
 *
 * Exclusive continuum CREATE-ONLY (7071-7080: ptrace request stubs —
 * traceme, peektext, peekdata, poketext, pokedata, cont, attach,
 * detach, req_ok, batch_id_7080). Unique
 * gj_ptrace_req_detach_u_7078 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7078_marker[] = "libcgj-batch7078";

/* PTRACE_DETACH: detach from a traced process (value 17). */
#define B7078_PTRACE_DETACH ((uint32_t)17u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7078_ptrace_req_detach(uint32_t u32Req)
{
	return (u32Req == B7078_PTRACE_DETACH) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptrace_req_detach_u_7078 - 1 if request is PTRACE_DETACH.
 *
 * request: ptrace(2) request argument
 *
 * Returns 1 when request equals PTRACE_DETACH (17), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_ptrace_req_detach_u_7078(uint32_t u32Request)
{
	(void)NULL;
	return b7078_ptrace_req_detach(u32Request);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ptrace_req_detach_u_7078(uint32_t u32Request)
    __attribute__((alias("gj_ptrace_req_detach_u_7078")));

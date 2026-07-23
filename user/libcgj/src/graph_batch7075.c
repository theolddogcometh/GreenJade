/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7075: ptrace PTRACE_POKEDATA request stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ptrace_req_pokedata_u_7075(uint32_t request);
 *     - Return 1 if request is PTRACE_POKEDATA (5), else 0.
 *   uint32_t __gj_ptrace_req_pokedata_u_7075  (alias)
 *   __libcgj_batch7075_marker = "libcgj-batch7075"
 *
 * Exclusive continuum CREATE-ONLY (7071-7080: ptrace request stubs —
 * traceme, peektext, peekdata, poketext, pokedata, cont, attach,
 * detach, req_ok, batch_id_7080). Unique
 * gj_ptrace_req_pokedata_u_7075 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7075_marker[] = "libcgj-batch7075";

/* PTRACE_POKEDATA: write a word into the tracee's data (value 5). */
#define B7075_PTRACE_POKEDATA ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7075_ptrace_req_pokedata(uint32_t u32Req)
{
	return (u32Req == B7075_PTRACE_POKEDATA) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptrace_req_pokedata_u_7075 - 1 if request is PTRACE_POKEDATA.
 *
 * request: ptrace(2) request argument
 *
 * Returns 1 when request equals PTRACE_POKEDATA (5), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_ptrace_req_pokedata_u_7075(uint32_t u32Request)
{
	(void)NULL;
	return b7075_ptrace_req_pokedata(u32Request);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ptrace_req_pokedata_u_7075(uint32_t u32Request)
    __attribute__((alias("gj_ptrace_req_pokedata_u_7075")));

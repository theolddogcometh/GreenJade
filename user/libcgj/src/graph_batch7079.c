/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7079: ptrace request validity soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ptrace_req_ok_u_7079(uint32_t request);
 *     - Return 1 if request is a soft-known classic ptrace request
 *       covered by this wave (0,1,2,4,5,7,16,17), else 0.
 *   uint32_t __gj_ptrace_req_ok_u_7079  (alias)
 *   __libcgj_batch7079_marker = "libcgj-batch7079"
 *
 * Exclusive continuum CREATE-ONLY (7071-7080: ptrace request stubs —
 * traceme, peektext, peekdata, poketext, pokedata, cont, attach,
 * detach, req_ok, batch_id_7080). Unique gj_ptrace_req_ok_u_7079
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7079_marker[] = "libcgj-batch7079";

/* Soft-known classic requests covered by this exclusive wave. */
#define B7079_PTRACE_TRACEME  ((uint32_t)0u)
#define B7079_PTRACE_PEEKTEXT ((uint32_t)1u)
#define B7079_PTRACE_PEEKDATA ((uint32_t)2u)
#define B7079_PTRACE_POKETEXT ((uint32_t)4u)
#define B7079_PTRACE_POKEDATA ((uint32_t)5u)
#define B7079_PTRACE_CONT     ((uint32_t)7u)
#define B7079_PTRACE_ATTACH   ((uint32_t)16u)
#define B7079_PTRACE_DETACH   ((uint32_t)17u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7079_ptrace_req_ok(uint32_t u32Req)
{
	if (u32Req == B7079_PTRACE_TRACEME ||
	    u32Req == B7079_PTRACE_PEEKTEXT ||
	    u32Req == B7079_PTRACE_PEEKDATA ||
	    u32Req == B7079_PTRACE_POKETEXT ||
	    u32Req == B7079_PTRACE_POKEDATA ||
	    u32Req == B7079_PTRACE_CONT ||
	    u32Req == B7079_PTRACE_ATTACH ||
	    u32Req == B7079_PTRACE_DETACH) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptrace_req_ok_u_7079 - 1 if request is a soft-known classic request.
 *
 * request: ptrace(2) request argument
 *
 * Returns 1 for TRACEME/PEEKTEXT/PEEKDATA/POKETEXT/POKEDATA/CONT/
 * ATTACH/DETACH (0,1,2,4,5,7,16,17), else 0.
 * Soft catalog check; no parent wires.
 */
uint32_t
gj_ptrace_req_ok_u_7079(uint32_t u32Request)
{
	(void)NULL;
	return b7079_ptrace_req_ok(u32Request);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ptrace_req_ok_u_7079(uint32_t u32Request)
    __attribute__((alias("gj_ptrace_req_ok_u_7079")));

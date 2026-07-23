/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8167: strerror known-errno validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strerror_known_ok_i_8167(uint32_t err);
 *     - Return 1 if err is in the soft strerror catalog set, else 0.
 *   uint32_t __gj_strerror_known_ok_i_8167  (alias)
 *   __libcgj_batch8167_marker = "libcgj-batch8167"
 *
 * Exclusive continuum CREATE-ONLY (8161-8170: strerror errno stubs —
 * success, eperm, enoent, einval, enomem, enosys, known_ok, table_n,
 * r_min_buf, batch_id_8170).
 * Unique gj_strerror_known_ok_i_8167 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8167_marker[] = "libcgj-batch8167";

/*
 * Soft Linux x86_64 codes matching freestanding strerror catalog
 * (Success + common errno rows). Local constants only; no errno.h.
 */
#define B8167_SUCCESS   ((uint32_t)0u)
#define B8167_EPERM     ((uint32_t)1u)
#define B8167_ENOENT    ((uint32_t)2u)
#define B8167_EIO       ((uint32_t)5u)
#define B8167_EAGAIN    ((uint32_t)11u)
#define B8167_ENOMEM    ((uint32_t)12u)
#define B8167_EACCES    ((uint32_t)13u)
#define B8167_EFAULT    ((uint32_t)14u)
#define B8167_EBUSY     ((uint32_t)16u)
#define B8167_EEXIST    ((uint32_t)17u)
#define B8167_ENOTDIR   ((uint32_t)20u)
#define B8167_EISDIR    ((uint32_t)21u)
#define B8167_EINVAL    ((uint32_t)22u)
#define B8167_ENOSPC    ((uint32_t)28u)
#define B8167_EPIPE     ((uint32_t)32u)
#define B8167_ERANGE    ((uint32_t)34u)
#define B8167_ENOSYS    ((uint32_t)38u)
#define B8167_ETIMEDOUT ((uint32_t)110u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8167_known_ok(uint32_t u32Err)
{
	if (u32Err == B8167_SUCCESS)
		return 1u;
	if (u32Err == B8167_EPERM)
		return 1u;
	if (u32Err == B8167_ENOENT)
		return 1u;
	if (u32Err == B8167_EIO)
		return 1u;
	if (u32Err == B8167_EAGAIN)
		return 1u;
	if (u32Err == B8167_ENOMEM)
		return 1u;
	if (u32Err == B8167_EACCES)
		return 1u;
	if (u32Err == B8167_EFAULT)
		return 1u;
	if (u32Err == B8167_EBUSY)
		return 1u;
	if (u32Err == B8167_EEXIST)
		return 1u;
	if (u32Err == B8167_ENOTDIR)
		return 1u;
	if (u32Err == B8167_EISDIR)
		return 1u;
	if (u32Err == B8167_EINVAL)
		return 1u;
	if (u32Err == B8167_ENOSPC)
		return 1u;
	if (u32Err == B8167_EPIPE)
		return 1u;
	if (u32Err == B8167_ERANGE)
		return 1u;
	if (u32Err == B8167_ENOSYS)
		return 1u;
	if (u32Err == B8167_ETIMEDOUT)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strerror_known_ok_i_8167 - 1 if err has a soft named strerror row.
 *
 * err: soft errno-style code
 *
 * Returns 1 for the freestanding catalog set, else 0 (unknown-error
 * path). Does not call strerror(3). No parent wires.
 */
uint32_t
gj_strerror_known_ok_i_8167(uint32_t u32Err)
{
	(void)NULL;
	return b8167_known_ok(u32Err);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strerror_known_ok_i_8167(uint32_t u32Err)
    __attribute__((alias("gj_strerror_known_ok_i_8167")));

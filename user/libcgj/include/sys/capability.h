/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux capability header shape (subset). Not GNU glibc / libcap.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/capability.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _LINUX_CAPABILITY_VERSION_1 0x19980330u
#define _LINUX_CAPABILITY_U32S_1    1
#define _LINUX_CAPABILITY_VERSION_2 0x20071026u
#define _LINUX_CAPABILITY_U32S_2    2
#define _LINUX_CAPABILITY_VERSION_3 0x20080522u
#define _LINUX_CAPABILITY_U32S_3    2

typedef struct __user_cap_header_struct {
    uint32_t version;
    int      pid;
} *cap_user_header_t;

typedef struct __user_cap_data_struct {
    uint32_t effective;
    uint32_t permitted;
    uint32_t inheritable;
} *cap_user_data_t;

int capget(cap_user_header_t pHdr, cap_user_data_t pData);
int capset(cap_user_header_t pHdr, const cap_user_data_t pData);

#ifdef __cplusplus
}
#endif

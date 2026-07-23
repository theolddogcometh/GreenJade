/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux capability header shape (subset). Not GNU glibc / libcap.
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

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room sys/quota.h (quotactl graph node).
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef caddr_t
typedef char *caddr_t;
#endif

int quotactl(int nCmd, const char *szSpecial, int nId, caddr_t pAddr);

#ifdef __cplusplus
}
#endif

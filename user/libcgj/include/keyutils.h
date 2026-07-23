/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room keyutils-shaped keyctl.h (subset). Not GNU glibc.
 */
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
long add_key(const char *szType, const char *szDesc, const void *pPayload,
             size_t cb, int nKeyring);
long request_key(const char *szType, const char *szDesc, const char *szCallout,
                 int nDestKeyring);
long keyctl(int nCmd, unsigned long a1, unsigned long a2, unsigned long a3,
            unsigned long a4);
#ifdef __cplusplus
}
#endif

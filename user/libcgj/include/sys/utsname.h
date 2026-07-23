/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
};

int uname(struct utsname *pBuf);

#ifdef __cplusplus
}
#endif

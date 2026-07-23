/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ttyent.h. Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct ttyent {
    char *ty_name;
    char *ty_getty;
    char *ty_type;
    int   ty_status;
    char *ty_window;
    char *ty_comment;
};

#define TTY_ON     0x01
#define TTY_SECURE 0x02

struct ttyent *getttyent(void);
struct ttyent *getttynam(const char *szName);
int            setttyent(void);
int            endttyent(void);
int            ttyslot(void);

#ifdef __cplusplus
}
#endif

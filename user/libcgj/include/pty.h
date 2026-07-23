/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped pty.h (stubs). Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* termios/winsize optional; pass NULL or opaque until termios lands */
struct termios;
struct winsize;

int openpty(int *pAmaster, int *pAslave, char *szName,
            const struct termios *pTermp, const struct winsize *pWinp);
int forkpty(int *pAmaster, char *szName, const struct termios *pTermp,
            const struct winsize *pWinp);
int login_tty(int nFd);

#ifdef __cplusplus
}
#endif

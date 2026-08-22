/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/ioctl.h> for OpenSSH-portable (TIOCGWINSZ / struct winsize
 * in channels.c). Host POSIX: glibc. DUT: libcgj ioctl is the syscall only.
 */
#pragma once
#include_next <sys/ioctl.h>

#ifndef TIOCGWINSZ
# define TIOCGWINSZ 0x5413
#endif
#ifndef TIOCSWINSZ
# define TIOCSWINSZ 0x5414
#endif
#ifndef TIOCSCTTY
# define TIOCSCTTY 0x540E
#endif
#ifndef TIOCNOTTY
# define TIOCNOTTY 0x5422
#endif

#if !defined(_GJ_WINSIZE_DEFINED) && !defined(_GJ_OPENSSH_WINSIZE) && \
    !defined(_TERMIOS_H)
# define _GJ_OPENSSH_WINSIZE
# define _GJ_WINSIZE_DEFINED
struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
};
#endif

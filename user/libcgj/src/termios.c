/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * termios soft fill via ioctl TCGETS/TCSETS (Linux x86_64).
 * cf* helpers validate speeds; tcgetpgrp/tcsetpgrp/tcgetsid live in pgrp.c.
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/* Linux ioctl request numbers (x86_64) */
#ifndef TCGETS
#define TCGETS 0x5401
#endif
#ifndef TCSETS
#define TCSETS 0x5402
#endif
#ifndef TCSETSW
#define TCSETSW 0x5403
#endif
#ifndef TCSETSF
#define TCSETSF 0x5404
#endif
#ifndef TCSBRK
#define TCSBRK 0x5409
#endif
#ifndef TCXONC
#define TCXONC 0x540A
#endif
#ifndef TCFLSH
#define TCFLSH 0x540B
#endif

/* Known B* baud values from termios.h (soft validation table). */
static const speed_t s_aValidSpeeds[] = {
    B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400,
    B4800, B9600, B19200, B38400, B57600, B115200
};

static int
speed_ok(speed_t speed)
{
    size_t i;

    for (i = 0; i < sizeof(s_aValidSpeeds) / sizeof(s_aValidSpeeds[0]); i++) {
        if (s_aValidSpeeds[i] == speed) {
            return 1;
        }
    }
    /* Accept raw integer baud rates some apps pass through cfsetspeed. */
    if (speed >= 50u && speed <= 4000000u) {
        return 1;
    }
    return 0;
}

int
tcgetattr(int nFd, struct termios *pT)
{
    if (pT == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    return ioctl(nFd, TCGETS, pT);
}

int
tcsetattr(int nFd, int nOptionalActions, const struct termios *pT)
{
    unsigned long req;

    if (pT == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    switch (nOptionalActions) {
    case TCSANOW:
        req = TCSETS;
        break;
    case TCSADRAIN:
        req = TCSETSW;
        break;
    case TCSAFLUSH:
        req = TCSETSF;
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    return ioctl(nFd, req, (void *)(uintptr_t)pT);
}

int
tcsendbreak(int nFd, int nDuration)
{
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    /* Linux ignores nDuration for zero (0.25s break); pass through. */
    return ioctl(nFd, TCSBRK, nDuration);
}

int
tcdrain(int nFd)
{
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    /* Linux: TCSBRK with arg 1 drains output (not a real break). */
    return ioctl(nFd, TCSBRK, 1);
}

int
tcflush(int nFd, int nQueueSelector)
{
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (nQueueSelector != TCIFLUSH && nQueueSelector != TCOFLUSH &&
        nQueueSelector != TCIOFLUSH) {
        errno = EINVAL;
        return -1;
    }
    return ioctl(nFd, TCFLSH, nQueueSelector);
}

int
tcflow(int nFd, int nAction)
{
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (nAction != TCOOFF && nAction != TCOON && nAction != TCIOFF &&
        nAction != TCION) {
        errno = EINVAL;
        return -1;
    }
    return ioctl(nFd, TCXONC, nAction);
}

speed_t
cfgetispeed(const struct termios *pT)
{
    if (pT == NULL) {
        return 0;
    }
    return pT->c_ispeed;
}

speed_t
cfgetospeed(const struct termios *pT)
{
    if (pT == NULL) {
        return 0;
    }
    return pT->c_ospeed;
}

int
cfsetispeed(struct termios *pT, speed_t speed)
{
    if (pT == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (!speed_ok(speed) && speed != 0) {
        errno = EINVAL;
        return -1;
    }
    pT->c_ispeed = speed;
    return 0;
}

int
cfsetospeed(struct termios *pT, speed_t speed)
{
    if (pT == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (!speed_ok(speed) && speed != 0) {
        errno = EINVAL;
        return -1;
    }
    pT->c_ospeed = speed;
    return 0;
}

int
cfsetspeed(struct termios *pT, speed_t speed)
{
    if (cfsetispeed(pT, speed) != 0) {
        return -1;
    }
    return cfsetospeed(pT, speed);
}

void
cfmakeraw(struct termios *pT)
{
    if (pT == NULL) {
        return;
    }
    pT->c_iflag &= (tcflag_t)~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR |
                                IGNCR | ICRNL | IXON);
    pT->c_oflag &= (tcflag_t)~OPOST;
    pT->c_lflag &= (tcflag_t)~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    pT->c_cflag &= (tcflag_t)~(CSIZE | PARENB);
    pT->c_cflag |= CS8;
    pT->c_cc[VMIN] = 1;
    pT->c_cc[VTIME] = 0;
}

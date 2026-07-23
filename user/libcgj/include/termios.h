/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <termios.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * tcgetattr/tcsetattr, cfsetispeed/cfsetospeed, and termios flag constants
 * for TTYs (shells, sshd, getpass). Typically backed by TCGETS/TCSETS ioctl.
 *
 * Design notes
 * ------------
 * Flag bit values match Linux. Bring-up may soft-fill when no real tty is
 * attached (pipes return ENOTTY).
 *
 * Non-goals
 * ---------
 * Full hardware flow-control matrix for every UART.
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char  cc_t;
typedef unsigned int   speed_t;
typedef unsigned int   tcflag_t;

#define NCCS 32

struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t     c_line;
    cc_t     c_cc[NCCS];
    speed_t  c_ispeed;
    speed_t  c_ospeed;
};

/* c_cc indices */
#define VINTR  0
#define VQUIT  1
#define VERASE 2
#define VKILL  3
#define VEOF   4
#define VTIME  5
#define VMIN   6
#define VSWTC  7
#define VSTART 8
#define VSTOP  9
#define VSUSP  10
#define VEOL   11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

/* iflag */
#define IGNBRK 0000001
#define BRKINT 0000002
#define IGNPAR 0000004
#define PARMRK 0000010
#define INPCK  0000020
#define ISTRIP 0000040
#define INLCR  0000100
#define IGNCR  0000200
#define ICRNL  0000400
#define IUCLC  0001000
#define IXON   0002000
#define IXANY  0004000
#define IXOFF  0010000

/* oflag */
#define OPOST  0000001
#define ONLCR  0000004

/* cflag */
#define CSIZE  0000060
#define CS5    0000000
#define CS6    0000020
#define CS7    0000040
#define CS8    0000060
#define CSTOPB 0000100
#define CREAD  0000200
#define PARENB 0000400
#define PARODD 0001000
#define HUPCL  0002000
#define CLOCAL 0004000
#define B0     0000000
#define B50    0000001
#define B75    0000002
#define B110   0000003
#define B134   0000004
#define B150   0000005
#define B200   0000006
#define B300   0000007
#define B600   0000010
#define B1200  0000011
#define B1800  0000012
#define B2400  0000013
#define B4800  0000014
#define B9600  0000015
#define B19200 0000016
#define B38400 0000017
#define B57600 0010001
#define B115200 0010002

/* lflag */
#define ISIG   0000001
#define ICANON 0000002
#define ECHO   0000010
#define ECHOE  0000020
#define ECHOK  0000040
#define ECHONL 0000100
#define NOFLSH 0000200
#define TOSTOP 0000400
#define IEXTEN 0100000

/* tcsetattr optional_actions */
#define TCSANOW   0
#define TCSADRAIN 1
#define TCSAFLUSH 2

/* tcflush queue_selector */
#define TCIFLUSH  0
#define TCOFLUSH  1
#define TCIOFLUSH 2

/* tcflow action */
#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

int     tcgetattr(int nFd, struct termios *pT);
int     tcsetattr(int nFd, int nOptionalActions, const struct termios *pT);
int     tcsendbreak(int nFd, int nDuration);
int     tcdrain(int nFd);
int     tcflush(int nFd, int nQueueSelector);
int     tcflow(int nFd, int nAction);
pid_t   tcgetpgrp(int nFd);
int     tcsetpgrp(int nFd, pid_t pgrp);
pid_t   tcgetsid(int nFd);
speed_t cfgetispeed(const struct termios *pT);
speed_t cfgetospeed(const struct termios *pT);
int     cfsetispeed(struct termios *pT, speed_t speed);
int     cfsetospeed(struct termios *pT, speed_t speed);
int     cfsetspeed(struct termios *pT, speed_t speed);
void    cfmakeraw(struct termios *pT);

#ifdef __cplusplus
}
#endif

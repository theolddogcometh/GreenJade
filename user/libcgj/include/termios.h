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
 *
 * Soft residual (C2 libcgj header; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft path  = TCGETS/TCSETS ioctl fill + cfmakeraw/cf* speed catalog;
 *                pipes/ENOTTY soft fail is OK (not product tty mint)
 *   product    = UDX/DDI + hot/cold Linux ABI hosts; sshd/shell TTY product
 *                path remains OPEN (agent PASS != close Dual DoD)
 *   catalog    = c_cc indices + iflag/oflag/cflag/lflag + baud + tc* ops
 *   honesty    = Soft!=product; freestanding class SKIP; no .ko product AC
 *   hazard     = H1 no net_eth_poll from IRQ (N/A here); H2 once-lamp spirit;
 *                H3 thr_exit before as_destroy (process law; not closed here)
 *   Bar honesty v2026.08.04.75. NEVER bump GJ_IMAGE_VERSION from this unit.
 * greppable: libcgj: soft residual termios
 * greppable: libcgj: soft residual termios Soft!=product
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1. Dual DoD A/B OPEN.
 */
#pragma once

#ifndef _TERMIOS_H
#define _TERMIOS_H 1
#endif
#ifndef _GJ_OPENSSH_WINSIZE
#define _GJ_OPENSSH_WINSIZE 1
#endif

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char  cc_t;
typedef unsigned int   speed_t;
typedef unsigned int   tcflag_t;

#define NCCS 32

#ifndef _GJ_WINSIZE_DEFINED
#define _GJ_WINSIZE_DEFINED
struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
};
#endif

#ifndef TIOCGWINSZ
#define TIOCGWINSZ 0x5413
#endif
#ifndef TIOCSWINSZ
#define TIOCSWINSZ 0x5414
#endif
#ifndef TCGETS
#define TCGETS  0x5401
#endif
#ifndef TCSETS
#define TCSETS  0x5402
#endif
#ifndef TCSETSW
#define TCSETSW 0x5403
#endif
#ifndef TCSETSF
#define TCSETSF 0x5404
#endif
#ifndef TCSBRK
#define TCSBRK  0x5409
#endif
#ifndef TCXONC
#define TCXONC  0x540A
#endif
#ifndef TCFLSH
#define TCFLSH  0x540B
#endif

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
#ifndef VSWTCH
#define VSWTCH   VSWTC /* OpenSSH ttymodes.h */
#endif

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
#define IMAXBEL 0020000
#define IUTF8   0040000

/* oflag (Linux soft residual catalog; Soft!=product) */
#define OPOST  0000001
#define OLCUC  0000002
#define ONLCR  0000004
#define OCRNL  0000010
#define ONOCR  0000020
#define ONLRET 0000040
#define OFILL  0000100
#define OFDEL  0000200
#define NLDLY  0000400
#define NL0    0000000
#define NL1    0000400
#define CRDLY  0003000
#define CR0    0000000
#define CR1    0001000
#define CR2    0002000
#define CR3    0003000
#define TABDLY 0014000
#define TAB0   0000000
#define TAB1   0004000
#define TAB2   0010000
#define TAB3   0014000
#define XTABS  0014000
#define BSDLY  0020000
#define BS0    0000000
#define BS1    0020000
#define VTDLY  0040000
#define VT0    0000000
#define VT1    0040000
#define FFDLY  0100000
#define FF0    0000000
#define FF1    0100000

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
#define B230400 0010003
#define B460800 0010004
#define B500000 0010005
#define B576000 0010006
#define B921600 0010007
#define B1000000 0010010
#define B1152000 0010011
#define B1500000 0010012
#define B2000000 0010013
#define B2500000 0010014
#define B3000000 0010015
#define B3500000 0010016
#define B4000000 0010017
#ifndef CBAUD
#define CBAUD    0010017
#endif
#ifndef CBAUDEX
#define CBAUDEX  0010000
#endif
#ifndef EXTA
#define EXTA     B19200
#endif
#ifndef EXTB
#define EXTB     B38400
#endif

/* lflag (Linux soft residual catalog; Soft!=product) */
#define ISIG   0000001
#define ICANON 0000002
#define XCASE  0000004
#define ECHO   0000010
#define ECHOE  0000020
#define ECHOK  0000040
#define ECHONL 0000100
#define NOFLSH 0000200
#define TOSTOP 0000400
#define ECHOCTL 0001000
#define ECHOPRT 0002000
#define ECHOKE  0004000
#define FLUSHO  0010000
#define PENDIN  0040000
#define IEXTEN  0100000
#define EXTPROC 0200000

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

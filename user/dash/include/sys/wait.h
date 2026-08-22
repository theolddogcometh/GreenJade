/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Dash glue: WIFSTOPPED / WSTOPSIG / WCOREDUMP / WIFCONTINUED.
 * Dual MIT OR Apache-2.0. Not a dash source.
 */
#ifndef DASH_GJ_SYS_WAIT_H
#define DASH_GJ_SYS_WAIT_H

#include_next <sys/wait.h>

#ifndef WSTOPSIG
#define WSTOPSIG(s) (((s) >> 8) & 0xff)
#endif
#ifndef WIFSTOPPED
#define WIFSTOPPED(s) (((s) & 0xff) == 0x7f)
#endif
#ifndef WIFCONTINUED
#define WIFCONTINUED(s) ((s) == 0xffff)
#endif
#ifndef WCOREDUMP
#define WCOREDUMP(s) ((s) & 0x80)
#endif

#endif /* DASH_GJ_SYS_WAIT_H */

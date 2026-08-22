/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <paths.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * _PATH_* macros OpenSSH sshd and POSIX utilities include. Strings name
 * conventional Unix locations; they do not prove those files exist on DUT.
 *
 * Dual DoD B OPEN. Soft!=product. G-AC-1. No stamp bump.
 */
#pragma once

#define _PATH_DEFPATH  "/usr/bin:/bin"
#define _PATH_STDPATH  "/usr/bin:/bin:/usr/sbin:/sbin"

#define _PATH_BSHELL   "/bin/sh"
#define _PATH_CONSOLE  "/dev/console"
#define _PATH_CSHELL   "/bin/csh"
#define _PATH_DEVNULL  "/dev/null"
#define _PATH_TTY      "/dev/tty"
#define _PATH_DEV      "/dev/"
#define _PATH_DEVPTMX  "/dev/ptmx"
#define _PATH_PTMX     "/dev/ptmx"
#define _PATH_TMP      "/tmp/"
#define _PATH_VARTMP   "/var/tmp/"
#define _PATH_VARRUN   "/var/run/"
#define _PATH_VARDB    "/var/lib/misc/"
#define _PATH_MAILDIR  "/var/mail"
#define _PATH_LASTLOG  "/var/log/lastlog"
#define _PATH_UTMP     "/var/run/utmp"
#define _PATH_WTMP     "/var/log/wtmp"
#define _PATH_BTMP     "/var/log/btmp"
#define _PATH_SHELLS   "/etc/shells"
#define _PATH_NOLOGIN  "/etc/nologin"
#define _PATH_PASSWD   "/etc/passwd"
#define _PATH_GROUP    "/etc/group"
#define _PATH_SHADOW   "/etc/shadow"
#define _PATH_GSHADOW  "/etc/gshadow"
#define _PATH_MNTTAB   "/etc/fstab"
#define _PATH_MOUNTED  "/etc/mtab"
#define _PATH_KLOG     "/proc/kmsg"
#define _PATH_SENDMAIL "/usr/sbin/sendmail"
#define _PATH_VI       "/usr/bin/vi"

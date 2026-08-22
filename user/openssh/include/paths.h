/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <paths.h> for OpenSSH-portable (HAVE_PATHS_H).
 * Host POSIX compiles (#include_next glibc). Not a claim these files
 * exist on the DUT.
 */
#pragma once
#include_next <paths.h>

#ifndef _PATH_DEFPATH
# define _PATH_DEFPATH "/usr/bin:/bin"
#endif
#ifndef _PATH_STDPATH
# define _PATH_STDPATH "/usr/bin:/bin:/usr/sbin:/sbin"
#endif
#ifndef _PATH_BSHELL
# define _PATH_BSHELL "/bin/sh"
#endif
#ifndef _PATH_CONSOLE
# define _PATH_CONSOLE "/dev/console"
#endif
#ifndef _PATH_TTY
# define _PATH_TTY "/dev/tty"
#endif
#ifndef _PATH_DEVNULL
# define _PATH_DEVNULL "/dev/null"
#endif
#ifndef _PATH_DEV
# define _PATH_DEV "/dev/"
#endif
#ifndef _PATH_TMP
# define _PATH_TMP "/tmp/"
#endif
#ifndef _PATH_VARTMP
# define _PATH_VARTMP "/var/tmp/"
#endif
#ifndef _PATH_VARRUN
# define _PATH_VARRUN "/var/run/"
#endif
#ifndef _PATH_MAILDIR
# define _PATH_MAILDIR "/var/mail"
#endif
#ifndef _PATH_LASTLOG
# define _PATH_LASTLOG "/var/log/lastlog"
#endif
#ifndef _PATH_UTMP
# define _PATH_UTMP "/var/run/utmp"
#endif
#ifndef _PATH_WTMP
# define _PATH_WTMP "/var/log/wtmp"
#endif
#ifndef _PATH_BTMP
# define _PATH_BTMP "/var/log/btmp"
#endif
#ifndef _PATH_SHELLS
# define _PATH_SHELLS "/etc/shells"
#endif
#ifndef _PATH_NOLOGIN
# define _PATH_NOLOGIN "/etc/nologin"
#endif
#ifndef _PATH_PASSWD
# define _PATH_PASSWD "/etc/passwd"
#endif
#ifndef _PATH_GROUP
# define _PATH_GROUP "/etc/group"
#endif
#ifndef _PATH_SHADOW
# define _PATH_SHADOW "/etc/shadow"
#endif
#ifndef _PATH_KLOG
# define _PATH_KLOG "/proc/kmsg"
#endif

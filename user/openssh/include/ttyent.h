/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <ttyent.h> for OpenSSH-portable (HAVE_TTYENT_H).
 * Host POSIX: glibc. DUT: do not include_next libcgj ttyent.h (its
 * file comment contains a star-slash sequence that ends the comment).
 */
#pragma once
#if __STDC_HOSTED__
# include_next <ttyent.h>
#else

struct ttyent {
    char *ty_name;
    char *ty_getty;
    char *ty_type;
    int   ty_status;
    char *ty_window;
    char *ty_comment;
};

#ifndef TTY_ON
# define TTY_ON     0x01
#endif
#ifndef TTY_SECURE
# define TTY_SECURE 0x02
#endif

struct ttyent *getttyent(void);
struct ttyent *getttynam(const char *szName);
int            setttyent(void);
int            endttyent(void);
int            ttyslot(void);

#endif

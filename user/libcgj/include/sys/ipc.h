/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/ipc.h (subset). Not GNU glibc.
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_CREAT  01000
#define IPC_EXCL   02000
#define IPC_NOWAIT 04000

#define IPC_RMID 0
#define IPC_SET  1
#define IPC_STAT 2
#define IPC_INFO 3

#define IPC_PRIVATE ((key_t)0)

struct ipc_perm {
    key_t          __key;
    uid_t          uid;
    gid_t          gid;
    uid_t          cuid;
    gid_t          cgid;
    unsigned short mode;
    unsigned short __pad1;
    unsigned short __seq;
    unsigned short __pad2;
    unsigned long  __unused1;
    unsigned long  __unused2;
};

key_t ftok(const char *szPath, int nProj);

#ifdef __cplusplus
}
#endif

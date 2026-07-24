/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78400 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78400.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78400 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78400 \
 *   user/libcgj/tests/cgj_soft_milestone_78400.c -ldl
 * Direct: -DCGJ_SOFT_M78400_DIRECT + graph_batch78391.c … graph_batch78400.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78400_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78400
#define CGJ_SOFT_B0 78391
#define CGJ_SOFT_B1 78392
#define CGJ_SOFT_B2 78393
#define CGJ_SOFT_B3 78394
#define CGJ_SOFT_B4 78395
#define CGJ_SOFT_B5 78396
#define CGJ_SOFT_B6 78397
#define CGJ_SOFT_B7 78398
#define CGJ_SOFT_B8 78399
#include "_cgj_soft_m_template.inc"

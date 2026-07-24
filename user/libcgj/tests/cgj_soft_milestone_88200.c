/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88200 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88200.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88200 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88200 \
 *   user/libcgj/tests/cgj_soft_milestone_88200.c -ldl
 * Direct: -DCGJ_SOFT_M88200_DIRECT + graph_batch88191.c … graph_batch88200.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88200_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88200
#define CGJ_SOFT_B0 88191
#define CGJ_SOFT_B1 88192
#define CGJ_SOFT_B2 88193
#define CGJ_SOFT_B3 88194
#define CGJ_SOFT_B4 88195
#define CGJ_SOFT_B5 88196
#define CGJ_SOFT_B6 88197
#define CGJ_SOFT_B7 88198
#define CGJ_SOFT_B8 88199
#include "_cgj_soft_m_template.inc"

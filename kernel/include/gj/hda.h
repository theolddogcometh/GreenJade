/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * HDA controller probe + software PCM stream path (A1).
 * Clean-room multi-stream mixer + soft codec (no GPL HDA paste).
 * Real DMA/CORB when BAR0 present; soft path always available for smoke.
 */
#pragma once

#include <gj/types.h>

#define GJ_HDA_STREAM_PCM_BYTES 4096u
#define GJ_HDA_CHANNELS_STEREO  2u
#define GJ_HDA_RATE_48K         48000u
#define GJ_HDA_BITS_16          16u

void hda_stub_probe(void);
int  hda_stub_ready(void);

/* Software stream (works with or without PCI HDA present) */
int  hda_stream_open(u32 u32Channels, u32 u32RateHz, u32 u32Bits);
void hda_stream_close(void);
int  hda_stream_ready(void);
/** Queue interleaved PCM samples; returns bytes accepted. */
u32  hda_stream_write(const void *pData, u32 u32Bytes);
/** Start consuming the ring (software “DMA running”). */
int  hda_stream_start(void);
void hda_stream_stop(void);
/** Advance stream by u32Frames samples/channel (timer/IRQ tick). */
u32  hda_stream_tick(u32 u32Frames);
u32  hda_stream_bytes_queued(void);
u32  hda_stream_bytes_played(void);
u32  hda_stream_underruns(void);
/** Fill silence and play a short pattern — used by smoke. */
int  hda_stream_smoke(void);

/* CORB/RIRB software rings + BDL (pre-real-DMA) */
#define GJ_HDA_CORB_ENTRIES 8u
#define GJ_HDA_BDL_ENTRIES  4u

struct gj_hda_bdl_entry {
    u64 u64Addr; /* software cookie / host VA stand-in */
    u32 u32Len;
    u32 u32Ioc;
};

int  hda_corb_init(void);
/** Queue a codec verb; returns 0 or -1 if full. */
int  hda_corb_send(u32 u32Verb);
/** Pop RIRB response; returns 1 if filled. */
int  hda_rirb_pop(u32 *pResp);
u32  hda_corb_sent(void);
u32  hda_rirb_count(void);
/** Install BDL for stream 0; software copies into PCM ring on start. */
int  hda_bdl_set(const struct gj_hda_bdl_entry *pEnt, u32 u32N);
u32  hda_bdl_entries(void);
int  hda_corb_bdl_smoke(void);
/** Run software CORB DMA: process WP-RP verbs from programmed base → RIRB. */
u32  hda_corb_dma_run(void);
u32  hda_corb_dma_runs(void);
/** Kick BDL stream 0: feed BDL segments into PCM ring and start (software DMA). */
int  hda_bdl_kick(void);
u32  hda_bdl_kicks(void);
/** Non-zero when real BAR0 CORB/RIRB pages are programmed (hardware path). */
int  hda_hw_corb_ready(void);
/** Non-zero when SD0 BDL/PCM pages programmed and stream RUN was set. */
int  hda_stream_dma_ready(void);

/* ---- Multi-stream mixer (SD0 + SD1) + soft codec (clean-room) ---- */
#define GJ_HDA_STREAMS_MAX 2u
/** Soft mixdown scratch capacity used by smoke (bytes). */
#define GJ_HDA_MIX_BYTES   512u

/* Soft codec NID map (bring-up model only; not a vendor dump). */
#define GJ_HDA_NID_ROOT 0u
#define GJ_HDA_NID_AFG  1u
#define GJ_HDA_NID_DAC  2u
#define GJ_HDA_NID_PIN  3u

/** Open stream id 0..1 (0 = default path used by hda_stream_*). */
int  hda_stream_n_open(u32 u32Id, u32 u32Channels, u32 u32RateHz, u32 u32Bits);
void hda_stream_n_close(u32 u32Id);
u32  hda_stream_n_write(u32 u32Id, const void *pData, u32 u32Bytes);
int  hda_stream_n_start(u32 u32Id);
void hda_stream_n_stop(u32 u32Id);
u32  hda_stream_n_tick(u32 u32Id, u32 u32Frames);
int  hda_stream_n_ready(u32 u32Id);
u32  hda_stream_n_bytes_queued(u32 u32Id);
u32  hda_stream_n_bytes_played(u32 u32Id);
u32  hda_stream_n_underruns(u32 u32Id);

/*
 * Per-stream soft BDL + stream-descriptor (SD) shadow (pre-real multi-SD DMA).
 * Software DMA advances LPIB on tick/mix; CBL/LVI/FMT mirror controller regs.
 */
int  hda_bdl_n_set(u32 u32Id, const struct gj_hda_bdl_entry *pEnt, u32 u32N);
u32  hda_bdl_n_entries(u32 u32Id);
/** Feed stream BDL segments into that stream's PCM ring and RUN soft SD. */
int  hda_bdl_n_kick(u32 u32Id);
u32  hda_bdl_n_kicks(u32 u32Id);
/** Soft LPIB (bytes consumed by software DMA for stream). */
u32  hda_stream_n_lpib(u32 u32Id);
/** Soft cyclic buffer length (sum of BDL segment lens, or ring fill). */
u32  hda_stream_n_cbl(u32 u32Id);
/** Soft SD format register (HDA stream format bits). */
u32  hda_stream_n_fmt(u32 u32Id);
/** Soft stream tag (1..15) used by SET_CHANNEL_STREAM_ID bind. */
u32  hda_stream_n_tag(u32 u32Id);
int  hda_stream_n_set_tag(u32 u32Id, u32 u32Tag);
/** Soft BDL current segment index / offset (software DMA cursor). */
u32  hda_bdl_n_seg(u32 u32Id);
u32  hda_bdl_n_seg_off(u32 u32Id);
/** IOC (interrupt-on-completion) soft count for stream BDL. */
u32  hda_bdl_n_ioc(u32 u32Id);

/** Master + per-stream gain in 0..256 (256 = unity). */
int  hda_mixer_set_master(u32 u32Gain256);
int  hda_mixer_set_stream(u32 u32Id, u32 u32Gain256);
u32  hda_mixer_master(void);
u32  hda_mixer_stream(u32 u32Id);
/**
 * Soft mixdown tick: sum all running streams (gains already on write) into
 * pOut (optional). Advances each stream ring. Returns mixed bytes produced.
 */
u32  hda_mixer_mix_tick(u32 u32Frames, u8 *pOut, u32 u32OutCap);
u32  hda_mixer_mix_bytes(void);
u32  hda_mixer_mix_underruns(void);

/** CORB codec: send verb and wait for RIRB (software or HW). */
int  hda_codec_verb(u32 u32Cad, u32 u32Nid, u32 u32Verb, u32 u32Payload,
                    u32 *pResp);
/** GET_PARAMETER (verb 0xF00) helper. */
int  hda_codec_get_param(u32 u32Nid, u32 u32Param, u32 *pResp);
/** SET_AMPLIFIER_GAIN_MUTE-shaped (verb 0x3xx) on soft codec + mixer. */
int  hda_codec_set_amp(u32 u32Nid, u32 u32Gain, int fMute);
/** SET_PIN_WIDGET_CONTROL-shaped (verb 0x707). */
int  hda_codec_set_pin(u32 u32Nid, u32 u32Ctrl);
/** SET_POWER_STATE-shaped (verb 0x705); 0 = D0. */
int  hda_codec_set_power(u32 u32Nid, u32 u32State);
/**
 * SET_CHANNEL_STREAM_ID-shaped (verb 0x706): bind converter to stream tag.
 * u32StreamTag 1..15, u32Channel usually 0 for stereo start.
 */
int  hda_codec_set_stream_id(u32 u32Nid, u32 u32StreamTag, u32 u32Channel);
u32  hda_codec_stream_id(void);
u32  hda_codec_stream_channel(void);
u32  hda_codec_pin_control(void);
u32  hda_codec_power_state(void);
int  hda_codec_programmed(void);
/** Discover AFG + program soft DAC/PIN path; returns 0 on PASS. */
int  hda_codec_program(void);
/** Multi-stream + soft codec + mixdown + soft BDL/LPIB smoke (greppable PASS). */
int  hda_multi_stream_smoke(void);

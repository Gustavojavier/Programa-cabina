# SX500 FAST RAW

Experimental firmware research project for Canon PowerShot SX500 IS firmware 1.00D.

## Goal
- Preserve native half-press autofocus, then release shutter on full press.
- Prioritize Bayer RAW capture; retain Canon JPEG as fallback until safe omission is demonstrated.
- Remove nonessential CHDK features without breaking startup, camera controls, exposure, focus, zoom, or file writing.
- Boot from SDHC while keeping internal Canon firmware untouched.

## Source baseline
CHDK source inspected at `strobecat/chdk` branch `trunk`. The SX500IS 1.00D makefile explicitly reuses the 100c implementation because the firmware is identical. This project must keep the 1.00D identity and its matching CRC/boot configuration.

## Safety
No binary in this repository is marked installable until it is compiled and tested for the exact model/firmware. Never flash internal firmware. Preserve a known-good bootable SD card.

## Current engineering findings
- `platform/sx500is/sub/100d/makefile.inc` redirects to 100c source.
- `platform/sx500is/sub/100c/capt_seq.c` has `PAUSE_FOR_FILE_COUNTER 300`, a deliberate 300 ms delay associated with ensuring the file counter is updated before RAW capture. This is an initial candidate for removing/reworking only after understanding file numbering and capture state.
- Capture path calls `sub_FF18ED84_my`, then `capt_seq_hook_raw_here`, and uses Canon capture routines. Do not remove Canon AF/exposure/motor routines.
- `platform/sx500is/platform_camera.h`: RAW dimensions 4704x3504, 12 bits/pixel, Bayer CFA pattern 0x01000201, DNG metadata present.
- `platform/sx500is/kbd.c`: native key map includes half press `0x00000200`, full press `0x00000600`, and full-only `0x00000400`.
- `platform/sx500is/sub/100c/makefile.inc`: 1 MiB EXMEM allocation configured for CHDK. Reduce only after map/heap use analysis.

## Planned phases
1. Minimal feature build while retaining core boot, camera platform, keyboard, capture, filewrite and RAW.
2. Audit capture sequence delays, especially file-counter pause, and inspect hook overhead.
3. Verify RAW data path/DNG conversion and JPEG fallback behaviour.
4. Build reproducible CI artifact, hash it, and label it experimental until camera validation.

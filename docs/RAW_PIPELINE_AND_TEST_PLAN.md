# RAW capture pipeline and safe optimization plan

## What the source proves

The SX500IS capture implementation calls `capt_seq_hook_raw_here()` from its capture task. The generic CHDK hook then:

1. Optionally waits for the Canon file counter to advance.
2. Marks RAW data as available and wakes the CHDK background task.
3. Waits until the background task marks RAW processing complete.

The background task calls `raw_process()` and only then calls `hook_raw_save_complete()`. The camera capture task does not continue past the RAW hook before that signal arrives.

This is a deliberate ownership barrier. The SX500IS firmware analysis comment in `stubs_entry.S` reports one RAW image buffer. Until a second independently owned RAW buffer is confirmed, removing the wait risks the next exposure overwriting data while the prior frame is still being written.

## Two distinct waits — do not confuse them

### File-counter wait

`platform/sx500is/sub/100c/capt_seq.c` sets `PAUSE_FOR_FILE_COUNTER` to 300 ms. The generic hook does not sleep for 300 ms unconditionally. It records the current file counter and polls for a change in 10 ms increments, with 300 ms as the maximum timeout. The source comment says this is needed because `raw_savefile()` uses the updated counter to construct the RAW filename.

Do not simply set this to zero or a tiny value. A counter race can produce a RAW filename that does not match its JPEG, or can overwrite an earlier file. Any adjustment must be tested over many consecutive captures, including slow-card and low-battery conditions.

### RAW-save completion wait

After notifying the background task, the capture task polls `raw_save_stage` until `RAWDATA_SAVED`. The background task sets that state only after `raw_process()` returns. This includes RAW processing and, when enabled, the SD-card write. This wait is currently required for safe single-buffer operation unless reverse engineering proves an alternate buffer or a safe buffer-copy mechanism.

## RAW output choices

In `core/raw.c`, uncompressed RAW is written directly with `write()` using `camera_sensor.raw_size`. DNG output calls `libdng->write_dng()`, which performs additional DNG packaging/metadata work. For a speed-first profile, test uncompressed RAW first and compare its measured write time with DNG. This changes the file format, not the Bayer sensor data. Keep both modes available until tested.

The SX500 camera header reports 4704 x 3504 RAW pixels at 12 bits/pixel. The nominal payload is 24,800,256 bytes (about 23.65 MiB) before file-system overhead or DNG metadata.

## Optimization order

1. Keep the Canon capture/AF/exposure/zoom/motor task sequence intact.
2. Keep the RAW hook and file-write hooks intact.
3. Benchmark uncompressed RAW versus DNG with the same SD card and settings.
4. Turn off optional per-frame processing in the camera configuration for the speed test: bad-pixel correction, curves, shot histogram, RAW histogram, and script shoot hooks. Re-enable one at a time to measure impact.
5. Keep the file-counter wait at 300 ms until timing logs prove the counter consistently advances much earlier on this exact firmware.
6. Do not shorten or remove the RAW-save completion handshake while the camera has only one verified RAW buffer.
7. Only after these measurements consider changes to module selection or RAW save scheduling.

## Test matrix before any installable build

- Firmware identification: exact camera model and 1.00D firmware; confirm CHDK's 1.00D CRC entry matches.
- Boot: load from a prepared SD card; confirm normal Canon firmware remains available after card removal.
- Autofocus: half-press must initiate and complete Canon AF; full press must capture normally.
- Native controls: zoom, playback, display, menu, set, directional keys and video button.
- RAW correctness: compare image dimensions, file size, no truncated files, and decode a sample from every test batch.
- JPEG/RAW numbering: verify pairs remain correctly matched across at least 100 consecutive captures and after power cycling.
- Throughput: record time from first full press to next available shot, for uncompressed RAW and DNG, using the same card; repeat with a fast UHS-I card and a slower card.
- Reliability: perform repeated bursts, low-space test, low-battery test, and cold restart; check for hangs, damaged files and startup failures.

## Current status

This repository currently contains the engineering notes and branch plan, not the upstream CHDK source tree or a compiled `DISKBOOT.BIN`. No firmware image should be placed on the camera until the complete CHDK tree is built for `sx500is` / `100d`, the output is checked against the 1.00D target, and the test matrix above has been passed on the actual camera.

## Source references

- Capture code: https://github.com/strobecat/chdk/blob/trunk/platform/sx500is/sub/100c/capt_seq.c
- Generic RAW handshake: https://github.com/strobecat/chdk/blob/trunk/platform/generic/capt_seq.c
- Background RAW processing: https://github.com/strobecat/chdk/blob/trunk/core/main.c
- RAW/DNG writer: https://github.com/strobecat/chdk/blob/trunk/core/raw.c
- Firmware CRC table: https://github.com/strobecat/chdk/blob/trunk/platform/sx500is/sub/100c/firmware_crc_data.h
- SX500 keyboard map: https://github.com/strobecat/chdk/blob/trunk/platform/sx500is/kbd.c

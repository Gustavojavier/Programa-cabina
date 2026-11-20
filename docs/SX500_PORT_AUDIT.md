# SX500IS firmware 1.00D source audit

Source inspected from `strobecat/chdk` `trunk`.

## Exact firmware mapping
`platform/sx500is/sub/100d/makefile.inc`:

```make
# sx500is - 100d uses 100c source (identical firmware)
TARGET_FW:=$(PLATFORMSUB)
override PLATFORMSUB=100c
include $(camfw)/makefile.inc
```

This is an explicit CHDK-maintainer declaration that 1.00D shares the 100C port implementation. Build target must still be SX500IS 1.00D, never a different model.

## Capture path findings
`platform/sx500is/sub/100c/capt_seq.c` contains:

- `#define PAUSE_FOR_FILE_COUNTER 300`, inherited by `generic/capt_seq.c` through the `capt_seq_hook_raw_here` path. Comment says this delay ensures the file counter is updated. It is a potential latency candidate but must not be changed blindly; inspect the generic hook and file-counter timing first.
- In the capture sequence handler, Canon routine `sub_FF18ED84_my` runs before the CHDK RAW hook.
- `capt_seq_hook_raw_here` executes before the completion path in that message handler.
- `sub_FF18ED84_my` calls the native sequence functions `sub_FF18FC30`, `sub_FF077808`, `sub_FF18E5DC`, `sub_FF18EA2C`, `sub_FF18F3FC`, `sub_FF0FDF98`, `sub_FF07DE40`, and `sub_FF2DC3F0`, with CHDK remote/NR hooks inserted. These should be preserved until each call is understood.

## Keyboard mapping
`platform/sx500is/kbd.c` maps:
- half press: group 2 mask `0x00000200`
- full press: group 2 mask `0x00000600`
- full-only: group 2 mask `0x00000400`

The source comments note that KEY_SHOOT_HALF is asserted whenever KEY_SHOOT_FULL is asserted. Keep this relation and pass native key scanning through.

## RAW geometry
`platform/sx500is/platform_camera.h` declares:
- RAW row pixels: 4704
- RAW rows: 3504
- sensor bits per pixel: 12
- CFA pattern: `0x01000201`
- DNG active area: x 32..4640, y 26..3486
- DNG/JPEG output area 4608 x 3456

Approximate uncompressed payload at 12 bits/pixel: 4704*3504*12/8 = 24,800,256 bytes (~23.65 MiB), before headers/metadata. SDHC is ample for storage; write latency and buffer management matter more.

## Memory allocation
`platform/sx500is/sub/100c/makefile.inc` sets `EXMEM_BUFFER_SIZE=0x100000` (1 MiB) for CHDK and enables CHDK-in-EXMEM. Do not reduce this without reviewing link map, heap, module loading and RAW buffer use.

## First safe engineering changes
1. Build with optional modules disabled using CHDK's supported build configuration rather than deleting runtime dependencies by hand.
2. Preserve capture sequence, keyboard, filewrite, boot, DNG/RAW and essential configuration functions.
3. Inspect generic capture hook to understand the 300 ms file counter delay and whether an event-based wait or early file-counter update can safely replace it.
4. Keep JPEG generation in stock Canon pipeline for the first build; remove/skip JPEG only after trace proves this can be done without breaking capture completion, buffer ownership or file numbering.
5. Never write to internal firmware; all prototypes must be SD-card loaded and recoverable.

## Refs
- https://github.com/strobecat/chdk/tree/trunk/platform/sx500is
- https://github.com/strobecat/chdk/blob/trunk/platform/sx500is/sub/100d/makefile.inc
- https://github.com/strobecat/chdk/blob/trunk/platform/sx500is/sub/100c/capt_seq.c
- https://github.com/strobecat/chdk/blob/trunk/platform/sx500is/platform_camera.h
- https://github.com/strobecat/chdk/blob/trunk/platform/sx500is/kbd.c

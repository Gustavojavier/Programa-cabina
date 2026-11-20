# SX500 FAST RAW - conservative CHDK build profile
# Target: Canon PowerShot SX500 IS, Canon firmware 1.00D.
# The target directory 100d intentionally reuses the port's shared 100c code,
# while retaining the 100d firmware CRC and target identity.
PLATFORM=sx500is
PLATFORMSUB=100d

# Trim optional CHDK components only. Do not alter Canon capture/AF/zoom code.
OPT_CURVES=
OPT_DEBUGGING=
OPT_PTP=
OPT_GEN_SIGS=
OPT_GEN_STUBS=

# No OPT_FI2: this package is SD-card autoboot only, never an internal flash update.
# No EXMEM or capture-task timing changes.
# RAW remains the standard CHDK RAW path; JPEG stays enabled.

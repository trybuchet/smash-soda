nvenc_shim
Developed by Luiz Henrique
GitHub Repo: https://github.com/luizhtss/nvenc_shim/

Drop-in compatibility shim for applications built against older NVIDIA NVENC SDK versions (e.g. SDK 6.0) that no longer work with modern drivers (R550+).
What it does

NVIDIA R550+ drivers removed support for legacy NVENC presets and rate control modes. Applications compiled against older SDKs fail at runtime with UNSUPPORTED_PARAM or INVALID_VERSION errors.

This shim intercepts NVENC API calls and transparently:

    Translates deprecated presets (e.g. LOW_LATENCY_HQ) to the new P1–P7 preset system with appropriate tuningInfo
    Converts removed rate control modes (CBR_HQ, VBR_HQ, CBR_LOWDELAY_HQ) to their modern equivalents with multi-pass encoding
    Patches struct versions and API version fields to match the current driver
    Zeroes out reserved fields that changed meaning between SDK generations

No modifications to the target application are needed.
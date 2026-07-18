// FF7 Rebirth 1.0.0.5 menu/pause HDR final post-process replacement.
//
// This permutation adds the foreground and background SDR composite layers
// present in the captured 3966BB6523888928 shader, while sharing the tested
// HDR10 output and Gran Turismo 7 path with the gameplay HDR replacement.
#define HDR_COMPOSITE_SDR_THREE_LAYERS
#include "../Includes/PixelShaderPass_PostProcessFinal_HDR_Experimental.hlsl"

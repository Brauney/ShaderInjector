// FF7 Rebirth 1.0.0.5 HDR gameplay final post-process replacement for
// NVIDIA DLSS Frame Generation.
//
// This permutation preserves the captured three-render-target output used by
// the Streamline/DLSS-G composition path while sharing the HDR10 output and
// Gran Turismo 7 tonemapper with the non-FG HDR gameplay replacement.
#define HDR_DLSS_FG_OUTPUTS
#include "../Includes/PixelShaderPass_PostProcessFinal_HDR_Experimental.hlsl"

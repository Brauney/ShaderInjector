// FF7 Rebirth 1.0.0.5 HDR menu/pause final post-process replacement for
// NVIDIA DLSS Frame Generation.
//
// This permutation combines the captured three-layer SDR menu composition
// with the three-render-target output required by the Streamline/DLSS-G path.
#define HDR_COMPOSITE_SDR_THREE_LAYERS
#define HDR_DLSS_FG_OUTPUTS
#include "../Includes/PixelShaderPass_PostProcessFinal_HDR_Experimental.hlsl"

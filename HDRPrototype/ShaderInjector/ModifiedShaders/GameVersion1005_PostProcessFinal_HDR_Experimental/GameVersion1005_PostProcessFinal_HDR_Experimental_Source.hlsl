// FF7 Rebirth 1.0.0.5 HDR final post-process replacement.
//
// Experimental baseline: retains the game's HDR10 PQ/BT.2020 output path and
// composites the SDR UI before that conversion. The implementation lives in
// ModifiedShaders/Includes so it can use ShaderInjector's shared libraries.
#include "../Includes/PixelShaderPass_PostProcessFinal_HDR_Experimental.hlsl"

#ifndef _ROOT_SIGNATURE_HLSLI_
#define _ROOT_SIGNATURE_HLSLI_


#define BindlessRootSignature                                                                                          \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | SAMPLER_HEAP_DIRECTLY_INDEXED), "                              \
    "RootConstants(b0, num32BitConstants=64, visibility = SHADER_VISIBILITY_ALL),"                                     \
/*Point*/					"StaticSampler(s0, filter = FILTER_MIN_MAG_MIP_POINT,"\
		"addressU = TEXTURE_ADDRESS_WRAP, addressV = TEXTURE_ADDRESS_WRAP, "               \
		"addressW = TEXTURE_ADDRESS_WRAP, MipLODBias = 0.0f, MaxAnisotropy = 16, "\
		"comparisonFunc = COMPARISON_ALWAYS, borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK, "\
		"minLOD = 0),"     \
/*Linear*/					"StaticSampler(s1, filter = FILTER_MIN_MAG_MIP_LINEAR,"\
		"addressU = TEXTURE_ADDRESS_WRAP, addressV = TEXTURE_ADDRESS_WRAP, "               \
		"addressW = TEXTURE_ADDRESS_WRAP, MipLODBias = 0.0f, MaxAnisotropy = 16, "\
		"comparisonFunc = COMPARISON_ALWAYS, borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK, "\
		"minLOD = 0),"     \
/*Anisotropic*/				"StaticSampler(s2, filter = FILTER_ANISOTROPIC,"\
		"addressU = TEXTURE_ADDRESS_WRAP, addressV = TEXTURE_ADDRESS_WRAP, "               \
		"addressW = TEXTURE_ADDRESS_WRAP, MipLODBias = 0.0f, MaxAnisotropy = 16, "\
		"comparisonFunc = COMPARISON_ALWAYS, borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK, "\
		"minLOD = 0),"     \
/*Linear Border Black*/		"StaticSampler(s3, filter = FILTER_MIN_MAG_MIP_LINEAR,"\
		"addressU = TEXTURE_ADDRESS_BORDER, addressV = TEXTURE_ADDRESS_BORDER, "               \
		"addressW = TEXTURE_ADDRESS_BORDER, MipLODBias = 0.0f, MaxAnisotropy = 16, "\
		"comparisonFunc = COMPARISON_ALWAYS, borderColor = STATIC_BORDER_COLOR_OPAQUE_BLACK, "\
		"minLOD = 0),"     \
/*Linear Border White*/		"StaticSampler(s4, filter = FILTER_MIN_MAG_MIP_LINEAR,"\
		"addressU = TEXTURE_ADDRESS_BORDER, addressV = TEXTURE_ADDRESS_BORDER, "               \
		"addressW = TEXTURE_ADDRESS_BORDER, MipLODBias = 0.0f, MaxAnisotropy = 16, "\
		"comparisonFunc = COMPARISON_ALWAYS, borderColor = STATIC_BORDER_COLOR_OPAQUE_WHITE, "\
		"minLOD = 0),"     \
/*Shadow*/					"StaticSampler(s5, filter = FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,"\
		"addressU = TEXTURE_ADDRESS_BORDER, addressV = TEXTURE_ADDRESS_BORDER, "               \
		"addressW = TEXTURE_ADDRESS_BORDER, MipLODBias = 0.0f, MaxAnisotropy = 16, "\
		"comparisonFunc = COMPARISON_LESS_EQUAL, borderColor = STATIC_BORDER_COLOR_OPAQUE_WHITE, "\
		"minLOD = 0),"\
/*Shadow*/					"StaticSampler(s6, filter = FILTER_MIN_MAG_MIP_LINEAR,"\
		"addressU = TEXTURE_ADDRESS_CLAMP, addressV = TEXTURE_ADDRESS_CLAMP, "               \
		"addressW = TEXTURE_ADDRESS_CLAMP, MipLODBias = 0.0f, MaxAnisotropy = 16, "\
		"comparisonFunc = COMPARISON_LESS_EQUAL, borderColor = STATIC_BORDER_COLOR_OPAQUE_WHITE, "\
		"minLOD = 0),"     \

SamplerState pointSampler : register(s0);
SamplerState linearSampler : register(s1);
SamplerState anisotropicSampler : register(s2);
SamplerState linearBorderBlackSampler : register(s3);
SamplerState linearBorderWhiteSampler : register(s4);
SamplerComparisonState linearBorderWhiteComparisionSampler : register(s5); //Shadow
SamplerState linearClampSampler : register(s6);


#endif //_ROOT_SIGNATURE_HLSLI_
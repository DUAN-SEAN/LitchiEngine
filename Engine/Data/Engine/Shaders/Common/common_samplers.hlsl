
// comparsion
static const int sampler_compare_depth    = 0;

// regular
static const int sampler_point_clamp      = 0;
static const int sampler_point_wrap       = 1;
static const int sampler_bilinear_clamp   = 2;
static const int sampler_bilinear_wrap    = 3;
static const int sampler_trilinear_clamp  = 4;
static const int sampler_anisotropic_wrap = 5;

// samplers
SamplerComparisonState samplers_comparison[1] : register(s0, space1);
SamplerState samplers[6]                      : register(s1, space2);

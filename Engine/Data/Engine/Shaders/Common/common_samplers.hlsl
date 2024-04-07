
// comparsion
static const int sampler_compare_depth    = 0;

// regular
static const uint sampler_point_clamp_edge = 0;
static const uint sampler_point_clamp_border = 1;
static const uint sampler_point_wrap = 2;
static const uint sampler_bilinear_clamp = 3;
static const uint sampler_bilinear_clamp_border = 4;
static const uint sampler_bilinear_wrap = 5;
static const uint sampler_trilinear_clamp = 6;
static const uint sampler_anisotropic_wrap = 7;
// samplers
SamplerComparisonState samplers_comparison[] : register(s0, space1);
SamplerState samplers[]                      : register(s1, space2);

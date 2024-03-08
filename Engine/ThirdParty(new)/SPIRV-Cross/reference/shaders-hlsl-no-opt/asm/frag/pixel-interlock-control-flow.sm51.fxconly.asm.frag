RasterizerOrderedByteAddressBuffer _11 : register(u1, space0);
RWByteAddressBuffer _17 : register(u2, space0);
RasterizerOrderedByteAddressBuffer _13 : register(u0, space0);

static float4 gl_FragCoord;
struct SPIRV_Cross_Input
{
    float4 gl_FragCoord : SV_Position;
};

void callee2()
{
    int _25 = int(gl_FragCoord.x);
    _11.Store(_25 * 4 + 0, _11.Load(_25 * 4 + 0) + 1u);
}

void callee()
{
    int _38 = int(gl_FragCoord.x);
    _13.Store(_38 * 4 + 0, _13.Load(_38 * 4 + 0) + 1u);
    callee2();
    if (true)
    {
    }
}

void _52()
{
    _17.Store(int(gl_FragCoord.x) * 4 + 0, 4u);
}

void frag_main()
{
    callee();
    _52();
}

void main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    gl_FragCoord.w = 1.0 / gl_FragCoord.w;
    frag_main();
}

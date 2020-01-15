#ifndef SMOKE_FX
#define SMOKE_FX

#include"modelDisolve.fx"

//カラー
StructuredBuffer<float4> smokeColor : register(t60);

//煙シェーダ
float4 PSMain_Smoke(PSInput In) : SV_Target0
{
	float4 color = SozaiNoAziInner(In);
	color *= smokeColor[In.instanceID];
	color.rgb *= color.a;	
	color *= saturate(1.0f - (Disolve_t[In.instanceID] / DisolveTexture.Sample(Sampler, In.TexCoord)));
    if (color.a < 0.0001f)
    {
        discard;
    }
	return color;
}

//色変えビルボードシェーダ
float4 PSMain_ColorChange(PSInput In) : SV_Target0
{
    float4 color = SozaiNoAziInner(In);
    color *= smokeColor[In.instanceID];
    if (color.a < 0.0001f)
    {
        discard;
    }
    color.rgb *= color.a;
    return color;
}

#endif //SMOKE_FX
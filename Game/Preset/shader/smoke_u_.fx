#ifndef SMOKE_FX
#define SMOKE_FX

#include"modelDisolve.fx"

//ÉJÉâÅ[
StructuredBuffer<float4> smokeColor : register(t60);

float4 PSMain_Smoke(PSInput In) : SV_Target0
{
	return PSMain_DisolveSozaiNoAzi_ConvertToPMA(In) * smokeColor[In.instanceID];
}

#endif //SMOKE_FX
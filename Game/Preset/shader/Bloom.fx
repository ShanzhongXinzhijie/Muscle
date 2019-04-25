//定数
cbuffer CSCb : register(b0) {
	//フレームバッファ解像度
	uint win_x;
	uint win_y;

	//輝度しきい値
	float luminanceThreshold;
};

// 入力テクスチャ
Texture2D<float4> inputTex : register(t1);
Texture2D<float4> interferenceFringes : register(t2);//干渉縞

// 出力テクスチャ
RWTexture2D<float4> rwOutputTex : register(u0);

sampler Sampler : register(s0);

static const float2 dirs[4] = {
	float2(4.0f, 4.0f),
	float2(2.0f, 0.0f),
	float2(0.0f, 2.0f),
	float2(1.0f, 3.0f),
};

[numthreads(32, 32, 1)]
void CSmain(uint3 run_xy : SV_DispatchThreadID)
{
	uint2 uv = run_xy.xy;

	if (uv.x % 2 == 0) {
		uv.x = win_x - uv.x;
	}
	if (uv.y % 2 == 0) {
		uv.y = win_y - uv.y;
	}

	//テクスチャサイズ超えてたら出る
	if (uv.x > win_x || uv.y > win_y) {
		return;
	}			
	
	//色取得
	float4 color = inputTex[uv];

	//輝度が設定以下なら抜ける
	float luminance = dot(color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
	if (luminance < luminanceThreshold){
		return;
	}

	//出力
	//color.xyz *= luminance - luminanceThreshold;
	color.xyz -= color.xyz * (luminanceThreshold / luminance);
	color.w = 1.0f;
	rwOutputTex[uv] = color;

	//輝度が高いほどループ回数増える
	float loopmax = min(6.0f, luminance / luminanceThreshold);

	[unroll]
	for (uint i2 = 0; i2 < 4; i2++)
	{
		[unroll]
		for (float i = 0.0f; i < loopmax; i++)
		{
			uint2 sampuv = round((float2)uv + ((i + 1.0f) / loopmax) * dirs[i2] * (luminance / luminanceThreshold));
			uint2 sampuv2 = round((float2)uv + ((i + 1.0f) / loopmax) * dirs[i2] * -(luminance / luminanceThreshold));

			if (sampuv.x < 0 || sampuv.y < 0 || sampuv.x > win_x || sampuv.y > win_y) { break; }
			if (sampuv2.x < 0 || sampuv2.y < 0 || sampuv2.x > win_x || sampuv2.y > win_y) { break; }

			//rwOutputTex[sampuv] = lerp(rwOutputTex[sampuv], color, 1.0f / (i + 2.0f));//サンプル数で透明度		
			//rwOutputTex[sampuv2] = lerp(rwOutputTex[sampuv2], color, 1.0f / (i + 2.0f));//サンプル数で透明度
			//rwOutputTex[sampuv] += color / (i + 2.0f);//サンプル数で透明度		
			//rwOutputTex[sampuv2] += color / (i + 2.0f);//サンプル数で透明度

			rwOutputTex[sampuv] = color * interferenceFringes.SampleLevel(Sampler, float2(i / 6.0f, 0.5f), 0);
			rwOutputTex[sampuv2] = color * interferenceFringes.SampleLevel(Sampler, float2(i / 6.0f, 0.5f), 0);
		}
	}
}
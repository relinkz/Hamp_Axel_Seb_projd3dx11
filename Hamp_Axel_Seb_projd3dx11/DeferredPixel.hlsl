cbuffer world
{
	float4x4 worldViewProj;
	float4x4 eyeSpace;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;
};


Texture2D txDiffuse : register(t0);
Texture2D shadowMap : register(t1);

SamplerState sampAni;


struct defPixelInput
{
	float4 PosWVP : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 PosView : POSITION;
};

struct defPixelOutput
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Pos	: SV_TARGET2;
};

defPixelOutput main(in defPixelInput input) : SV_TARGET
{
	defPixelOutput output;
	float3 posLight = float3(0.0f, 0.0f, -2.0f);
	posLight.xy /= 1.0f;

	float2 smTex = float2(0.5f * posLight.x + 0.5f, -0.5*posLight.y + 0.5f);
	float depth = posLight.z / 1;

	float dx = 1.0f / 4;

	float s0 = (shadowMap.Sample(sampAni, smTex).r + 0.00005 < depth) ? 0.0f : 1.0f;
	float s1 = (shadowMap.Sample(sampAni, smTex + float2(dx, 0.0f)).r + 0.00005 < depth) ? 0.0f : 1.0f;
	float s2 = (shadowMap.Sample(sampAni, smTex + float2(0.0f, dx)).r + 0.00005 < depth) ? 0.0f : 1.0f;
	float s3 = (shadowMap.Sample(sampAni, smTex + float2(dx, dx)).r + 0.00005 < depth) ? 0.0f : 1.0f;

	//sampleSize
	float2 texelPos = smTex * 4;

	float2 lerps = frac(texelPos);

	//i'd like to lerp'it lerp'it
	float iHaveNoClueWhatIAmDoing = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y); //i got it lepin' they hatin'

	
	float test;
	output.Color = float4(txDiffuse.Sample(sampAni, input.Tex).xyz, 0.0f);
	//depth = float4(shadowMap.Sample(sampAni, input.Tex).xyz, 0.0f);
	

	output.Color *= iHaveNoClueWhatIAmDoing;

	float3 normalWS = normalize(input.Normal);

	output.Normal = float4(normalWS, 0.0f);
	output.Normal = saturate(output.Normal);

	//output.Normal = float4(in, 0.0f);
	output.Pos = input.PosView;
	//output.Depth.xyz = mul(output.Depth.xyz, lightProjectionMatrix);


	return output;
}
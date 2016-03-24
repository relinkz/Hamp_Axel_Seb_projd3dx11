
Texture2D txDiffuse : register(t0);
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
	float4 Depth : SV_TARGET0;
	float4 Pos	: SV_TARGET1;
	float4 Normal : SV_TARGET2;
	float4 Color : SV_TARGET3;
};

defPixelOutput main(in defPixelInput input) : SV_TARGET
{
	defPixelOutput output;
	output.Color = float4(txDiffuse.Sample(sampAni, input.Tex).xyz, 0.0f);
	
	float3 normalWS = normalize(input.Normal);

	output.Normal = float4(normalWS, 0.0f);
	output.Normal = saturate(output.Normal);

	//output.Normal = float4(in, 0.0f);
	output.Pos = input.PosView;
	output.Depth.xyz = input.PosWVP.z;

	return output;
}
struct PS_IN
{
	float3 Pos	: POSITION;
	float3 Norm	: NORMAL;
	float2 Tex	: TEXCOORD;
};

struct PS_OUT
{
	float4 Pos	: SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex	: TEXCOORD;
};

float4 main(PS_OUT input) : SV_TARGET
{
	//input.Norm = saturate(input.Norm);
	return float4(input.Norm,0);
	//return float4(1,0,0,0);
}
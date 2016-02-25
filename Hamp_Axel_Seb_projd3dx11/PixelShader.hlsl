cbuffer perFrame : register(b0)
{
	float x;
	float y;
	float z;
	float w;
	float pad;
	float diffuse;
	float strength;
}

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
	


	//return float4(1.0f, 0.0f, 0.0f, 1.0f); //för planet
	return float4(input.Norm,1);
}
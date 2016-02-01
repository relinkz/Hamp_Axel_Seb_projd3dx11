struct PS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

struct PS_OUT
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

float4 main(PS_OUT input) : SV_TARGET
{
	return float4(input.Color, 0);
}
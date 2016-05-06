/*float4 main( in float3 pos : POSITION ) : SV_POSITION
{
	return float4(pos,1.0f);
}*/
struct VS_IN
{
	float3 Pos : POSITION;
	float2 UVCoord : TEXCOORD;
};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 UVCoord : TEXCOORD;
};
VS_OUT main(VS_IN input)
{
	VS_OUT output;
	output.Pos = float4(input.Pos, 1.0f);
	output.UVCoord = input.UVCoord;
	return output;
}
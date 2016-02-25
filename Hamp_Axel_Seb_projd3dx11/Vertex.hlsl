cbuffer perFrame : register(b0)
{
	float4x4 homoClipSpace;
	float4x4 eyeSpace;
}

struct VS_IN
{
	float3 Pos	: POSITION;
	float3 Norm	: NORMAL;
	float2 Tex	: TEXCOORD;
};

struct VS_OUT
{
	float4 Pos	: SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex	: TEXCOORD;
	//float4 Pos2 : POSITION;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), homoClipSpace);
	//output.Pos = float4(input.Pos, 1.0f);
	output.Norm = mul(float4(input.Norm, 0), eyeSpace);
	output.Tex = input.Tex;

	return output;
}
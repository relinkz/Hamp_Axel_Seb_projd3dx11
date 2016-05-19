cbuffer MatrixBuffer: register(b0)
{
	float4x4 homoClipSpace;
	float4x4 worldSpace;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;
}

cbuffer LightBuffer2 : register(b1)
{
	float4x4 lightHomoClipSpace;
};

struct VertexInputType
{
	float4 Position : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct PixelInputType
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float4 LightViewPosition : TEXCOORD1;
	float3 LightPos : TEXCOORD2;
};

float4 ShadowVertexShader(float3 inPosition : SHADOW_POSITION) : SV_POSITION
{
	float4 result = mul(float4(inPosition, 1.0f), lightHomoClipSpace);
	//float4 result = mul(float4(inPosition, 1.0f), lightProjectionMatrix);

	return  result;

	//return mul(float4(posL, 1.0f), homoClipSpace);
}
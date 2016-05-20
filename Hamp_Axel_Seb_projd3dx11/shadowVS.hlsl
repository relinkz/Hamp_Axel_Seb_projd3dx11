cbuffer LightBuffer2 : register(b0)
{
	float4x4 lightHomoClipSpace;
};

float4 ShadowVertexShader(float3 inPosition : SHADOW_POSITION) : SV_POSITION
{
	float4 result = mul(float4(inPosition, 1.0f), lightHomoClipSpace);
	//float4 result = mul(float4(inPosition, 1.0f), lightProjectionMatrix);

	return  result;

	//return mul(float4(posL, 1.0f), homoClipSpace);
}
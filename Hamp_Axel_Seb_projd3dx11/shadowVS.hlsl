cbuffer LightBuffer2 : register(b0)
{
	float4x4 lightHomoClipSpace;
	float4x4 objWorldSpace;
};

float4 ShadowVertexShader(float3 inPosition : SHADOW_POSITION) : SV_POSITION
{
	//place it in the world
	inPosition = mul(float4(inPosition, 1.0f), objWorldSpace);
	//relative to the "light camera"
	float4 result = mul(float4(inPosition, 1.0f), lightHomoClipSpace);

	return  result;
}
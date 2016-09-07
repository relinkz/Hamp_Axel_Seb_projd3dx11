cbuffer LightBuffer2 : register(b0)
{
	float4x4 perspectiveMatrix;
	float4x4 objWorldSpace;
};

float4 ShadowVertexShader(float3 inPosition : SHADOW_POSITION) : SV_POSITION
{
	//place it in the world, relative to camera
	inPosition = mul(float4(inPosition, 1.0f), objWorldSpace);
	//transform the position into homogeneus clip space
	float4 result = mul(float4(inPosition, 1.0f), perspectiveMatrix);

	return  result;
}
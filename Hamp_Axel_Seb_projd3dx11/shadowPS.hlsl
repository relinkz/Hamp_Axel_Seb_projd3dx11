
cbuffer LightBuffer				: register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
}

float4 ShadowPixelShader(in float4 input : SV_POSITION) : SV_TARGET
{

	return float4(0,1,0,0);
}
cbuffer perFrame : register(b0)
{
	float4 lightPos : POSITION;
	float pad;
	float ambient;
	float diffuse;
	float strength;
}
cbuffer CameraData : register(b1)
{
	float4 cameraPos;
}
Texture2D positionMap : register(t0);
SamplerState sampAni;

Texture2D normalMap : register(t1);

Texture2D colorMap : register(t2);


struct PS_IN
{
	float4 Pos	: SV_POSITION;
	float2 UVCoord	: TEXCOORD;
};

struct PS_OUT
{
	float4 Pos	: SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex	: TEXCOORD;
	float4 PosView : POSITION;
};

float4 main(PS_IN input) : SV_TARGET
{
	float3 color;
	float3 pos;
	float3 normal;
	//float3 posLight = float3(cameraPos.x, cameraPos.y, cameraPos.z);	//hardCoded light on the cameraPosition
	float3 posLight = float3(2, 2, 0);								//hardCoded light on x = 0, y = 100, z = 0
	float3 lightColor = float3(1.0f, 1.0f, 1.0f);

	color = colorMap.Sample(sampAni, input.UVCoord).xyz;	//sample the colorMap from DeferredRendering
	pos = positionMap.Sample(sampAni, input.UVCoord).xyz;	//sample the PositionMap from DeferredRendering
	normal = normalMap.Sample(sampAni, input.UVCoord).xyz;	//sample the NormalMap from DeferredRendering
	
	normal = normalize(normal);

	float3 lightVec = posLight - pos;

	lightVec = normalize(lightVec);
	float angle = dot(lightVec, normal);

	if (angle < 0)
	{
		angle = 0;
	}
	//light calculation
	float3 finalColor;
	finalColor.x = color.x * angle + color.x * 0.0f;
	finalColor.y = color.y * angle + color.y * 0.0f;
	finalColor.z = color.z * angle + color.z * 0.0f;

	return float4(finalColor, 1.0f);

}
cbuffer world : register(b0)
{
	float4x4 worldViewProj;
	float4x4 eyeSpace;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;

	float4 camPos;
	float4 lightPosition;  //if not directx will scream at me :(
};
cbuffer CameraData : register(b1)
{
	float4 cameraPos;
}
cbuffer shadowMapData : register(b2)
{
	float4x4 lightWVP;
}
Texture2D positionMap : register(t0);
SamplerState sampAni;

Texture2D normalMap : register(t1);

Texture2D colorMap : register(t2);
Texture2D shadowMap : register(t3);


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
	//float3 posLight = float3(2, 2, 0);								//hardCoded light on x = 0, y = 100, z = 0
	//float3 lightColor = float3(1.0f, 1.0f, 1.0f);

	color = colorMap.Sample(sampAni, input.UVCoord).xyz;	//sample the colorMap from DeferredRendering
	pos = positionMap.Sample(sampAni, input.UVCoord).xyz;	//sample the PositionMap from DeferredRendering
	normal = normalMap.Sample(sampAni, input.UVCoord).xyz;	//sample the NormalMap from DeferredRendering


	normal = normalize(normal);

	float2 shadowUV = float2(0, 0);

	shadowUV = input.UVCoord;

	float4 lightPos = lightPosition;
	//lightPos = cameraPos;

	float lightIntensity = 0.0f;
	float lightDepthValue = 0.0f;
	float depthValue = 0.0f;
	float bias = 0.005f;

	float3 outVec = normalize(lightPos.xyz - pos.xyz);

	lightPos = mul(float4(pos, 1.0f), lightWVP);

	float3 viewDir = normalize(cameraPos.xyz - pos.xyz);

	float shineFactor = 5.0f;
	float lightSpecular = 0.65f;

	/*lightIntensity = dot(normal, outVec);
	if (lightIntensity < 0)
	{
		lightIntensity = 0;
	}*/
	shadowUV.x = ((lightPos.x / lightPos.w) / 2.0f) + 0.5f;
	shadowUV.y = ((lightPos.y / lightPos.w) / -2.0f) + 0.5f;



	if (saturate(shadowUV.x) != shadowUV.x || saturate(shadowUV.y) != shadowUV.y)
	{
		lightIntensity = saturate(dot(normal.xyz, outVec.xyz));
	}
	else
	{
		//pixel is in shadow map
		lightIntensity = 0;

		//calculate the depth of the light
		lightDepthValue = lightPos.z / lightPos.w;

		//sample the shadowmap
		depthValue = shadowMap.Sample(sampAni, shadowUV).r;
		//depthValue = shadowMap.Sample(pointSampler, shadowUV).r;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		//light sees the pixel
		if (lightDepthValue <= depthValue)
		{
			lightIntensity = saturate(dot(outVec.xyz, normal.xyz));
			//color = float3(0, 1, 0);
		}
		//output.Color = float4(0, 1, 0, 0);
	}

	color = saturate((color.rgb * lightIntensity * 0.8f) + (color.rgb * 0.2f));
	return float4(color, 1.0f);
	/*float3 lightVec = posLight - pos;

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

	return float4(finalColor, 1.0f);*/

}
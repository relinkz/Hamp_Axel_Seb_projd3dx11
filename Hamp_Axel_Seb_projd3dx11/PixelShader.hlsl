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
cbuffer mousePosData : register(b3)
{
	float4 mousePos;
}
Texture2D positionMap : register(t0);
SamplerState sampAni;

Texture2D normalMap : register(t1);

Texture2D colorMap : register(t2);
Texture2D shadowMap : register(t3);
Texture2D IDMap : register(t4);


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
	float4 specular;

	float3 id;
	//float3 posLight = float3(cameraPos.x, cameraPos.y, cameraPos.z);	//hardCoded light on the cameraPosition
	//float3 posLight = float3(2, 2, 0);								//hardCoded light on x = 0, y = 100, z = 0
	//float3 lightColor = float3(1.0f, 1.0f, 1.0f);

	color = colorMap.Sample(sampAni, input.UVCoord).xyz;	//sample the colorMap from DeferredRendering
	pos = positionMap.Sample(sampAni, input.UVCoord).xyz;	//sample the PositionMap from DeferredRendering
	normal = normalMap.Sample(sampAni, input.UVCoord).xyz;
	
	//works on my computer
	//id = IDMap.Load(int3((input.UVCoord.x * 640), (input.UVCoord.y * 480), 0));
	
	id = IDMap.Sample(sampAni, input.UVCoord).xyz;

	//sample the NormalMap from DeferredRendering
	//id = IDMap.Sample(sampAni, saturate(mousePos)).xyz;
															//initialize the specular color
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

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

	lightIntensity = saturate(dot(normal, outVec));


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

	float3 diffuseColor = (color.rgb * lightIntensity * 0.8f);
	float3 ambientColor = (color.rgb * 0.2f);

	//color = saturate((color.rgb * lightIntensity * 0.8f) + (color.rgb * 0.2f));
	
	if (lightIntensity > 0.0f)
	{
		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		float3 reflection = normalize(2 * lightIntensity * normal.xyz - outVec.xyz);
		
		//determine the amount of specular light based on the reflection vector, viewing direction and specular power
		specular = pow(saturate(dot(reflection, viewDir)), 32.0f);
	}
	color = saturate(diffuseColor + ambientColor);
	color = saturate(color + specular);

	
	uint3 idColor = uint3(0, 0, 0);
	//uint idtemp = 0;
	float4 idtemp = float4(0, 0, 0, 0);

	//idtemp = IDMap.Load(int3(427,402, 0));
	//idtemp = IDMap.Load(int3(mousePos.x, mousePos.y, 0));
	idtemp = IDMap.Sample(sampAni, float2(mousePos.x / 640, mousePos.y / 480));

	//if the sampled mousepos has the same id as the rendering pixel,
	//that means that the object is highlighted
	if (idtemp.x != 0)
	{
		if (idtemp.x == id.x)
		{
			color.b =+ 50;
			return float4(color, 0);
		}
	}
	return float4(color, 1.0f);
}
cbuffer world : register(b0)
{
	float4x4 worldViewProj;
	float4x4 eyeSpace;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;

	float4 camPos;
	float4 lightPosition;  //if not float4 directx will scream at me :(
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

SamplerState sampAni;

Texture2D positionMap	: register(t0);
Texture2D normalMap		: register(t1);
Texture2D colorMap		: register(t2);
Texture2D shadowMap		: register(t3);
Texture2D IDMap			: register(t4);

struct PointLight
{
	float4 Pos;
	float3 Color;
	float intensity;
};
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
	float4 lightPos;
	
	float3 id;						//keeping track of an objects id
	float2 shadowUV;				//texture uv that sample from the shadowmap texture from DeferredRendering

	float3 vPosToLight;				//this will hold the vector, from the pixel position, to the light position.
	float3 viewDir;					//this will hold the vector, from the pixel position, to the camera position

	float shineFactor;				//variable to create a illution of shinyness
	float lightSpecular;			//unknown, i think light normal?

	//for shadowmapping
	float lightIntensity = 0.0f;	//brightness, the higher, the more lightning applies
	float lightDepthValue = 0.0f;	//how far away an objects pixel is from the light
	float depthValue = 0.0f;		//how far away an objects pixel is from the camera
	float bias = 0.005f;			//a variable used to stabalize trash values

	shadowUV = float2(0, 0); 

	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);	//initialize the specular color
	lightPos = lightPosition;					//can remove this

	color = colorMap.Sample(sampAni, input.UVCoord).xyz;	//sample the colorMap from DeferredRendering
	pos = positionMap.Sample(sampAni, input.UVCoord).xyz;	//sample the PositionMap from DeferredRendering
	normal = normalMap.Sample(sampAni, input.UVCoord).xyz;  //sample the normalMap from DeferredRendering											
	normal = normalize(normal); 
	

	//float4 lightPos = lightPosition;
	float3 outVec = normalize(lightPos.xyz - pos.xyz);
	vPosToLight = lightPos.xyz - pos.xyz;
	vPosToLight = normalize(vPosToLight);

	//adding perspective into the lights position
	lightPos = mul(float4(pos, 1.0f), lightWVP);

	viewDir = cameraPos.xyz - pos.xyz;
	viewDir = normalize(viewDir);

	shineFactor = 5.0f;
	lightSpecular = 0.65f;


	lightIntensity = dot(normal, vPosToLight);
	lightIntensity = saturate(lightIntensity);

	//Create The texture coordinates for the projecting of the shadowMap
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

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		//light sees the pixel
		if (lightDepthValue <= depthValue)
		{
			lightIntensity = saturate(dot(outVec.xyz, normal.xyz));
		}
	}

	float3 diffuseColor = (color.rgb * lightIntensity * 0.8f);
	float3 ambientColor = (color.rgb * 0.2f);
	
	if (lightIntensity > 0.0f)
	{
		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		float3 reflection = normalize(lightIntensity * normal.xyz - outVec.xyz);
		
		//determine the amount of specular light based on the reflection vector, viewing direction and specular power
		specular = pow(saturate(dot(reflection, viewDir)), 32.0f);
	}
	color = saturate(diffuseColor + ambientColor);
	color = saturate(color + specular);
	
	uint3 idColor = uint3(0, 0, 0);

	float4 idtemp = float4(0, 0, 0, 0);

	//sample the specific pixel that are rendereds id. This is used further
	id = IDMap.Sample(sampAni, input.UVCoord).xyz;
	
	//convert the mousePos to UV-coordinates
	idtemp = IDMap.Sample(sampAni, float2(mousePos.x / 640, mousePos.y / 480));

	//if the sampled mousepos has the same id as the rendering pixel,
	//that means that the object is highlighted
	if (idtemp.x != 0)
	{
		if (idtemp.x == id.x)
		{
			color.b =+ 50;
			//return float4(color, 0);
		}
	}

	//does point light calculations
	//this is for specular highlighting
	PointLight pl;
	
	pl.Pos = float4(2.5f, 13.0f, 5.0f, 0.0f);
	pl.Color = float3(0.5f, 0.0f, 0.0f);
	pl.intensity = 1.5f;

	float3 toLightVec = pl.Pos.xyz - pos.xyz;
	float distance = length(toLightVec);
	toLightVec = normalize(toLightVec);
	float angle = dot(toLightVec.xyz, normal.xyz);

	float3 addColor = float3(0.0f, 0.0f, 0.0f);
	if (angle > 0.0f )
	{

		addColor.r += (pl.Color.r * pl.intensity * angle)/ distance;
		addColor.g += (pl.Color.g * pl.intensity * angle)/ distance;
		addColor.b += (pl.Color.b * pl.intensity * angle)/ distance;

		addColor = saturate(addColor);

		color.r += addColor.r;
		color.g += addColor.g;
		color.b += addColor.b;
	}

	return float4(color, 1.0f);
}
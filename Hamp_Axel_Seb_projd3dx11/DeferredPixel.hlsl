cbuffer world
{
	float4x4 worldViewProj;
	float4x4 eyeSpace;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;
};


Texture2D txDiffuse : register(t0);
Texture2D shadowMap : register(t1);

SamplerState sampAni;


struct defPixelInput
{
	float4 PosWVP : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 PosView : POSITION;
};

struct defPixelOutput
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Pos	: SV_TARGET2;
};

defPixelOutput main(in defPixelInput input) : SV_TARGET
{
	float4 camPos = float4(0,0,0,0);
	float4 diffColor = float4(0, 0, 0, 0);
	float2 shadowUV = float2(0, 0);

	float4 lightPos = float4(0, 2, -2, 0);
	float lightIntensity = 0.0f;
	float lightDepthValue = 0.0f;
	float depthValue = 0.0f;
	float bias = 0.00005f;


	defPixelOutput output;
	
	diffColor = txDiffuse.Sample(sampAni, input.Tex);

	output.Color = diffColor;
	//move the position to projection space for the light
	lightPos = mul(lightPos, lightProjectionMatrix);

	//create the normalized vector from position to light source
	float3 outVec = normalize(lightPos.xyz - (input.PosView).xyz);

	//Create the normalized reflection vector
	float3 refVec = normalize(reflect(-outVec, input.Normal));

	//create the normalized vector form positon to camera
	float3 viewDir = normalize(camPos - input.PosView).xyz;

	float specIntesity = saturate(dot(refVec, viewDir));
	float shineFactor = 5.0f;
	float lightSpecular = 0.65f;

	//calculate the specular part
	//float4 specular = float4(specColor.rgb * lightSpecular * max(pow(specIntesity, shineFactor), 0.0f), 1.0f);

	lightIntensity = dot(input.Normal, outVec);
	if (lightIntensity < 0)
	{
		lightIntensity = 0;
	}

	//calculate the projected texture coordinate
	shadowUV.x = (lightPos.x / lightPos.w) * 0.5f + 0.5f;
	shadowUV.y = (lightPos.y / lightPos.w) * -0.5f + 0.5f;

	//If point outside shadowMap
	if (saturate(shadowUV.x) != shadowUV.x || saturate(shadowUV.y) != shadowUV.y)
	{
		lightIntensity = saturate(dot(input.Normal.xyz, outVec.xyz));
	}
	else
	{
		//pixel is in shadow
		lightIntensity = 0;

		//calculate the depth of the light
		lightDepthValue = lightPos.z / lightPos.w;

		//sample the shadowmap
		depthValue = shadowUV = shadowMap.Sample(sampAni, shadowUV).r;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;


		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.Normal.xyz, outVec.xyz));
		}
		
	}
	output.Color = saturate((diffColor.rgba * lightIntensity * 0.8f) + 0.2f);
	return output;
}
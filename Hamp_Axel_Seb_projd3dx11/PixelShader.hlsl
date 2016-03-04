cbuffer perFrame : register(b0)
{
	float4 lightPos : POSITION;
	float pad;
	float ambient;
	float diffuse;
	float strength;
}

Texture2D txDiffuse : register(t0);
SamplerState sampAni;

struct PS_IN
{
	float3 Pos	: POSITION;
	float3 Norm	: NORMAL;
	float2 Tex	: TEXCOORD;
};

struct PS_OUT
{
	float4 Pos	: SV_POSITION;
	float3 Norm : NORMAL;
	float2 Tex	: TEXCOORD;
	float4 PosView : POSITION;
};

float4 main(PS_OUT input) : SV_TARGET
{
	float3 text;

	text = txDiffuse.Sample(sampAni, input.Tex).xyz;

	float3 pixelPos = float3(input.PosView.xyz);
	float3 posLight = lightPos.xyz;

	//float3 lightVector = posLight - float3(0.0f, 0.0f, 4.0f);

	float3 lightVector = posLight - pixelPos;
	lightVector = normalize(lightVector);

	//float4 pixToLight = lightPos - input.PosView;
	float angle = dot(lightVector, (input.Norm));
	angle = saturate(angle);

	//text.x = diffuse * text.x * angle + text.x * ambient;
	//text.y = diffuse * text.y * angle + text.y * ambient;
	//text.z = diffuse * text.z * angle + text.z * ambient;
	if (angle < 0)
		angle = 0;

	text.x = diffuse * text.x * angle + text.x * ambient;
	text.y = diffuse * text.y *angle + text.y * ambient;
	text.z = diffuse * text.z *angle + text.z * ambient;

	//if (angle >= 0) //there is a way for the light to hit the surface
	//{
	//	text.x = diffuse * text.x * angle + text.x * ambient;
	//	text.y = diffuse * text.y *angle + text.y * ambient;
	//	text.z = diffuse * text.z *angle + text.z * ambient;
	//}
	//else //so that ambient just applies
	//{
	//	text.x = text.x * ambient;
	//	text.y = text.y * ambient;
	//	text.z = text.z * ambient;
	//}

	/*
	if (angle >= 0)
	{
		text.x = diffuse * text.x * angle + text.x * ambient;
		text.y = diffuse * text.y *angle + text.y * ambient;
		text.z = diffuse * text.z *angle + text.z * ambient;
	}
	else
	{
		text.x = text.x * ambient;
		text.y = text.y * ambient;
		text.z = text.z * ambient;
	}
	*/

	//return float4(text, lights[5]);
	return float4(text, 0.0f);
	//return float4(1.0f, 0.0f, 0.0f, 1.0f); //f�r planet
	//return float4(input.Norm,1);
}
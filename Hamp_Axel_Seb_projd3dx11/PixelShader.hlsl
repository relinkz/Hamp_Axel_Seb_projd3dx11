cbuffer perFrame : register(b0)
{
	float4 lightPos : POSITION;
	float pad;
	float ambient;
	float diffuse;
	float strength;
}

Texture2D positionMap : register(t0);
SamplerState sampAni;

Texture2D normalMap : register(t1);

Texture2D colorMap : register(t2);


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

float4 main(in float4 screenPos : SV_POSITION) : SV_TARGET0
{
	float3 text;


	text = positionMap.Load(screenPos).xyz;

	return float4(1.0f, 1.0f, 1.0f, 0.0f);
//float3 pixelPos = float3(input.PosView.xyz);
//float3 posLight = lightPos.xyz;
//
////float3 lightVector = posLight - float3(0.0f, 0.0f, 4.0f);
//
//float3 lightVector = posLight - pixelPos;
//lightVector = normalize(lightVector);
//
////float4 pixToLight = lightPos - input.PosView;
//float angle = dot(lightVector, (input.Norm));
////angle = saturate(angle);
//
//if (angle < 0)
//{
//	angle = 0;
//}
//
//text.x = diffuse * text.x * angle + text.x * ambient;
//text.y = diffuse * text.y * angle + text.y * ambient;
//text.z = diffuse * text.z * angle + text.z * ambient;
//
//

/*if (angle >= 0) //there is a way for the light to hit the surface
{
text.x = diffuse1 * text.x * angle + text.x * ambient1;
text.y = diffuse1 * text.y * angle + text.y * ambient1;
text.z = diffuse1 * text.z * angle + text.z * ambient1;
}
else //so that ambient just applies
{
text.x = text.x * ambient1;
text.y = text.y * ambient1;
text.z = text.z * ambient1;
}*/

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
//return float4(1.0f, 0.0f, 0.0f, 1.0f); //för planet
//return float4(input.Norm,1);
}
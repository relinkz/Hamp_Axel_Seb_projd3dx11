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
   // int3 sampleIndices = int3(input.Pos.xy, 0);
	//text = positionMap.Load(sampleIndices).xyz;
	float3 posLight = float3(cameraPos.x, cameraPos.y, cameraPos.z);
	//float3 posLight = float3(2, 2, 2);
	float3 lightColor = float3(1.0f, 1.0f, 1.0f);

	color = colorMap.Sample(sampAni, input.UVCoord).xyz;
	pos = positionMap.Sample(sampAni, input.UVCoord).xyz;
	normal = normalMap.Sample(sampAni, input.UVCoord).xyz;
	
	normal = normalize(normal);
	float3 newNormal;
	newNormal.x = ((normal.x * 2) - 1);
	newNormal.y = ((normal.y * 2) - 1);
	newNormal.z = ((normal.z * 2) - 1);

	float3 lightVec = posLight - pos;

	lightVec = normalize(lightVec);
	float angle = dot(lightVec, normal);

	if (angle < 0)
	{
		angle = 0;
	}
	float3 finalColor;
	finalColor.x = color.x * angle + color.x * 0.0f;
	finalColor.y = color.y * angle + color.y * 0.0f;
	finalColor.z = color.z * angle + color.z * 0.0f;
	return float4(finalColor, 1.0f);
	return float4(1.0f,0.0f,1.0f, 0.0f);
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
//return float4(text, 0.0f);
//return float4(1.0f, 0.0f, 0.0f, 1.0f); //för planet
//return float4(input.Norm,1);
}
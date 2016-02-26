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
};

float4 main(PS_OUT input) : SV_TARGET
{
	float3 text;

text = txDiffuse.Sample(sampAni, input.Tex).xyz;

/*text.x *= ambient;
text.y *= ambient;
text.z *= ambient;*/

float4 pixToLight = lightPos - float4(input.Norm, 1.0f);
float angle = dot(pixToLight, input.Norm);



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


//return float4(text, lights[5]);
return float4(text, 0.0f);
//return float4(1.0f, 0.0f, 0.0f, 1.0f); //för planet
//return float4(input.Norm,1);
}
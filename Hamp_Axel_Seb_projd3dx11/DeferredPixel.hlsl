
Texture2D txDiffuse : register(t0);
Texture2D normalMap : register(t1);
SamplerState sampAni;


struct defPixelInput
{
	float4 PosWVP : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float4 PosView : POSITION;
	float3 Tangent : TANGENT;
	uint objID : ID;
};

struct defPixelOutput
{
	float4 Depth : SV_TARGET0;
	float4 Pos	: SV_TARGET1;
	float4 Normal : SV_TARGET2;
	float4 Color : SV_TARGET3;
	uint objID : SV_TARGET4;
};

defPixelOutput main(in defPixelInput input) : SV_TARGET
{
	float3 text;
	float3 normalTexture;
	defPixelOutput output;

	output.objID = input.objID;

	output.Color = float4(txDiffuse.Sample(sampAni, input.Tex).xyz, 0.0f);  //sample the DiffuseMap
	normalTexture = normalMap.Sample(sampAni, input.Tex).xyz;				//sample the NormalMap
	

	normalTexture = (normalTexture * 2.0f) - 1.0f;
	//creates the TBN matrix used to transform the normal that we read from the Texture to the Direction the pixel is facing
	float3 objectNormal = normalize(input.Normal);
	float3 tangent = normalize(input.Tangent - (dot(input.Tangent, objectNormal) * objectNormal));
	float3 B = normalize(cross(objectNormal, input.Tangent));

	float3x3 TBN = float3x3(input.Tangent, B, objectNormal);

	float3 newNormal = normalize(mul(normalTexture, TBN)); //transform the normal
	
	//output.Normal = float4(newNormal, 0.0f);
	output.Normal = float4(input.Normal, 0.0f);

	output.Pos = input.PosView;
	output.Depth.xyz = input.PosWVP.z;


	return output;
}
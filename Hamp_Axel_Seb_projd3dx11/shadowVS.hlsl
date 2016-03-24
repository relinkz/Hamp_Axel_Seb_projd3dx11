cbuffer MatrixBuffer: register(b0)
{
	float4x4 homoClipSpace;
	float4x4 worldSpace;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;
}

cbuffer LightBuffer2
{
	float3 lightPosition;
	float padding;
};

struct VertexInputType
{
	float4 Position : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct PixelInputType
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float4 LightViewPosition : TEXCOORD1;
	float3 LightPos : TEXCOORD2;
};

float4 ShadowVertexShader(VertexInputType input) : SV_POSITION
{
	/*
	PixelInputType output;
	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.Position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(input.Position, homoClipSpace);
	//output.Position = mul(output.Position, viewMatrix);
	//output.Position = mul(output.Position, projectionMatrix);

	/*
	Transform vertex based on the lights perspective
	
	output.LightViewPosition = mul(input.Position, lightViewMatrix);
	output.Position = mul(output.LightViewPosition, lightProjectionMatrix);

	output.Tex = input.Tex;		//need uv here it seems

	//calc normal vector against the worldmatrix
	output.Normal = mul(float4(input.Normal, 1), worldSpace);
	output.Normal = normalize(output.Normal);

	//calculate the position of the vertex in the world
	worldPosition = mul(input.Position, worldSpace);

	//determine the light position based on the position of the light and the position of the vertex in the world
	output.LightPos = lightPosition.xyz - worldPosition.xyz;

	output.LightPos = normalize(output.LightPos);
	*/
	//render from the lights perspective
	return mul(float4(lightPosition, 1.0f), lightProjectionMatrix);
}
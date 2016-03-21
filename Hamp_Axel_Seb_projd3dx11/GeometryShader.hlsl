cbuffer perFrame : register(b0)
{
	float4x4 _matrix;
};
cbuffer perFrame : register(b1)
{
	float4x4 worldViewMatrix;
};
struct GSOutput
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Norm : NORMAL;
	float4 Pos2 : POSITION;
};

[maxvertexcount(7)]
void main(triangle VS_OUT input[3], inout TriangleStream< VS_OUT > data)
{

	float4 normal = float4(cross(input[1].Pos - input[0].Pos, input[2].Pos - input[0].Pos), 0.0f);

	normal = normalize(normal);

	float4 rotNorm = mul(normal, worldViewMatrix);
		rotNorm = normalize(rotNorm);

	VS_OUT output[3];
	for (uint i = 0; i < 3; i++)
	{
		output[i].Pos = input[i].Pos;
		output[i].Pos = mul(input[i].Pos, _matrix);
		//output[i].Color = float4(input[i].Color, 0.0f);
		output[i].Tex = input[i].Tex;
		output[i].Norm = rotNorm;
		output[i].Pos2 = mul(input[i].Pos, worldViewMatrix);
		data.Append(output[i]);


	}
	data.RestartStrip();
	VS_OUT outputCopy[3];
	for (uint i = 0; i < 3; i++)
	{
		outputCopy[i].Pos = input[i].Pos + normal;
		outputCopy[i].Pos = mul(outputCopy[i].Pos, _matrix);
		//outputCopy[i].Color = float4(input[i].Color, 0.0f);
		outputCopy[i].Tex = input[i].Tex;
		outputCopy[i].Norm = rotNorm;
		outputCopy[i].Pos2 = mul(input[i].Pos, worldViewMatrix);
		data.Append(outputCopy[i]);
	}
	data.RestartStrip();


}
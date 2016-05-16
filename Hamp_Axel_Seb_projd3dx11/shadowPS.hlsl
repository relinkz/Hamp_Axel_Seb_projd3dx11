/*
Textures
*/
Texture2D shaderTexture		: register(t0);
Texture2D depthMapTexture	: register(t1);

/*
Sampler states

req a ckamp based sampler when sampling the depth budder so that it doesn't wrap around
and sample incorrect information
*/

SamplerState SampleTypeClamp	: register(s0);
SamplerState SampleTypeWrap		: register(s1);

cbuffer LightBuffer				: register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
}

struct PixelInputType
{
	float4 position				: SV_POSITION;
	float2 tex					: TEXCOORD0;
	float3 normal				: NORMAL;
	float4 lightViewPosition	: TEXCOORD1;
	float3 lightPos				: TEXCOORD2;
};

float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
//	float bias;
//	float4 color;
//	float2 projectTexCoord;
//	float depthValue;
//	float lightDepthValue;
//	float lightIntensity;
//	float4 textureColor;
//
//	//shadow mapping requires a bias adjustment when comparing the depth of the light and the depth of the object
//	//due to the low floating point precision of the depth map
//	bias = 0.001f;
//
//	//set the default output color to the ambient light value for all pixels
//	color = ambientColor;
//
//	/*
//	Calculate the projected texture coordinates for sampling the shadow map (depth buffer texture) based on
//	the light's viewing position
//	
//	Sebban noterar:
//	WTF händer här, kolla upp!! tror det har att göra med norm device coord?
//	*/
//
//	//calculate the projected texture coordinates
//	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
//	projectTexCoord.y = input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
//	
//	//Check if the projected coordinates are in the view of the light, if not then the pixel gets just an ambient value. 
//	//Determine if the projected coordinates are in 0 to 1 range, If so, the pixel is in the view of the light
//	if (
//		(saturate(projectTexCoord.x) == projectTexCoord.x)
//		&&
//		(saturate(projectTexCoord.y) == projectTexCoord.y)
//		)
//	{
//		/*
//		Now that we are in the view of the light we will retrieve the depth value from the shadow map (depthMapTexture). 
//		We only sample the red component since this is a grey scale texture. 
//		The depth value we get from the texture translates into the distance to the nearest object. 
//		This is important since objects are what cast the shadows and hence why it is called a shadow map. 
//		*/
//
//		//sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location
//		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
//
//		//now that we have the depth of the object for this pixel we need the depth of the light to determine if it is in front or
//		//behind the object, We get this from the lightViewPosition. Note that we need to subtract the bias from this or
//		//we will get the floating point precision issue
//
//		//calculate the depth of the light
//		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
//
//		//subtract the bias from the lightDepthValue
//		lightDepthValue = lightDepthValue - bias;
//
//		/*
//		 Now we perform the comparison between the light depth and the object depth. 
//		 If the light is closer to us then no shadow. 
//		 But if the light is behind an object in the shadow map then it gets shadowed. 
//		 Note that a shadow just means we only apply ambient light, we don't color it black or anything.
//
//         Compare the depth of the shadow map value and the depth of the 
//		 light to determine whether to shadow or to light this pixel.
//         If the light is in front of the object then light the pixel, 
//		 if not then shadow this pixel since an object (occluder) is casting a shadow on it.
//		*/
//
//		if (lightDepthValue < depthValue)
//		{
//			//if the light was in front of the object then there is no shadow and we do regular lightning
//			//calculate the amount of light on this pixel.
//			lightIntensity = saturate(dot(input.normal, input.lightPos));
//			if (lightIntensity > 0.0f)
//			{
//				//determine the final diffuse color based on the diffuse color and the amount of light intensity
//				color += (diffuseColor * lightIntensity);
//
//				//saturate the final color
//				color = saturate(color);
//			}
//		}
//	}
//	
//	//sample the pixel color from the texture using the sampler at this texture coordinate location.
//	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
//
//	//combine the light and texture color
//	color = color * textureColor;
//
	return float4(1,0,0,0);


}
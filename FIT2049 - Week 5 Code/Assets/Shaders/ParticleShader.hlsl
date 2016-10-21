cbuffer ParticleData
{
	float3 camPosition;
	float particleRadius;
	matrix worldViewProj;
	float4 particleColour;
};

struct VertexInput
{
	float4 position : POSITION;
};

struct GeomInput
{
	float4 position : SV_POSITION;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D particleTexture;
SamplerState samplerType;

GeomInput VSMain(VertexInput input)
{
	GeomInput output;

	input.position.w = 1.0f;

	output.position = input.position;

	return output;
}

[maxvertexcount(4)]
void GSMain(point GeomInput input[1], uint primitiveID : SV_PrimitiveID, inout TriangleStream<PixelInput> OutputStream)
{
	float3 planeNormal = input[0].position - camPosition;
	planeNormal = normalize(planeNormal);

	float3 upVector = float3(0.0f, 1.0f, 0.0f);
	
	float3 rightVector = normalize(cross(planeNormal, upVector)); // Cross planes normal with the up vector (+y) to get billboards right vector

	rightVector = rightVector * particleRadius;
	upVector = float3(0, particleRadius, 0);

	// Create the billboards quad
	float3 vert[4];

	// We get the points by using the billboards right vector and the billboards height
	vert[0] = input[0].position - rightVector - upVector; // Get bottom left vertex
	vert[1] = input[0].position + rightVector - upVector; // Get bottom right vertex
	vert[2] = input[0].position - rightVector + upVector; // Get top left vertex
	vert[3] = input[0].position + rightVector + upVector; // Get top right vertex

	// Get billboards texture coordinates
	float2 texCoord[4];
	texCoord[0] = float2(0, 1);
	texCoord[1] = float2(1, 1);
	texCoord[2] = float2(0, 0);
	texCoord[3] = float2(1, 0);

	// Now we "append" or add the vertices to the outgoing stream list
	PixelInput outputVert;
	for (int i = 0; i < 4; i++)
	{
		outputVert.position = mul(float4(vert[i], 1.0f), worldViewProj);
		outputVert.uv = texCoord[i];

		OutputStream.Append(outputVert);
	}
}

float4 PSMain(PixelInput input) : SV_TARGET
{
	float4 textureColour = particleTexture.Sample(samplerType, input.uv);
	return saturate(particleColour * textureColour);
}
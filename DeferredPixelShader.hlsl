Texture2D shaderTexture:register(t0);
Texture2D normalTexture:register(t1);

SamplerState SampleTypeWrap:register(s0);

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
	float4 color:COLOR;
	float4 normal:NORMAL;
	float4 tangent:TANGENT;
	float4 binormal:BINORMAL;
	float4 worldPosition:TEXCOORD1;
	float4 lightViewPosition:TEXCOORD2;
};

struct PixelOutputType
{ 
	float4 color:SV_TARGET0;
	float4 normal:SV_TARGET1;
	float4 pixelPos:SV_TARGET2;
	float4 lightViewPos:SV_TARGET3;
};

//Pixel Shader
PixelOutputType DPS(PixelInputType input):SV_TARGET
{
	PixelOutputType output;
	float4 bumpNormal;
	float4x3 TBN;
	float4 T, N, B;

	//Sample the pixel color from the texture using the sampler at this texture coordinate location
	output.color = shaderTexture.Sample(SampleTypeWrap, input.tex);
	bumpNormal = normalTexture.Sample(SampleTypeWrap, input.tex);

	//Add color map to texture color
	output.color = saturate(output.color * input.color);

	//If not then normalize the tangent and normal vectors
	T = normalize(input.tangent);
	N = normalize(input.normal);
	B = normalize(input.binormal);

	//Construct the TBN matrix
	TBN = transpose(float3x4(T, B, N));

	//Expand the range of the normal value from (0, +1) to (-1, +1)
	bumpNormal = normalize(bumpNormal * 2.0f - 1.0f);
	
	//Calculate the normal from the data in the bump map
	output.normal = mul(TBN, bumpNormal);

	//Normalize the new normal
	output.normal = normalize(output.normal);

	//Set the output pixel position equal to the pixel position in world space
	output.pixelPos = input.worldPosition;

	//Set the lights view position
	output.lightViewPos = input.lightViewPosition;

	return output;
}
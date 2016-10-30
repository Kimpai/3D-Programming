struct PixelInputType
{
	float4 position:SV_POSITION;
};

struct PixelOutputType
{
	float4 depthPos:SV_TARGET0;
};

PixelOutputType SPS(PixelInputType input):SV_TARGET
{
	PixelOutputType output;	
	float depthValue;

	depthValue = input.position.z / input.position.w;

	output.depthPos = float4(depthValue, depthValue, depthValue, 1.0f);

	return output;
}
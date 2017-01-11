Texture2D shaderTexture:register(t0);
SamplerState SampleType:register(s0);

struct PixelInputType
{
	float4 positon:SV_POSITION;
	float2 tex:TEXCOORD0;
	float4 color:COLOR;
};

struct PixelOutputType
{ 
	float4 color:SV_TARGET0;
};

PixelOutputType PPS(PixelInputType input):SV_TARGET
{
	PixelOutputType output;
	float4 textureColor;
	float4 finalColor;

	textureColor = shaderTexture.Sample(SampleType, input.tex);

	output.color = textureColor * input.color;

	return output;
}
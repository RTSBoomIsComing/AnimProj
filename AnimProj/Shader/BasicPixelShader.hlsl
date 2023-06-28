struct PixelShaderInput
{
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
};

float4 main(PixelShaderInput psInput) : SV_TARGET
{
	return psInput.color;
} 
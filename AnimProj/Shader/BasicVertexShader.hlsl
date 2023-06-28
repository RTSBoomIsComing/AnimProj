struct CameraConstantBuffer
{
	float4x4 View;
	float4x4 Projection;
};

struct VertexShaderInput
{
	float4 position	: POSITION;
	float4 color	: COLOR;
};

struct VertexShaderOutput
{
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
};

VertexShaderOutput main(VertexShaderInput vsInput)
{
	VertexShaderOutput vsOut = (VertexShaderOutput)0;
	vsOut.position = vsInput.position;
	vsOut.color = vsInput.color;
	return vsOut;
}
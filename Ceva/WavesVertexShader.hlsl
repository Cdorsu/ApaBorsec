
struct VSOut
{
	float4 Position : POSITION;
};

VSOut main(float4 pos : POSITION)
{
	VSOut output = (VSOut) 0;
	output.Position = pos;
	return output;
}

struct PSIn
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

float4 main(PSIn input) : SV_TARGET
{
    return input.Color;
}
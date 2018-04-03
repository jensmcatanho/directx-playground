struct VS_INPUT {
	float4 position : POSITION;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT VShader(VS_INPUT input) {
	VS_OUTPUT output;
	output.position = float4(input.position.x - 0.5, input.position.y - 0.5, input.position.z, input.position.w);
	output.color = float4(input.position.x, input.position.y, -input.position.z, 1.0);

    return output;
}
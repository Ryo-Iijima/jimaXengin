#include "FBXShader.hlsli"

Texture2D<float4> tex :register(t0);
SamplerState smp : register(s0);

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

PSOutput main(VSOutput input) : SV_TARGET
{
	PSOutput output;

	//return float4(1,0,0,1);

	float4 texcolor = tex.Sample(smp,input.uv);

	//return texcolor;

	// �V�F�[�f�B���O�ɂ��F
	float4 shadecolor;
	// ����x
	const float shinines = 4.0f;
	// ���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - input.svpos.xyz);
	// ���C�g�Ɍ������x�N�g���̖@���̓���
	float3 dotlightnormal = dot(lightv, input.normal);
	// ���ˌ��x�N�g��
	float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	// �����ˌ�
	float3 ambient = m_ambient;
	// �g�U���ˌ�
	float3 diffuse = dotlightnormal * m_diffuse;
	//// ���ʔ��ˌ�
	float3 specular = pow(saturate(dot(reflect, eyedir)), shinines) * m_specular;

	shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
	shadecolor.a = m_alpha;

	output.target0 = shadecolor * texcolor * color;
	output.target1 = output.target0;

	return output;
}
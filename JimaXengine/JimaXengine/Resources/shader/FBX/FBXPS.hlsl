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

	// �e�N�X�`���}�b�s���O
	float4 texcolor = tex.Sample(smp,input.uv);

	//return texcolor;

	// ����x
	const float shininess = 4.0f;
	// ���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - input.svpos.xyz);

	// �����ˌ�
	float3 ambient = m_ambient;

	// �V�F�[�f�B���O�ɂ��F
	float4 shadecolor;

	// ���s����
	for (int i = 0; i < DIRLIGHT_NUM; i++) {
		if (dirLights[i].active) {
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);
			// ���ˌ��x�N�g��
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// �g�U���ˌ�
			float3 diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			shadecolor.rgb += (ambient + diffuse + specular) * dirLights[i].lightcolor;
			shadecolor.a = m_alpha;
		}
	}

	// �_����
	for (int i = 0; i < POINTLIGHT_NUM; i++) {
		if (pointLights[i].activ) {
			// ���C�g�̃x�N�g��
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			// �x�N�g���̒���
			float d = length(lightv);
			// ���K�����A�P�ʃx�N�g���ɂ���
			lightv = normalize(lightv);
			// ���������W��
			float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(lightv, input.normal);
			// ���ˌ��x�N�g��
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// �g�U���ˌ�
			float diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
			// ���ׂĉ��Z
			shadecolor.rgb += atten*(ambient + diffuse + specular) * pointLights[i].lightcolor;
			shadecolor.a = m_alpha;
		}
	}

	output.target0 = shadecolor * texcolor * color;
	//output.target0 = float4(1, 0, 0, 1);
	output.target1 = output.target0;

	return output;
}
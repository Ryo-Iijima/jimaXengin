cbuffer cbuff0 : register(b0)
{
	float4 color;		// �F
	matrix viewproj;	// �r���[�v���W�F�N�V�����s��
	matrix world;		// ���[���h�s��
	float3 cameraPos;	// �J�������W
};

// ���s�����̐�
static const int DIRLIGHT_NUM = 3;

struct DirLight
{
	float3 lightv;    // ���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightcolor;    // ���C�g�̐F(RGB)
	uint active;
};

// �_�����̐�
static const int POINTLIGHT_NUM = 3;

struct PointLight
{
	float3 lightpos;	// ���C�g���W
	float3 lightcolor;	// ���C�g�̐F
	float3 lightatten;	// ���������W��
	uint activ;
};

// �X�|�b�g���C�g�̐�
static const int SPOTLIGHT_NUM = 3;

struct SpotLight
{
	float3 lightv;				// ���������̋t�x�N�g��
	float3 lightpos;			// ���C�g���W
	float3 lightcolor;			// ���C�g�̐F
	float3 lightatten;			// ���������W��
	float2 lightfactoranglecos;	// �����p�x�̃R�T�C��
	uint activ;
};

// �ۉe�̐�
static const int CIRCLESHADOW_NUM = 1;

struct CircleShadow
{
	float3 dir;					// ���e�����̋t�x�N�g��
	float3 casterpos;			// �L���X�^�[���W
	float distancecasterlight;	// �L���X�^�[�ƃ��C�g�̋���
	float3 atten;				// ���������W��
	float2 factoranglecos;		// �����p�x�̃R�T�C��
	uint activ;
};

cbuffer cbuff2 : register(b2)
{
	float3 ambientColor;
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

cbuffer cbuff4 : register(b4)
{
	float3 m_ambient;	// �A���r�G���g�W��
	float3 m_diffuse;	// �f�B�t���[�Y�W��
	float3 m_specular;	// �X�y�L�����[�W��
	float m_alpha;	// �A���t�@
}

// �{�[���̍ő吔
static const int MAX_BONES = 32;
cbuffer skinning:register(b3)
{
	matrix matSkinning[MAX_BONES];
}

// ���_�o�b�t�@�̓���
struct VSInput
{
	float4 pos	  : POSITION;
	float3 normal : NORMAL;				// �@��
	float2 uv	  : TEXCOORD;			// �e�N�X�`�����W
	uint4 boneIndices  : BONEINDICES;	// �{�[���̔ԍ�
	float4 boneWeight : BONEWEIGHTS;	// �{�[���̃X�L���E�F�C�g
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos  : SV_POSITION;	// �V�X�e���p�̒��_���W
	float4 worldpos : POSITION; // ���[���h���W
	float3 normal : NORMAL;			// �@���x�N�g��
	float2 uv	  : TEXCOORD;		// uv�l
};


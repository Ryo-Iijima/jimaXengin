#include "Object3d.h"
#include <d3dcompiler.h>
#include "../WinApp.h"
#pragma comment (lib,"d3dcompiler.lib")

#include "../general/Input.h"
#include "FbxLoader.h"
#include "../DirectXCommon.h"

using namespace Microsoft::WRL;
using namespace DirectX;

// static�ϐ��̎���
DirectXCommon* Object3d::dxCommon = nullptr;;
ID3D12Device* Object3d::_dev = nullptr;
ComPtr<ID3D12RootSignature> Object3d::rootSignature;
ComPtr<ID3D12PipelineState> Object3d::piplineState;



void Object3d::StaticInitialize(DirectXCommon* dxcommon, WinApp* winapp)
{
	Object3d::dxCommon = dxcommon;
}

void Object3d::CreateGraphicsPipline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	assert(_dev);

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shader/FBX/FBXVS.hlsl",    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) 
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shader/FBX/FBXPS.hlsl",    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) 
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		// xy���W
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// �@���x�N�g��
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// uv���W
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// �e�����󂯂�{�[���ԍ�
		{
			"BONEINDICES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		// �{�[���̃X�L���E�F�C�g
		{
			"BONEWEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;    // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[3];
	// CBV�i���W�ϊ��s��p�j
	rootparams[transform].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	// SRV�i�e�N�X�`���j
	rootparams[texture].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	// CBV�i�X�L�j���O�p�j
	rootparams[skin].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = _dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0); 
	}

	gpipeline.pRootSignature = rootSignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(piplineState.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{ 
		assert(0); 
	}
}

void Object3d::Initialize()
{
	HRESULT result;
	// �萔�o�b�t�@�̐���
	result = _dev->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(TransformData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBufferTranceform.GetAddressOf())
	);
	assert(!result);

	result = _dev->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(SkinData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBufferSkin.GetAddressOf())
	);
	assert(!result);

	eye = { 0, 0, -20 };
	target = { 0, 0, 0 };
	up = { 0, 1, 0 };

	camera = new Camera();
	// ���s�ړ��s��̌v�Z
	camera->SetViewMatrix(eye, target, up);
	// �v���W�F�N�V�����s��̌v�Z
	camera->SetProjectionMatrix(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);

	// 1�t���[�����̎��Ԃ�60FPS�ɐݒ�
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);
}

void Object3d::Update()
{
	////////////////////////////
	/// �g�����X�t�H�[���̓]��
	////////////////////////////
	CameraMove();

	XMMATRIX matScale, matRot, matTrans;

	// �s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	const auto matView = camera->GetMatView();
	const auto matProje = camera->GetMatProjection();

	const XMMATRIX matViewProjection = matView * matProje;
	const XMMATRIX modelTransform = model->GetModelTransform();
	const XMFLOAT3 cameraPos = camera->GetEye();

	HRESULT result;
	// �萔�o�b�t�@�ɓ]��
	TransformData* constMap = nullptr;
	result = constBufferTranceform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->viewproj = matViewProjection;
		//constMap->world = matWorld * modelTransform;
		constMap->world = matWorld;
		constMap->cameraPos = cameraPos;
		constBufferTranceform->Unmap(0, nullptr);
	}

	////////////////////////////
	/// �X�L���̓]��
	////////////////////////////

	// �A�j���[�V����
	if (isPlay)
	{
		// 1�t���[���i�߂�
		currentTime += frameTime;
		// �Ō�܂ōĐ�������擪�ɖ߂�
		if (currentTime > endTime)
		{
			currentTime = startTime;
		}
	}
	else
	{
		PlayAnimation();
	}

	// �{�[���z��
	std::vector<Model::Bone>& bones = model->GetBonse();

	// �萔�o�b�t�@�փf�[�^�]��
	SkinData* constMapSkin = nullptr;
	result = constBufferSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++)
	{
		// ���̎p���s��
		XMMATRIX matCurrentPos;
		// ���̎p���s����擾
		FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		// XMMATRIX�ɕϊ�
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPos, fbxCurrentPose);
		// �������ăX�L�j���O�s���
		constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPos;
	}
	constBufferSkin->Unmap(0, nullptr);
}

//void Object3d::Draw(ID3D12GraphicsCommandList* cmdList)
void Object3d::Draw()
{
	// ���f�����Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr)
	{
		return;
	}
	
	Object3d::dxCommon->GetCommandList()->SetPipelineState(piplineState.Get());
	Object3d::dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	Object3d::dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Object3d::dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(transform, constBufferTranceform->GetGPUVirtualAddress());
	Object3d::dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(skin, constBufferSkin->GetGPUVirtualAddress());

	model->Draw(Object3d::dxCommon->GetCommandList());

	//cmdList->SetPipelineState(piplineState.Get());
	//cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//cmdList->SetGraphicsRootConstantBufferView(transform, constBufferTranceform->GetGPUVirtualAddress());
	//cmdList->SetGraphicsRootConstantBufferView(skin, constBufferSkin->GetGPUVirtualAddress());

	//model->Draw(cmdList);
}

void Object3d::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();
	// 0�Ԃ̃A�j���[�V�����擾
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	// �A�j���[�V�����̖��O�擾
	const char* animstackname = animstack->GetName();
	// �A�j���[�V�����̎��ԏ��
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	// �J�n���Ԏ擾
	startTime = takeinfo->mLocalTimeSpan.GetStart();
	// �I�����Ԏ擾
	endTime = takeinfo->mLocalTimeSpan.GetStop();
	// �J�n���Ԃɍ��킹��
	currentTime = startTime;
	// �Đ�����Ԃɂ���
	isPlay = true;
}

void Object3d::CameraMove()
{
	//Vector2 currentMousePos = input->GetCurrentMousePos();
	//printf("mousePos:%f \n", currentMousePos.x);
	float moveAcc = 1;
	// ���N���b�N�z�[���h��
	if (input->MouseButtonPress(0))
	{
		// ALT�����Ȃ��瓮������target�Œ��eye�𓮂���
		if (input->KeyPress(DIK_LMENU))
		{
			// �E
			if (input->GetPrevMousePos().x < input->GetCurrentMousePos().x)
			{
				eye.x += moveAcc;
			}
			// ��
			if (input->GetPrevMousePos().x > input->GetCurrentMousePos().x)
			{
				eye.x -= moveAcc;
			}

			// ��
			if (input->GetPrevMousePos().y < input->GetCurrentMousePos().y)
			{
				eye.y += moveAcc;
			}
			// ��
			if (input->GetPrevMousePos().y > input->GetCurrentMousePos().y)
			{
				eye.y -= moveAcc;
			}
		}
		// eye�Œ��target�𓮂���
		else
		{
			// �E
			if (input->GetPrevMousePos().x < input->GetCurrentMousePos().x)
			{
				target.x += moveAcc;
			}
			// ��
			if (input->GetPrevMousePos().x > input->GetCurrentMousePos().x)
			{
				target.x -= moveAcc;
			}

			// ��
			if (input->GetPrevMousePos().y < input->GetCurrentMousePos().y)
			{
				target.y += moveAcc;
			}
			// ��
			if (input->GetPrevMousePos().y > input->GetCurrentMousePos().y)
			{
				target.y -= moveAcc;
			}
		}
	}

	if (input->MouseWheelMove()<0)
	{
		target.z -= moveAcc;
		eye.z -= moveAcc;
	}
	else if (input->MouseWheelMove() > 0)
	{
		target.z += moveAcc;
		eye.z += moveAcc;
	}

	camera->SetViewMatrix(eye, target, up);
}

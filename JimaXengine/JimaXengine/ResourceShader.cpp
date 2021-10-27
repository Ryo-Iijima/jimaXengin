#include "ResourceShader.h"
#include "general/General.h"
#include "general/ConsoleOutput.h"

using namespace std;
using namespace Microsoft::WRL;

string ResourceShader::shaderpass = "Resources/shader/";
unordered_map<string, ComPtr<ID3DBlob>> ResourceShader::shaders;

ResourceShader::ResourceShader()
{
}

ResourceShader::~ResourceShader()
{
}

HRESULT ResourceShader::CompileShader(const std::string& filename, const std::string& entrypoint, const std::string& shadermodel)
{
	HRESULT result;
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	result = D3DCompileFromFile(
		General::StringToWString(ResourceShader::shaderpass + filename).c_str(), //シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
		(LPCSTR)entrypoint.c_str(), //エントリーポイント名
		(LPCSTR)shadermodel.c_str(), //シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用設定
		0,
		&shaderBlob,
		&errorBlob);

	if (FAILED(result))
	{
		/*
		//errorblobからエラー内容をstring型にコピー
		string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";

		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
		*/

#ifdef _DEBUG
		ConsoleOutputColor(COLOR_RED);
		printf("%s%s compile failed\n", shaderpass.c_str(), filename.c_str());
		ConsoleOutputColor();
#endif // _DEBUG
		return result;
	}
	else
	{
#ifdef _DEBUG
		ConsoleOutputColor(COLOR_GREEN);
		printf("%s%s compile successed\n", shaderpass.c_str(), filename.c_str());
		ConsoleOutputColor();
#endif // _DEBUG
	}

	shaders.emplace(filename, shaderBlob);

	return result;
}
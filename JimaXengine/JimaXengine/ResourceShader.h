#pragma once
#include <d3dcompiler.h>
#include <unordered_map>
#include <wrl.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace JimaXengine
{
	class ResourceShader
	{
	private:
		ResourceShader();
		static std::string shaderpass;
		static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> shaders;

	public:
		~ResourceShader();
		static HRESULT CompileShader(const std::string& filename, const std::string& entrypoint, const std::string& shadermodel);
		static Microsoft::WRL::ComPtr<ID3DBlob> GetShaders(const std::string& filename) { return shaders[filename]; }
	};
}
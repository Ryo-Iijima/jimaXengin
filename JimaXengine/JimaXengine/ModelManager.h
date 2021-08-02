#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <DirectXMath.h>
#include "3d/Model.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

class DirectXCommon;
class ModelManager
{
private:
	static DirectXCommon* dxCommon;
	static std::string modelpass;
	static std::unordered_map<std::string, Model> models;

	ModelManager();
	static void DebugLog(const std::string& filename, bool isSuccess, bool isLoadModel);

	static HRESULT CreateBuffers(Model* model);

public:
	static void Initialize(DirectXCommon* dxCommon);
	static void LoadObjFile(const std::string& filename, bool smoothflag = false);

	static Model* GetObjMesh(const std::string& filename);
};


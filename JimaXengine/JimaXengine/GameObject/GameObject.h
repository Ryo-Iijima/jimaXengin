#pragma once
#include "../general/Input.h"
#include "../Application.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../CollisionPrimitive.h"
#include "../Collision.h"
#include "../Object2d.h"
#include "../3d/Object3d.h"
#include "../Camera.h"
#include "../Particle/ParticleEmitter.h"
//#include "FbxObj.h"

class GameObjectManager;

class GameObject
{
public:
	//
	enum RENDER_TYPE
	{
		RENDER_TYPE_BACKGROUND = 1000,
		RENDER_TYPE_GEOMETRY = 2000,
		RENDER_TYPE_ALPHA_TEST = 2450,
		RENDER_TYPE_TRANSPARENT = 3000,
		RENDER_TYPE_OVERLAY = 4000,
	};
	// オブジェクトタイプ
	enum TYPE
	{
		NONE,
		PLAYER,
	};

	// 純粋仮想関数
	GameObject();
	virtual ~GameObject();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	// 仮想関数
	virtual void Dead();
	virtual void Delete();
	virtual TYPE GetType();
	virtual RENDER_TYPE GetRenderType();
	virtual void DrawImGui();
	virtual void SilhouetteDraw();
	
	// Getter
	Vector3 GetPos() { return pos; }
	Vector3 GetScale() { return scale; }
	Vector3 GetRotation() { return rotation; }
	
	bool GetIsDead() { return isDead; }
	bool GetIsDelete() { return isDelete; }

	Sphere GetSphereCol() { return sphereCol; }
	Plane GetPlaneCol() { return planeCol; }
	Triangle GetTriangleCol() { return triangleCol; }
	Lay GetLayCol() { return layCol; }
	AABB3D GetAABB3DCol() { return aabb3dCol; }
	// Setter
	void SetGameObjectManager(GameObjectManager* oManager) { this->oManager = oManager; }

	void InitializeParticleEmitter(ParticleManager* pManager);
protected:
	Camera* pCamera;
	GameObjectManager* oManager;
	ParticleEmitter* pEmitter;

	// 位置・移動
	Vector3 pos;
	Vector3 vel;
	Vector3 acc;
	float speed;
	// 大きさ
	Vector3 scale;
	float size;
	float halfsize;
	// 回転
	Vector3 rotation;
	// 生死・削除フラグ
	bool isDead;
	bool isDelete;
	// 判定用コリジョン
	Sphere sphereCol;
	Plane planeCol;
	Triangle triangleCol;
	Lay layCol;
	AABB3D aabb3dCol;

	RENDER_TYPE renderType = RENDER_TYPE_GEOMETRY;
};
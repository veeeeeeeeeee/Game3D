#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "DirectXTK/SimpleMath.h"
#include "Direct3D.h"
#include "Mesh.h"
#include "Camera.h"

using namespace DirectX::SimpleMath;

class GameObject
{
protected:
	Matrix world;

	Vector3 m_position, m_originalPos;
	float m_rotX, m_rotY, m_rotZ;
	float m_scaleX, m_scaleY, m_scaleZ;

	const float boundary = 34.5f;

	Mesh* m_mesh;

public:
	GameObject();
	GameObject(Mesh* mesh);
	GameObject(Mesh* mesh, Vector3 position);
	GameObject(Mesh* mesh, Vector3 position, float rotX, float rotY, float rotZ);
	GameObject(Mesh* mesh, Vector3 position, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ);
	virtual ~GameObject();

	Vector3 GetPosition() { return m_position; }
	float GetXRotation() { return m_rotX; }
	float GetYRotation() { return m_rotY; }
	float GetZRotation() { return m_rotZ; }
	float GetXScale() { return m_scaleX; }
	float GetYScale() { return m_scaleY; }
	float GetZScale() { return m_scaleZ; }
	Mesh* GetMesh() { return m_mesh; }

	void SetPosition(Vector3 pos) { m_position = pos; }
	void SetXRotation(float xRot) { m_rotX = xRot; }
	void SetYRotation(float yRot) { m_rotY = yRot; }
	void SetZRotation(float zRot) { m_rotZ = zRot; }
	void SetXScale(float xScale) { m_scaleX = xScale; }
	void SetYScale(float yScale) { m_scaleY = yScale; }
	void SetZScale(float zScale) { m_scaleZ = zScale; }
	
	float getBoundary();
	virtual void Update(float timestep) = 0;
	virtual void Render(Direct3D* direct3D, Camera* currentCamera);

	friend class Game;
};

#endif
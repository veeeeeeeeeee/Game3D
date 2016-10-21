#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include "Collisions.h"

class Projectile : public GameObject {
private:
	bool exist;

	const float m_moveSpeed = 40.0f;
	Vector3 m_dir;
	bool m_type;

public:
	Projectile (Mesh* mesh, Vector3 position, Vector3 dir, float rotX, float rotY, bool type);
	~Projectile();

	void Update (float timestep);

	void destruct();
	bool isDestroyed();
	bool getType();
};

#endif
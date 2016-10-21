#include "Projectile.h"

Projectile::Projectile (Mesh* mesh, Vector3 position, Vector3 dir, float rotX, float rotY, bool type) :
	GameObject (mesh, position) {
	
	m_dir = dir;
	m_type = type;
	exist = true;

	m_rotX = rotX;
	m_rotY = rotY;

}

Projectile::~Projectile() {
}

void Projectile::Update (float timestep) {
	m_position += (m_dir * m_moveSpeed * timestep);

	if (m_position.x > boundary || m_position.y > boundary || m_position.z > boundary ||
		m_position.x < -boundary || m_position.y < 0 || m_position.z < -boundary)
		
		exist = false;
}

void Projectile::destruct() {
	exist = false;
}

bool Projectile::isDestroyed() {
	return !exist;
}

bool Projectile::getType() {
	return m_type;
}
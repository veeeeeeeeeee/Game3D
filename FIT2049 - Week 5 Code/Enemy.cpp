#include "Enemy.h"

Enemy::Enemy (Mesh* mesh, Vector3 position, EnemyTypes type) :
	GameObject (mesh, position) {
	
	m_maxHP = 100;
	m_HP = m_maxHP;

	m_type = type;

	m_bSphere.SetCenter (m_position);
	m_bSphere.SetRadius (BSDEFAULT);
	time = 0;
	X = m_position.x;
	Y = m_position.y;
	Z = m_position.z;

	m_translation1 = m_position;
	m_translation2 = Vector3::Zero;
	hitTime = 0;
}

Enemy::~Enemy() {
}

void oscilliate (Vector3& v, float degree) {
	if (!(rand() % 3)) {
		int sign = 1;
		int intDeg = (int)degree*10;

		if (rand() % 2)
			sign = 1;
		else sign = -1;
		float deviationX = sign * (rand() % intDeg + 1) / 10;
	
		if (rand() % 2)
			sign = 1;
		else sign = -1;
		float deviationY = sign * (rand() % intDeg + 1) / 10;
	
		if (rand() % 2)
			sign = 1;
		else sign = -1;
		float deviationZ = sign * (rand() % intDeg + 1) / 10;

		v += Vector3 (deviationX, deviationY, deviationZ);
	}
}

void Enemy::Update (float timestep) {
	// m_rotY += m_spinSpeed * timestep;

	// has to move + shoot?
	// enemy and player shooting each other must be implemented inside Game
	// can use (if m_type) to make different movement function
	
	time += timestep;
	
	if (m_type == blue) { // harmonic oscilliation?
		m_translation1.x = m_position.x = X * cos (DirectX::XM_PI/4*time);
		m_bSphere.SetCenter (m_position);
	}

	if (m_type == orange) { // y twist
		m_rotY += (m_rotSpeed * timestep);
		m_translation1.y = Y + 0.8f + Y * cos (DirectX::XM_PI/5*time);
		
		Matrix yRotation = Matrix::CreateRotationY (m_rotY);
		m_position = Vector3::TransformNormal (m_translation1, yRotation);
		m_bSphere.SetCenter (m_position);
	}

	if (m_type == red) { // z circle
		// assume moving it back to x = 0, y = 0
		// translate to (x, y)
		// rot += value
		m_rotZ += (m_rotSpeed * timestep);
		m_translation2.y = 0.8f + Vector3::Distance (m_originalPos, Vector3 (0, 0, Z));
		m_translation2.x = X;

		Matrix zRotation = Matrix::CreateRotationZ (m_rotZ);
		m_position = Vector3::TransformNormal (m_translation1, zRotation) + m_translation2;
		m_bSphere.SetCenter (m_position);
	}

	if (m_type == green) { // x circle
		// same as z
		m_rotX += (m_rotSpeed * timestep);
		m_translation2.y = 0.8f + Vector3::Distance (m_originalPos, Vector3 (X, 0, 0));
		m_translation2.z = Z;

		Matrix xRotation = Matrix::CreateRotationX (m_rotX);
		m_position = Vector3::TransformNormal (m_translation1, xRotation) + m_translation2;
		m_bSphere.SetCenter (m_position);
	}

	if (m_type == purple) { // oscilliate
		// deviation = 5
		// pos += or -= deviation

		oscilliate (m_position, 1.0f);
		if (Vector3::Distance (m_position, m_originalPos) > 1.0f)
			m_position = m_originalPos;
		
		m_translation1 = m_position;
		m_bSphere.SetCenter (m_position);
	}
	
	
	if (time < hitTime + impactTime*timestep && hitTime != 0) {
		m_mesh->changeToHitTex();
	}
	else if (time > hitTime + impactTime*timestep*2 || hitTime == 0) {
		m_mesh->changeToOriTex();
	}
}

void Enemy::enemyRender (Direct3D* direct3D, Camera* currentCam) {
	Matrix world = Matrix::CreateTranslation (m_translation1) * Matrix::CreateFromYawPitchRoll (m_rotY, m_rotX, m_rotZ) * Matrix::CreateTranslation (m_translation2);
	m_mesh->Render (direct3D, world, currentCam);
}

Vector3 Enemy::attack (Player* player) {
	Vector3 bulletDir = player->GetPosition() + Vector3 (0, 1.5f, 0) - m_bSphere.GetCenter();
	oscilliate (bulletDir, 1.0f);
	bulletDir.Normalize();

	return bulletDir;
}

float Enemy::getHP() {
	return m_HP;
}

float Enemy::getMaxHP() {
	return m_maxHP;
}

void Enemy::getShot() {
	if (m_HP > 20)
		m_HP -= 20;
	else m_HP = 0;

	hitTime = time;
}

bool Enemy::isShooting() {
	//return false;
	int t = rand() % 400;
	return t == 0? true : false;
}

bool Enemy::isDestroyed() {
	return m_HP == 0? true : false;
}

EnemyTypes Enemy::getType() {
	return m_type;
}

CBoundingSphere Enemy::getBSphere() {
	return m_bSphere;
}

void Enemy::setColour (Color colour) {
	m_colour = colour;
}

Color Enemy::getColour() {
	return m_colour;
}
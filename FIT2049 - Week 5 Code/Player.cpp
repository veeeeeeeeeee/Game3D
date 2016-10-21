#include "Player.h"

#include <string>
#define DEBUG(val) MessageBox(NULL, std::to_string(val).c_str(), "", MB_OK)
#define VDEBUG(v3) MessageBox(NULL, (std::to_string(v3.x)+" "+std::to_string(v3.y)+" "+std::to_string(v3.z)).c_str(), "", MB_OK)

Player::Player(Camera* cam, InputController* input) : GameObject()
{
	m_cam = cam;
	m_input = input;
	m_moveSpeed = 10.0f;
	m_rotationSpeed = 1.0f;
	m_lookAtXRotation = 0;

	m_maxHP = 100;
	m_HP = m_maxHP;
	m_score = 0;
	m_shooting = false;

	m_bSphere.SetCenter (m_position);
	m_bSphere.SetRadius (BSDEFAULT);
}

void Player::Update(float timestep)
{
	m_rotY -= (m_input->GetMouseXDelta() * m_rotationSpeed) * timestep;

	float updated_xRotation = m_lookAtXRotation + (m_input->GetMouseYDelta() * m_rotationSpeed) * timestep;
	if (updated_xRotation < DirectX::XM_PI/2.3 && updated_xRotation > -DirectX::XM_PI/2.3)	
		m_lookAtXRotation = updated_xRotation;

	Vector3 forward (0, 0, 1);
	Vector3 left (1, 0, 0);

	Matrix yRotation = Matrix::CreateRotationY (m_rotY);
	forward = Vector3::TransformNormal (forward, yRotation);
	left = Vector3::TransformNormal (left, yRotation);

	Vector3 tmp_pos = m_position;
	m_walking = false;
	if (m_input->GetKeyDown('W')) {
		tmp_pos += (forward * m_moveSpeed) * timestep;
		m_walking = true;
	}
	if (m_input->GetKeyDown('S')) {
		tmp_pos -= (forward * m_moveSpeed) * timestep;
		m_walking = true;
	}
	if (m_input->GetKeyDown('A')) {
		tmp_pos += (left * m_moveSpeed) * timestep;
		m_walking = true;
	}
	if (m_input->GetKeyDown('D')) {
		tmp_pos -= (left * m_moveSpeed) * timestep;
		m_walking = true;
	}
	if (tmp_pos.x > boundary)
		tmp_pos.x = boundary;
	if (tmp_pos.x < -boundary)
		tmp_pos.x = -boundary;
	if (tmp_pos.z > boundary)
		tmp_pos.z = boundary;
	if (tmp_pos.z < -boundary)
		tmp_pos.z = -boundary;

	m_position = tmp_pos;

	Matrix xRotation = Matrix::CreateRotationX (m_lookAtXRotation);
	Matrix lookAtRotation = xRotation * yRotation;
	Vector3 camPos = m_position + Vector3 (0, 1.5f, 0);
	
	m_lookAt = Vector3 (0, 1.5f, 10);
	m_lookAt = Vector3::Transform (m_lookAt, lookAtRotation);
	m_lookAt += m_position;

	m_cam->SetPosition (camPos);
	m_cam->SetLookAt (m_lookAt);

	m_shooting = false;
	if (m_input->GetMouseUp (LEFT_MOUSE)) {
		m_shooting = true;
	}
	
	Vector3 tmp_sCenter = m_position;
	tmp_sCenter.y = (m_position.y + camPos.y)/2;
	m_bSphere.SetCenter (tmp_sCenter);

	if (m_input->GetKeyUp ('N')) {
		VDEBUG (m_lookAt);
	}
}

float Player::getHP() {
	return m_HP;
}

float Player::getMaxHP() {
	return m_maxHP;
}

void Player::getShot() {
	if (m_HP > 5)
		m_HP -= 5;
	else m_HP = 0;
}

bool Player::isDead() {
	return m_HP == 0? true : false;
}

int Player::getScore() {
	return m_score;
}

bool Player::isShooting() {
	return m_shooting;
}

bool Player::isWalking() {
	return m_walking;
}

Vector3 Player::getLookAt() {
	return m_lookAt;
}

CBoundingSphere Player::getBSphere() {
	return m_bSphere;
}

void Player::hit() {
	m_score += 15;
}

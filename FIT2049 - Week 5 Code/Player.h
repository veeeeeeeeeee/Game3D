#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "Camera.h"
#include "Collisions.h"
#include "InputController.h"

class Player : public GameObject
{
private:
	Camera* m_cam;
	InputController* m_input;
	
	float m_moveSpeed;
	float m_rotationSpeed;
	float m_lookAtXRotation;

	Vector3 m_lookAt;

	float m_maxHP, m_HP;
	int m_score;

	bool m_shooting;
	bool m_walking;

	const float BSDEFAULT = 1.0f;
	CBoundingSphere m_bSphere;

public:
	Player(Camera* cam, InputController* input);

	void Update(float timestep);

	float getHP();
	float getMaxHP();
	int getScore();
	Vector3 getLookAt();
	CBoundingSphere getBSphere();

	bool isShooting();
	bool isWalking();
	void shoot();
	void hit();
	void getShot();
	bool isDead();
};

#endif
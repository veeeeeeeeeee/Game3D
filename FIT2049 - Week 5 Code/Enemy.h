#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include "Collisions.h"
#include "Player.h"
#include "Texture.h"
#include "DirectXTK\SimpleMath.h"

#include <map>
#include <string>

enum EnemyTypes {
	blue, orange, red, green, purple
};

const std::map <EnemyTypes, std::string> eTypeName {
	{blue, "blue"},
	{orange, "orange"},
	{red, "red"},
	{green, "green"},
	{purple, "purple"}
};

const std::map <std::string, EnemyTypes> eNameType {
	{"blue", blue},
	{"orange", orange},
	{"red", red},
	{"green", green},
	{"purple", purple}
};

const std::map <EnemyTypes, int> eTypeIndex {
	{blue, 0},
	{orange, 1},
	{red, 2},
	{green, 3},
	{purple, 4}
};

const EnemyTypes eIndexType[5] = {blue, orange, red, green, purple};

const std::map <std::string, int> eNameIndex {
	{"blue", 0},
	{"orange", 1},
	{"red", 2},
	{"green", 3},
	{"purple", 4}
};

const std::string eIndexName[5] = {"blue", "orange", "red", "green", "purple"};

const Color BLUE = Color (0.38f, 0.83f, 0.89f, 1.0f);
const Color ORANGE = Color (1.0f, 0.84f, 0.51f, 1.0f);
const Color RED = Color (0.89f, 0.34f, 0.34f, 1.0f);
const Color GREEN = Color (0.49f, 0.82f, 0.45, 1.0f);
const Color PURPLE = Color (0.7f, 0.49f, 0.91f, 1.0f);

const Color eIndexColour[5] = {BLUE, ORANGE, RED, GREEN, PURPLE};

class Enemy : public GameObject {
private:
	Vector3 m_translation2;
	Vector3 m_translation1;

	float m_maxHP, m_HP;
	float m_rotSpeed = 1.0f;
	float m_moveSpeed = 5.0f;
	float time, X, Y, Z;
	const float impactTime = 50;

	EnemyTypes m_type;

	const float BSDEFAULT = 2.0f;
	CBoundingSphere m_bSphere;

	Color m_colour;

public:
	Enemy (Mesh* mesh, Vector3 position, EnemyTypes type);
	~Enemy();

	void Update (float timestep);
	void enemyRender (Direct3D* direct3D, Camera* currentCam);

	float getHP();
	float getMaxHP();
	CBoundingSphere getBSphere();

	Vector3 attack (Player* player);
	void getShot();
	bool isShooting();
	bool isDestroyed();

	EnemyTypes getType();
	float hitTime;

	void setColour (Color colour);
	Color getColour();
};

#endif
#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"
#include "Texture.h"
#include "Mesh.h"
#include "ParticleSystem.h"

#include <list>
#include <vector>
#include <queue>

class ObjectFactory {
protected:
	static void createEnemy (std::list <Enemy*>& enemies, std::vector <Mesh*>& enemyMeshes, Player* player, float boundary);
	static void createPlayerProjectile (std::list <Projectile*>& projectiles, Mesh* pMesh, Player* player);
	static void createEnemyProjectile (std::list <Projectile*>& projectiles, Mesh* pMesh, Enemy* enemy, Player* player);
	static void createExplosion (std::list <ParticleSystem*>& psList, ParticleSystem ps, Texture* pt, Enemy* e, float time);

	friend class Game;
};


#endif
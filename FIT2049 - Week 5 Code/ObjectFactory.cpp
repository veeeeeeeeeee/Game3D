#include "ObjectFactory.h"

void ObjectFactory::createEnemy (std::list <Enemy*>& enemies, std::vector <Mesh*>& enemyMeshes, Player* player, float boundary) {
	int eIndex = rand() % 5;

	Vector3 ePos = Vector3::Zero;

	ePos.x = (float)(rand() % ((int)boundary) - (int)boundary/2);
	ePos.z = (float)(rand() % ((int)boundary) - (int)boundary/2);
	ePos.y = (float)(rand() % ((int)boundary/8) + ((int)boundary/8));

	Mesh* tmp = new Mesh();
	*tmp = *enemyMeshes[eIndex];

	enemies.push_back (new Enemy (tmp, ePos, eIndexType[eIndex]));
	enemies.back()->setColour (eIndexColour[eIndex]);
}

void ObjectFactory::createPlayerProjectile (std::list <Projectile*>& projectiles, Mesh* pMesh, Player* player) {
	Vector3 origin = player->GetPosition() + Vector3 (0, 1.5f, 0);
	Vector3 dir = player->getLookAt() - origin;
	
	dir.Normalize();
	origin += dir;

	float rotX, rotY;
	Vector3 dirZ (0, 0, dir.z);
	Vector3 dirXZ (dir.x, 0, dir.z);

	rotY = acos (dirZ.Dot (dirXZ) / (dirZ.Length() * dirXZ.Length()));
	float tmp = dir.x * dir.z;
	if (tmp <= 0)
		rotY *= -1.0f;
	

	rotX = acos (dirXZ.Length() / dir.Length());
	if (dir.z >= 0)
		rotX *= -1.0f;
	
	if (dir.y < 0)
		rotX *= -1.0f;

	projectiles.push_back (new Projectile (pMesh, origin, dir, rotX, rotY, 0));
}

void ObjectFactory::createEnemyProjectile (std::list <Projectile*>& projectiles, Mesh* pMesh, Enemy* enemy, Player* player) {
	Vector3 origin = enemy->GetPosition();
	Vector3 dir = enemy->attack (player);
	origin += dir;
	
	float rotX, rotY;
	Vector3 dirZ (0, 0, dir.z);
	Vector3 dirXZ (dir.x, 0, dir.z);

	rotY = acos (dirZ.Dot (dirXZ) / (dirZ.Length() * dirXZ.Length()));
	float tmp = dir.x * dir.z;
	if (tmp <= 0)
		rotY *= -1.0f;
	
	rotX = acos (dirXZ.Length() / dir.Length());
	if (dir.z <= 0) {
		rotX *= -1.0f;
	}

	projectiles.push_back (new Projectile (pMesh, origin, dir, rotX, rotY, 1));
}

void ObjectFactory::createExplosion (std::list <ParticleSystem*>& psList, ParticleSystem psTemplate, Texture* pt, Enemy* e, float time) {
	ParticleSystem* ps = new ParticleSystem();
	*ps = psTemplate;
	
	ps->SetTexture (pt);
	ps->SetMaxParticles (4000);
	ps->SetNumberToRelease (100);
	ps->SetReleaseInterval (0.1f);
	ps->SetLifetime (1.0f, 3.5f);
	ps->SetRadius (0.5f);
	ps->SetColour (e->getColour());
	ps->SetPosition (e->GetPosition());
	ps->SetVelocity (Vector3::Zero);
	ps->SetGravity (Vector3(0, -7, 0));
	ps->SetWind (Vector3::Zero);
	ps->UseWind (true);
	ps->SetVelocityVariance (5.0f);
	ps->systemTimeCreated = time;

	psList.push_back (ps);
}
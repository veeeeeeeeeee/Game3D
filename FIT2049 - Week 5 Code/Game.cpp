/*	FIT2049 - Example Code
*	Game.cpp
*	Created by Elliott Wilson - 2015 - Monash Univeristy
*	Implementation of Game.h
*/

#include "Game.h"
#include "DirectXTK/CommonStates.h"

Game::Game()
{
	m_renderer = NULL;
	m_input = NULL;
	m_currentCam = NULL;
	m_shader = NULL;

	time = 0;
	frame = 0;

	m_stateMachine = new StateMachine<GameStates, Game>(this, NO_STATE);
	m_stateMachine->RegisterState(MENU_STATE, &Game::Menu_OnEnter, 
		&Game::Menu_OnUpdate, &Game::Menu_Draw, &Game::Menu_OnExit);

	m_stateMachine->RegisterState(GAMEPLAY_STATE, &Game::Gameplay_OnEnter,
		&Game::Gameplay_OnUpdate, &Game::Gameplay_Draw, &Game::Gameplay_OnExit);

	m_stateMachine->RegisterState(GAMEOVER_STATE, &Game::GameOver_OnEnter,
		&Game::GameOver_OnUpdate, &Game::GameOver_Draw, &Game::GameOver_OnExit);
}

Game::~Game()
{

}

bool Game::Initialise(Direct3D* renderer, InputController* input)
{
	m_renderer = renderer;		//Set our renderer
	m_input = input;			//Set our input controller
	
	m_shader = new Shader();	//Create our shader
	//Initialise the shader, we need to pass in the shader filenames
	if (!m_shader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/PixelShader.ps", NULL))
	{
		return false;
	}

	m_texturedShader = new TexturedShader();
	if (!m_texturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/TexturedPixelShader.ps", NULL))
	{
		return false;
	}

	//Create a new "default" camera
	m_currentCam = new Camera();

	m_lighting = new SceneLighting();
	//m_lighting->setAmbientLight (Color (0.4f, 0.4f, 0.4f, 1.0f));
	/*
	m_phongShader = new PhongShader(m_lighting, m_currentCam);
	if (!m_phongShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/PhongVertex.vs", L"Assets/Shaders/PhongPixel.ps", NULL))
	{
		return false;
	}*/

	//Loading our button texture
	Texture* buttonTexture = new Texture();
	if (!buttonTexture->Initialise(m_renderer, L"Assets/Textures/Button.png"))
	{
		return false;
	}
	
	
	Texture* rampTexture = new Texture();
	if (!rampTexture->Initialise(m_renderer, L"Assets/Textures/ramp.png"))
	{
		return false;
	}
	m_toonShader = new ToonShader(m_lighting, m_currentCam, rampTexture);
	if (!m_toonShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/PhongVertex.vs", L"Assets/Shaders/ToonPixel.ps", NULL))
	{
		return false;
	}
	
	Texture* skyTexture = new Texture();
	if (!skyTexture->Initialise (m_renderer, L"Assets/Textures/skybox.png")) {
		return false;
	}
	Mesh* skyboxMesh = new Mesh();
	if (!skyboxMesh->Initialise (m_renderer, "Assets/Meshes/skybox.obj", Color (1.0f, 1.0f, 1.0f, 1.0f), skyTexture, m_texturedShader)) {
		return false;
	}

	Texture* levelTexture = new Texture();
	if (!levelTexture->Initialise (m_renderer, L"Assets/Textures/level.png")) {
		return false;
	}
	Mesh* levelMesh = new Mesh();
	if (!levelMesh->Initialise (m_renderer, "Assets/Meshes/level.obj", Color (1.0f, 1.0f, 1.0f, 1.0f), levelTexture, m_texturedShader)) {
		return false;
	}

	StaticObject* sky = new StaticObject (skyboxMesh, Vector3::Zero);
	m_staticObjects.push_back (sky);

	StaticObject* level = new StaticObject (levelMesh, Vector3::Zero);
	m_staticObjects.push_back (level);

	pTexture1 = new Texture();
	pTexture2 = new Texture();
	pMesh1 = new Mesh();
	pMesh2 = new Mesh();
	
	if (!pTexture2->Initialise (m_renderer, L"Assets/Textures/projectile1.png")) {
		return false;
	}
	if (!pTexture1->Initialise (m_renderer, L"Assets/Textures/projectile2.png")) {
		return false;
	}

	if (!pMesh1->Initialise (m_renderer, "Assets/Meshes/projectile.obj", Color (1.0f, 1.0f, 1.0f, 1.0f), pTexture1, m_toonShader)) {
		return false;
	}
	if (!pMesh2->Initialise (m_renderer, "Assets/Meshes/projectile.obj", Color (1.0f, 1.0f, 1.0f, 1.0f), pTexture2, m_toonShader)) {
		return false;
	}

	/*
	if (!pTexture1->Initialise (m_renderer, L"Assets/Textures/blast.png")) {
		return false;
	}
	if (!pTexture2->Initialise (m_renderer, L"Assets/Textures/blast.png")) {
		return false;
	}

	if (!pMesh1->Initialise (m_renderer, "Assets/Meshes/blast.obj", Color (1.0f, 1.0f, 1.0f, 1.0f), pTexture1, m_texturedShader)) {
		return false;
	}
	if (!pMesh2->Initialise (m_renderer, "Assets/Meshes/blast.obj", Color (1.0f, 1.0f, 1.0f, 1.0f), pTexture2, m_texturedShader)) {
		return false;
	}
	
	*/

	//Create the sprite batch
	m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());
	//Load our Sprite Font
	m_arial = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-12pt.spritefont");
	m_arial2 = new SpriteFont (m_renderer->GetDevice(), L"Assets/Fonts/Arial-23pt.spritefont");

	//Here we create our button, we pass of the information and objects needed in but the real special part here is the last parameter
	//this parameter is our lambda expression, we are passing functionality into our function here, which is a really cool concept.
	
	/*
	m_button = new Button(128, 64, buttonTexture, L"Hello World!", Vector2(200, 200), m_spriteBatch, m_arial, m_input, [this]
	{
		//I'm calling a method in here but you could do anything you like!
		ButtonMethod() {
			define button method code here.
		}
	});*/

	m_player = new Player (m_currentCam, m_input);

	crosshair = new Texture();
	if (!crosshair->Initialise (m_renderer, L"Assets/Textures/crosshair.png")) {
		return false;
	}

	HB_margin = new Texture();
	if (!HB_margin->Initialise (m_renderer, L"Assets/Textures/HB-margin.png")) {
		return false;
	}
	HB = new Texture();
	if (!HB->Initialise (m_renderer, L"Assets/Textures/HB.png")) {
		return false;
	}

	for (int i=0; i<5; i++) {
		enemyTextures.push_back (new Texture());
		std::wstring tDir = L"Assets/Textures/" + std::wstring (eIndexName[i].begin(), eIndexName[i].end());
		tDir += L".png";

		if (!enemyTextures[i]->Initialise (m_renderer, tDir.c_str())) {
			return false;
		}

		enemyMeshes.push_back (new Mesh());
		std::string mDir = "Assets/Meshes/" + eIndexName[i] + "Enemy.obj";

		if (!enemyMeshes[i]->Initialise (m_renderer, mDir.c_str(), Color (1.0f, 1.0f, 1.0f, 1.0f), enemyTextures[i], m_texturedShader)) {
			return false;
		}
		
		tDir = L"Assets/Textures/" + std::wstring (eIndexName[i].begin(), eIndexName[i].end());
		tDir += L"hit.png";

		Texture* hTexture = new Texture();
		if (!hTexture->Initialise (m_renderer, tDir.c_str())) {
			return false;
		}
		enemyMeshes[i]->setHitTexture (hTexture);
		//enemies.push_back (new Enemy (enemyMeshes[i], Vector3::Zero, eIndexType[i]));
	}
	m_count = 0;
	
	// sounds
	m_soundManager = new SoundManager();
	if (!m_soundManager->Initialise()) {
		return false;
	}
	if (!m_soundManager->Load ("Assets/Sounds/pew.wav")) {
		return false;
	}
	if (!m_soundManager->Load ("Assets/Sounds/footsteps.wav")) {
		return false;
	}
	if (!m_soundManager->Load ("Assets/Sounds/explode.wav")) {
		return false;
	}

	pew = m_soundManager->GetSound ("Assets/Sounds/pew.wav");
	footstep = m_soundManager->GetSound ("Assets/Sounds/footsteps.wav");
	explode = m_soundManager->GetSound ("Assets/Sounds/explode.wav");

	// particle system
	if (!ParticleSystem::InitialiseSystem (m_renderer, L"Assets/Shaders/ParticleShader.hlsl")) {
		return false;
	}

	psTemplate = ParticleSystem();
	if (!psTemplate.Initialise(m_renderer))
	{
		return false;
	}

	ptTemplate = new Texture();
	if (!ptTemplate->Initialise(m_renderer, L"Assets/Textures/particle.png"))
	{
		return false;
	}

	m_stateMachine->ChangeState(MENU_STATE);

	return true;
}

void Game::Menu_OnEnter()
{

}
void Game::Menu_OnUpdate(float timestep)
{
	if (m_input->GetKeyUp(VK_RETURN))
	{
		m_stateMachine->ChangeState(GAMEPLAY_STATE);
	}
}
void Game::Menu_Draw()
{
	//When we want to draw 2D objects we need to do it within the Begin and End calls of the Sprite Batch
	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	m_arial2->DrawString (m_spriteBatch, L"Welcome to ASFPS! Press ENTER to play!", Vector2 (200, 200));
	
	m_spriteBatch->End();
}
void Game::Menu_OnExit()
{

}

void Game::Gameplay_OnEnter()
{

}
void Game::Gameplay_OnUpdate(float timestep)
{
	
	if (!m_player->isDead()) {
		m_currentCam->Update(timestep);		//Every update we will update the camera
	
		for (unsigned int i = 0; i < m_staticObjects.size(); i++)
		{
			m_staticObjects[i]->Update (timestep);
		}
		m_player->Update (timestep);

		if (enemies.size() < 4) {
			ObjectFactory::createEnemy (enemies, enemyMeshes, m_player, m_player->getBoundary());
			ObjectFactory::createEnemy (enemies, enemyMeshes, m_player, m_player->getBoundary());
		}

		/* 
		 * if player is shooting
		 * generate a projectile with a direction and origin
		 * projectile update will travel it from ori to dir
		 * projectile has function to destruct
		 * projectile has property whether player or enemy shoots it
		 * -------------
		 * iterate through enemies
		 * if enemy->shooting
		 * bulletDir = enemy->attack (player)
		 * generate projectile with bulletDir and origin
		 * -------------
		 * iterate through projectiles
		 * for each, iterate through enemies or player
		 * check collision
		 * hit -> enemy/player->getshot, destroy projectile
		 * -------------
		 * iterate through enemies
		 * if destroyed -> erase
		 * -------------
		 * if player dead -> end game
		 */

	
		if (m_player->isShooting())
			ObjectFactory::createPlayerProjectile (projectiles, pMesh1, m_player);
		
		// Sound
		if (m_player->isShooting()) {
			pew->Stop();
			pew->Play();
		}
		if (m_player->isWalking())
			footstep->Play();

		eIt = enemies.begin();
		while (eIt!=enemies.end()) {
			if ((*eIt)->isShooting())
				ObjectFactory::createEnemyProjectile (projectiles, pMesh2, (*eIt), m_player);

			eIt++;
		}
		
		pIt = projectiles.begin();
		while (pIt != projectiles.end()) {
			if ((*pIt)->getType() == 0) { // player
				eIt = enemies.begin();
				bool loop = true;
				while (loop && eIt != enemies.end()) {
					if (CheckCollision ((*eIt)->getBSphere(), (*pIt)->GetPosition())) {
						(*eIt)->getShot();
						(*pIt)->destruct();
						m_count ++;
						m_player->hit();
						loop = false;
					}
					else eIt ++;
				}
			}
			else { // enemy
				if (CheckCollision (m_player->getBSphere(), (*pIt)->GetPosition())) {
					(*pIt)->destruct();
					m_player->getShot();
				}
			}
			if (pIt != projectiles.end())
				pIt ++;
		}
		
		// clean projectile got destroyed
		pIt = projectiles.begin();
		while (pIt != projectiles.end()) {
			(*pIt)->Update (timestep);
		
			if ((*pIt)->isDestroyed()) {
				delete (*pIt);
				*pIt = NULL;
				pIt = projectiles.erase (pIt);
			}
			else pIt ++;
		}
		
		// clean enemy got destroyed
		eIt = enemies.begin();
		while (eIt!=enemies.end()) {
			(*eIt)->Update (timestep);
			if ((*eIt)->isDestroyed()) {
				ObjectFactory::createExplosion (psList, psTemplate, ptTemplate, *eIt, time);
				explode->Play();

				delete (*eIt);
				*eIt = NULL;
				eIt = enemies.erase (eIt);
			}
			else eIt++;
		}

		lIt = psList.begin();
		while (lIt!=psList.end()) {
			(*lIt)->Update (timestep);
			lIt++;
		}

		// clean up psList
		/* exception when calling (*lIt)->Release()
		 * (vertexBuffer accessing NULL).
		 * Not sure if not including it is memory-efficient considering
		 * there is a max amount of particles
		 * and particles themselves got released
		 */
		if (!psList.empty()) {
			lIt = psList.begin();
			if (time - (*lIt)->systemTimeCreated >= psLifeTime) {
				//(*lIt)->Release();
				delete (*lIt);
				*lIt = NULL;
				lIt = psList.erase (lIt);
			}
		}
	}
}
void Game::Gameplay_Draw()
{
	for (unsigned int i = 0; i < m_staticObjects.size(); i++)
	{
		m_staticObjects[i]->Render (m_renderer, m_currentCam);
	}

	m_player->Render (m_renderer, m_currentCam);
	for (eIt=enemies.begin(); eIt!=enemies.end(); eIt++) {
		(*eIt)->enemyRender (m_renderer, m_currentCam);
	}

	for (pIt=projectiles.begin(); pIt!=projectiles.end(); pIt++) {
		(*pIt)->Render (m_renderer, m_currentCam);
	}

	ParticleSystem::Begin(m_renderer);
	for (lIt=psList.begin(); lIt!=psList.end(); lIt++) {
		(*lIt)->Render (m_renderer, m_currentCam);
	}
	ParticleSystem::End(m_renderer);
	
	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	m_spriteBatch->Draw (crosshair->GetShaderResourceView(), Vector2 ((float)m_input->getOriMouseX()-18.5f, (float)m_input->getOriMouseY()-18.5f));
	
	std::wstring score = L"Score: ";
	score += std::to_wstring (m_player->getScore());
	m_arial->DrawString (m_spriteBatch, score.c_str(), Vector2 (715.0f, 86.0f));
	
	m_arial->DrawString (m_spriteBatch, L"HP", Vector2 (715.0f, 46.0f));
	m_spriteBatch->Draw (HB_margin->GetShaderResourceView(), Vector2 (750.0f, 40.0f));
	RECT des;
	des.top = 45.0f;
	des.bottom = 65.0f;
	des.left = 755.0f;
	float percentHP = m_player->getHP() / m_player->getMaxHP();
	float HB_bar = 200.0f * percentHP;
	des.right = des.left + HB_bar;
	m_spriteBatch->Draw (HB->GetShaderResourceView(), des, Color (1.0f-percentHP*percentHP*percentHP, percentHP, 0));

	std::wstring d = L"";
	d += std::to_wstring (psList.size());
	
	m_arial->DrawString (m_spriteBatch, d.c_str(), Vector2(100, 100));
	
	if (m_player->isDead()) {
		m_arial2->DrawString (m_spriteBatch, L"GAME OVER", Vector2 (420, 200));
	}
	m_spriteBatch->End();
}

void Game::Gameplay_OnExit()
{

}

void Game::GameOver_OnEnter()
{

}
void Game::GameOver_OnUpdate(float timestep)
{

}
void Game::GameOver_Draw()
{

}
void Game::GameOver_OnExit()
{

}

void Game::Update(float timestep)
{
	m_input->BeginUpdate();				//The input controller needs to be updated every frame
	
	time += timestep;
	frame ++;
	m_stateMachine->Update(timestep);
	
	m_input->EndUpdate();				//We also need to tell the Input Controller when a frame is over
}

void Game::ButtonMethod()
{
	//You could do anything you like here!
	MessageBox(NULL, "This method was called by our button using a lambda expression!", "This is pretty cool!", MB_OK);
}

void Game::Render()
{
	m_renderer->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);						//Begin the scene and clear to black

	m_renderer->SetCurrentShader(NULL);

	m_stateMachine->Draw();
	
	m_renderer->EndScene();												//End the scene and present the frame

}

void Game::Shutdown()
{
	//Clean up everything!
	if (m_currentCam)
	{
		delete m_currentCam;
		m_currentCam = NULL;
	}

	if (m_shader)
	{
		m_shader->Release();
		delete m_shader;
		m_shader = NULL;
	}

	if (m_texturedShader)
	{
		m_texturedShader->Release();
		delete m_texturedShader;
		m_texturedShader = NULL;
	}

	if (m_arial)
	{
		delete m_arial;
		m_arial = NULL;
	}

	if (m_spriteBatch)
	{
		delete m_spriteBatch;
		m_spriteBatch = NULL;
	}
	
	if (crosshair) {
		crosshair->Release();
		delete crosshair;
		crosshair = NULL;
	}

	if (HB_margin) {
		HB_margin->Release();
		delete HB_margin;
		HB_margin = NULL;
	}
	
	if (HB) {
		HB->Release();
		delete HB;
		HB = NULL;
	}

	if (pTexture1) {
		pTexture1->Release();
		delete pTexture1;
		pTexture1 = NULL;
	}
	
	if (pTexture2) {
		pTexture2->Release();
		delete pTexture2;
		pTexture2 = NULL;
	}
	
	if (pMesh1) {
		pMesh1->Release();
		delete pMesh1;
		pMesh1 = NULL;
	}
	
	if (pMesh2) {
		pMesh2->Release();
		delete pMesh2;
		pMesh2 = NULL;
	}

	if (m_soundManager) {
		m_soundManager->Release();
		delete m_soundManager;
		m_soundManager = NULL;
	}
	
	for (int i=0; i<(unsigned)m_staticObjects.size(); i++) {
		if (m_staticObjects[i]) {
			delete m_staticObjects[i];
			m_staticObjects[i] = NULL;
		}
	}
	
	for (int i=0; i<(unsigned)enemyTextures.size(); i++) {
		if (enemyTextures[i]) {
			enemyTextures[i]->Release();
			delete enemyTextures[i];
			enemyTextures[i] = NULL;
		}
	}

	for (int i=0; i<(unsigned)enemyMeshes.size(); i++) {
		if (enemyMeshes[i]) {
			enemyMeshes[i]->Release();
			delete enemyMeshes[i];
			enemyMeshes[i] = NULL;
		}
	}

	for (eIt=enemies.begin(); eIt!=enemies.end(); eIt++) {
		if (*eIt) {
			delete (*eIt);
			(*eIt) = NULL;
		}
	}

	for (pIt=projectiles.begin(); pIt!=projectiles.end(); pIt++) {
		if (*pIt) {
			delete (*pIt);
			(*pIt) = NULL;
		}
	}

	if (m_player) {
		delete m_player;
		m_player = NULL;
	}

	if (ptTemplate) {
		ptTemplate->Release();
		delete ptTemplate;
		ptTemplate = NULL;
	}

	ParticleSystem::ShutdownSystem();
}
/*	FIT2049 - Example Code
*	Game.h
*	Created by Elliott Wilson - 2015 - Monash Univeristy
*	This class is where we create all of the object for a game
*	It contains the overall Update and Render method for the whole game
*	Most of the code you write will be here
*/

#ifndef GAME_H
#define GAME_H

#include "Direct3D.h"
#include "InputController.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "Button.h"
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"
#include "StateMachine.h"
#include "GameObject.h"
#include "SceneLighting.h"
#include "Enemy.h"
#include "Player.h"
#include "StaticObject.h"
#include "Projectile.h"
#include "SpinningObject.h"
#include "TexturedShader.h"
#include "PhongShader.h"
#include "ToonShader.h"
#include "Collisions.h"
#include "ObjectFactory.h"
#include "SoundManager.h"
#include "Sound.h"
#include "ParticleSystem.h"


#include <vector>
#include <list>
#include <queue>

using namespace DirectX;

class Game
{
private:
	enum GameStates
	{
		NO_STATE = 0,
		MENU_STATE,
		GAMEPLAY_STATE,
		GAMEOVER_STATE,
		STATE_COUNT
	};

	StateMachine<GameStates, Game>* m_stateMachine;

	Direct3D* m_renderer;		//Our renderer is passed in when the Game is created
	InputController* m_input;	//
	Camera* m_currentCam;		//Our game has a Camera to see everything with!
	Shader* m_shader;			//We also need a shader to draw things with, later we will probably have multiple shaders
	Shader* m_texturedShader;	//
	Shader* m_phongShader;
	Shader* m_toonShader;
	SceneLighting* m_lighting;
	
	Player* m_player;
	std::vector <StaticObject*> m_staticObjects;
	
	SoundManager* m_soundManager;
	Sound* pew;
	Sound* footstep;
	Sound* explode;
	int frame;

	std::vector <Texture*> enemyTextures;
	std::vector <Mesh*> enemyMeshes;
	std::list <Enemy*> enemies;
	std::list <Enemy*>::iterator eIt;
	
	std::list <Projectile*> projectiles;
	std::list <Projectile*>::iterator pIt;
	Texture* pTexture1;
	Texture* pTexture2;
	Mesh* pMesh1;
	Mesh* pMesh2;
	int m_count;

	std::list <ParticleSystem*> psList;
	std::list <ParticleSystem*>::iterator lIt;
	ParticleSystem psTemplate;
	Texture* ptTemplate;
	const float psLifeTime = 3.5f;
	float time;

	SpriteBatch* m_spriteBatch;	//The sprite batch can be used to draw things in 2D screen space, we can draw textures and text
	SpriteFont* m_arial;		//If we want to draw text then need a font to draw it with, a Sprite Font can do that!
	SpriteFont* m_arial2;

	Texture* crosshair;
	Texture* HB_margin;
	Texture* HB;

	void ButtonMethod();		//This method is just here so the button has something to call, you could call or do anything you want with the button

	void Menu_OnEnter();
	void Menu_OnUpdate(float timestep);
	void Menu_Draw();
	void Menu_OnExit();

	void Gameplay_OnEnter();
	void Gameplay_OnUpdate(float timestep);
	void Gameplay_Draw();
	void Gameplay_OnExit();

	void GameOver_OnEnter();
	void GameOver_OnUpdate(float timestep);
	void GameOver_Draw();
	void GameOver_OnExit();


public:
	Game();		//Constructor
	~Game();	//Destructor

	bool Initialise(Direct3D* renderer, InputController* input);	//The initialise method loads all of the content for the game (meshes, textures, etc.)

	void Update(float timestep);	//The overall Update method for the game. All gameplay logic will be done somewhere within this method
	void Render();					//The overall Render method for the game. Here all of the meshes that need to be drawn will be drawn

	void Shutdown();		//Cleanup everything we initialised
};

#endif
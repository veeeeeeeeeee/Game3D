#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <d3dcompiler.h>
#include "Direct3D.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

#define MAX_BUFFER_SIZE 4096

class ParticleSystem
{
private:
	struct Particle
	{
		Vector3 position;
		Vector3 velocity;
		float timeCreated;
		float lifetime;

		Particle* next;
	};
	
	struct ParticleVertex
	{
		Vector3 position;	
	};
	struct ParticleConstantsBuffer
	{
		Vector3 cameraPosition;
		float particleRadius;
		Matrix worldViewProjection;
		Color particleColour;
	};
	
	static ID3D11VertexShader* s_vertexShader;
	static ID3D11PixelShader* s_pixelShader;
	static ID3D11GeometryShader* s_geometryShader;
	static ID3D11InputLayout* s_inputLayout;
	static ID3D11Buffer* s_constantsBuffer;
	static ID3D11SamplerState* s_textureSampler;
	
	ID3D11Buffer* m_vertexBuffer;

	Particle* m_activeParticleList;
	Particle* m_freeParticleList;

	int m_activeCount;
	float m_currentTime;
	float m_lastUpdate;

	float m_particleRadius;
	Texture* m_particleTexture;
	Color m_particleColour;

	int m_maxParticles;
	int m_numberToRelease;
	float m_releaseInterval;
	float m_particleLifetimeMin;
	float m_particleLifetimeMax;

	Vector3 m_systemPosition;
	Vector3 m_systemVelocity;
	Vector3 m_systemGravity;
	Vector3 m_systemWind;
	bool m_useWind;
	float m_velocityVariance;

	static bool InitialiseShaders(Direct3D* renderer, LPCWSTR shaderFilename);
	static bool InitialiseBuffers(Direct3D* renderer);
	static bool InitialiseSamplers(Direct3D* renderer);

	bool bursted;

public:
	ParticleSystem();
	~ParticleSystem();

	void SetTexture(Texture* texture) { m_particleTexture = texture; }

	void SetMaxParticles(int max) { m_maxParticles = max; }
	void SetNumberToRelease(int release) { m_numberToRelease = release; }
	void SetReleaseInterval(float interval) { m_releaseInterval = interval; }
	void SetLifetime(float min, float max) { m_particleLifetimeMin = min; m_particleLifetimeMax = max; }
	void SetRadius(float radius) { m_particleRadius = radius; }
	void SetColour(Color colour) { m_particleColour = colour; }
	void SetPosition(Vector3 position) { m_systemPosition = position; }
	void SetVelocity(Vector3 velocity) { m_systemVelocity = velocity; }
	void SetGravity(Vector3 gravity) { m_systemGravity = gravity; }
	void SetWind(Vector3 wind) { m_systemWind = wind; }
	void UseWind(bool wind) { m_useWind = wind; }
	void SetVelocityVariance(float variance) { m_velocityVariance = variance; }

	bool Initialise(Direct3D* renderer);
	void Update(float timestep);
	void Render(Direct3D* renderer, Camera* cam);
	void Release();

	static bool InitialiseSystem(Direct3D* renderer, LPCWSTR shaderFilename);
	static void Begin(Direct3D* renderer);
	static void End(Direct3D* renderer);
	static void ShutdownSystem();

	float systemTimeCreated;
};

#endif
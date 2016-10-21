#ifndef PARTICLESHADER_H
#define PARTICLESHADER_H

#include "Shader.h"
#include "Camera.h"

class ParticleShader : public Shader
{
private:
	struct ParticleBuffer
	{
		Vector3 cameraPosition;
		float particleRadius;
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	
	ID3D11GeometryShader* m_geometryShader;
	ID3D11SamplerState* m_textureSampler;
	ID3D11Buffer* m_particleBuffer;

	float m_particleRadius;
	Camera* m_camera;

public:
	ParticleShader(float radius, Camera* cam);
	~ParticleShader();

	void Begin(ID3D11DeviceContext* context);
	bool Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename, LPCWSTR geometryFilename);
	bool SetConstants(ID3D11DeviceContext* context, Matrix world, Matrix view, Matrix projection);
	bool SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView);
	void Release();

	void SetParticleRadius(float radius) { m_particleRadius = radius; }
	void SetCamera(Camera* cam) { m_camera = cam; }
};

#endif
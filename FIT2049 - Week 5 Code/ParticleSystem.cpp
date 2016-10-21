#include "ParticleSystem.h"

float randMinMax(float min, float max)
{
	return min + (max - min) * ((float)rand() / RAND_MAX);
}

Vector3 randUnitVector()
{
	Vector3 vec(randMinMax(-1.0f, 1.0f), randMinMax(-1.0f, 1.0f), randMinMax(-1.0f, 1.0f));
	vec.Normalize();
	return vec;
}

ParticleSystem::ParticleSystem()
{
	m_particleTexture = NULL;
	
	m_vertexBuffer = NULL;

	m_activeParticleList = NULL;
	m_freeParticleList = NULL;

	m_activeCount = 0;
	m_currentTime = 0;
	m_lastUpdate = 0;

	m_particleRadius = 1.0f;
	m_particleColour = Color(1, 1, 1, 1);
	
	m_maxParticles = 0;
	m_numberToRelease = 0;
	m_releaseInterval = 0;
	m_particleLifetimeMin = 0;
	m_particleLifetimeMax = 0;

	m_systemPosition = Vector3::Zero;
	m_systemVelocity = Vector3::Zero;
	m_systemGravity = Vector3::Zero;
	m_systemWind = Vector3::Zero;
	m_useWind = false;
	m_velocityVariance = 0;

	systemTimeCreated = 0;
	bursted = false;
}
ParticleSystem::~ParticleSystem()
{

}

bool ParticleSystem::Initialise(Direct3D* renderer)
{
	D3D11_BUFFER_DESC vertexBufferDescription;

	vertexBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDescription.ByteWidth = sizeof(ParticleVertex) * MAX_BUFFER_SIZE;
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDescription.MiscFlags = 0;
	vertexBufferDescription.StructureByteStride = 0;

	if (FAILED(renderer->GetDevice()->CreateBuffer(&vertexBufferDescription, NULL, &m_vertexBuffer)))
	{
		return false;
	}

	return true;
}

void ParticleSystem::Update(float timestep)
{
	//Update existing Particles!

	Particle* currentParticle = NULL;
	Particle** particleListPointer;

	m_currentTime += timestep;
	particleListPointer = &m_activeParticleList;

	while (*particleListPointer)
	{
		currentParticle = *particleListPointer;
		
		float timePassed = m_currentTime - currentParticle->timeCreated;
		if (timePassed >= currentParticle->lifetime)
		{
			*particleListPointer = currentParticle->next;
			currentParticle->next = m_freeParticleList;
			m_freeParticleList = currentParticle;
			
			m_activeCount--;
		}
		else
		{
			currentParticle->velocity += m_systemGravity * timestep;

			if (m_useWind)
				currentParticle->velocity += (m_systemWind - currentParticle->velocity) * timestep;
			
			currentParticle->position += currentParticle->velocity * timestep;
			particleListPointer = &currentParticle->next;
		}
	}

	//Release new particles!
	if (m_currentTime - m_lastUpdate > m_releaseInterval && !bursted)
	{
		m_lastUpdate = m_currentTime;

		for (int i = 0; i < m_numberToRelease; i++)
		{
			if (m_freeParticleList)
			{
				currentParticle = m_freeParticleList;
				m_freeParticleList = currentParticle->next;
				currentParticle->next = NULL;
			}
			else
			{
				if (m_activeCount < m_maxParticles)
				{
					currentParticle = new Particle();
					if (!currentParticle)
						return;

					currentParticle->next = NULL;
				}
			}

			if (m_activeCount < m_maxParticles)
			{
				currentParticle->next = m_activeParticleList;
				m_activeParticleList = currentParticle;

				currentParticle->velocity = m_systemVelocity;

				if (m_velocityVariance != 0.0f)
				{
					
					m_velocityVariance = (4.0 - rand() % 4) * 2;
					currentParticle->velocity += randUnitVector() * m_velocityVariance;
				}

				currentParticle->timeCreated = m_currentTime;
				currentParticle->position = m_systemPosition;
				currentParticle->lifetime = randMinMax(m_particleLifetimeMin, m_particleLifetimeMax);
				
				m_activeCount++;
			}
		}

		bursted = true;
	}
}
void ParticleSystem::Render(Direct3D* renderer, Camera* cam)
{
	//Copy verts into buffer
	Particle* currentParticle = m_activeParticleList;
	ParticleVertex* verts;
	ParticleConstantsBuffer* constants;
	D3D11_MAPPED_SUBRESOURCE vertexResource;
	D3D11_MAPPED_SUBRESOURCE constantsResource;

	if (FAILED(renderer->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexResource)))
	{
		return;
	}
	ParticleVertex* start = (ParticleVertex*)vertexResource.pData;
	verts = (ParticleVertex*)vertexResource.pData;
	while (currentParticle)
	{
		verts->position = currentParticle->position;
		verts++;

		currentParticle = currentParticle->next;
	}
	
	renderer->GetDeviceContext()->Unmap(m_vertexBuffer, 0);

	//Set Constant buffer
	if (FAILED(renderer->GetDeviceContext()->Map(s_constantsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantsResource)))
	{
		return;
	}

	constants = (ParticleConstantsBuffer*)constantsResource.pData;

	Matrix wvp = cam->GetView() * cam->GetProjection();

	constants->cameraPosition = cam->GetPosition();
	constants->particleRadius = m_particleRadius;
	constants->worldViewProjection = wvp.Transpose();
	constants->particleColour = m_particleColour;

	renderer->GetDeviceContext()->Unmap(s_constantsBuffer, 0);
	
	renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	unsigned int stride = sizeof(ParticleVertex);
	unsigned int offset = 0;
	renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	
	//Set Texture
	ID3D11ShaderResourceView* textureView = m_particleTexture->GetShaderResourceView();
	renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &textureView);
	
	renderer->GetDeviceContext()->Draw(m_activeCount, 0);
}
void ParticleSystem::Release()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}

	while (m_activeParticleList)
	{
		Particle* p = m_activeParticleList;
		m_activeParticleList = p->next;
		delete p;
	}

	while (m_freeParticleList)
	{
		Particle* p = m_freeParticleList;
		m_freeParticleList = p->next;
		delete p;
	}
}

//Static Methods

ID3D11VertexShader* ParticleSystem::s_vertexShader = NULL;
ID3D11PixelShader* ParticleSystem::s_pixelShader = NULL;
ID3D11GeometryShader* ParticleSystem::s_geometryShader = NULL;
ID3D11InputLayout* ParticleSystem::s_inputLayout = NULL;
ID3D11Buffer* ParticleSystem::s_constantsBuffer = NULL;
ID3D11SamplerState* ParticleSystem::s_textureSampler = NULL;

bool ParticleSystem::InitialiseSystem(Direct3D* renderer, LPCWSTR shaderFilename)
{
	if (!InitialiseShaders(renderer, shaderFilename))
	{
		return false;
	}

	if (!InitialiseBuffers(renderer))
	{
		return false;
	}
	return true;
}

bool ParticleSystem::InitialiseShaders(Direct3D* renderer, LPCWSTR shaderFilename)
{
	ID3DBlob* vertexShaderBlob = NULL;
	ID3DBlob* geometryShaderBlob = NULL;
	ID3DBlob* pixelShaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	const unsigned int numberOfVertexElements = 1;
	D3D11_INPUT_ELEMENT_DESC vertexLayout[numberOfVertexElements];

	if (FAILED(D3DCompileFromFile(shaderFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			OutputDebugString((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		return false;
	}

	if (FAILED(D3DCompileFromFile(shaderFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GSMain", "gs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &geometryShaderBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			OutputDebugString((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		return false;
	}

	if (FAILED(D3DCompileFromFile(shaderFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			OutputDebugString((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (vertexShaderBlob)
			vertexShaderBlob->Release();
		if (geometryShaderBlob)
			geometryShaderBlob->Release();

		return false;
	}

	if (FAILED(renderer->GetDevice()->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &s_vertexShader)))
	{
		if (vertexShaderBlob)
			vertexShaderBlob->Release();
		if (geometryShaderBlob)
			geometryShaderBlob->Release();
		if (pixelShaderBlob)
			pixelShaderBlob->Release();
		if (errorBlob)
			errorBlob->Release();

		return false;
	}
	if (FAILED(renderer->GetDevice()->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), NULL, &s_geometryShader)))
	{
		if (vertexShaderBlob)
			vertexShaderBlob->Release();
		if (geometryShaderBlob)
			geometryShaderBlob->Release();
		if (pixelShaderBlob)
			pixelShaderBlob->Release();
		if (errorBlob)
			errorBlob->Release();

		return false;
	}
	if (FAILED(renderer->GetDevice()->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &s_pixelShader)))
	{
		if (vertexShaderBlob)
			vertexShaderBlob->Release();
		if (geometryShaderBlob)
			geometryShaderBlob->Release();
		if (pixelShaderBlob)
			pixelShaderBlob->Release();
		if (errorBlob)
			errorBlob->Release();

		return false;
	}

	vertexLayout[0].SemanticName = "POSITION";
	vertexLayout[0].SemanticIndex = 0;
	vertexLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexLayout[0].InputSlot = 0;
	vertexLayout[0].AlignedByteOffset = 0;
	vertexLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[0].InstanceDataStepRate = 0;

	if (FAILED(renderer->GetDevice()->CreateInputLayout(vertexLayout, numberOfVertexElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &s_inputLayout)))
	{
		if (vertexShaderBlob)
			vertexShaderBlob->Release();
		if (geometryShaderBlob)
			geometryShaderBlob->Release();
		if (pixelShaderBlob)
			pixelShaderBlob->Release();
		if (errorBlob)
			errorBlob->Release();

		return false;
	}

	return true;
}

bool ParticleSystem::InitialiseBuffers(Direct3D* renderer)
{
	D3D11_BUFFER_DESC constantsBufferDescription;

	constantsBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	constantsBufferDescription.ByteWidth = sizeof(ParticleConstantsBuffer);
	constantsBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantsBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantsBufferDescription.MiscFlags = 0;
	constantsBufferDescription.StructureByteStride = 0;

	if (FAILED(renderer->GetDevice()->CreateBuffer(&constantsBufferDescription, NULL, &s_constantsBuffer)))
	{
		return false;
	}

	return true;
}

bool ParticleSystem::InitialiseSamplers(Direct3D* renderer)
{
	D3D11_SAMPLER_DESC textureSamplerDescription;

	textureSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDescription.MipLODBias = 0.0f;
	textureSamplerDescription.MaxAnisotropy = 1;
	textureSamplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	textureSamplerDescription.BorderColor[0] = 1;
	textureSamplerDescription.BorderColor[1] = 1;
	textureSamplerDescription.BorderColor[2] = 1;
	textureSamplerDescription.BorderColor[3] = 1;
	textureSamplerDescription.MinLOD = 0;
	textureSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(renderer->GetDevice()->CreateSamplerState(&textureSamplerDescription, &s_textureSampler)))
	{
		return false;
	}

	return false;
}

void ParticleSystem::Begin(Direct3D* renderer)
{
	renderer->GetDeviceContext()->IASetInputLayout(s_inputLayout);

	renderer->GetDeviceContext()->VSSetShader(s_vertexShader, NULL, 0);
	renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &s_constantsBuffer);
	
	renderer->GetDeviceContext()->GSSetShader(s_geometryShader, NULL, 0);
	renderer->GetDeviceContext()->GSSetConstantBuffers(0, 1, &s_constantsBuffer);
	
	renderer->GetDeviceContext()->PSSetShader(s_pixelShader, NULL, 0);
	renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &s_constantsBuffer);
	renderer->GetDeviceContext()->PSSetSamplers(0, 1, &s_textureSampler);

	renderer->TurnOffZBuffer();
	renderer->TurnOnAlphaBlending();
}

void ParticleSystem::End(Direct3D* renderer)
{
	//Turn off weird alpha stuff
	renderer->GetDeviceContext()->GSSetShader (NULL, NULL, 0);
	renderer->TurnOnZBuffer();
	renderer->TurnOffAlphaBlending();
}

void ParticleSystem::ShutdownSystem()
{
	if (s_vertexShader)
	{
		s_vertexShader->Release();
		s_vertexShader = NULL;
	}
	if (s_pixelShader)
	{
		s_pixelShader->Release();
		s_pixelShader = NULL;
	}
	if (s_geometryShader)
	{
		s_geometryShader->Release();
		s_geometryShader = NULL;
	}
	if (s_inputLayout)
	{
		s_inputLayout->Release();
		s_inputLayout = NULL;
	}
	if (s_constantsBuffer)
	{
		s_constantsBuffer->Release();
		s_constantsBuffer = NULL;
	}
	if (s_textureSampler)
	{
		s_textureSampler->Release();
		s_textureSampler = NULL;
	}
}
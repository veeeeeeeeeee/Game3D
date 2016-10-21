#include "ParticleShader.h"

ParticleShader::ParticleShader(float radius, Camera* cam) : Shader()
{
	m_geometryShader = NULL;
	m_textureSampler = NULL;
	m_particleBuffer = NULL;
	m_particleRadius = radius;
	m_camera = cam;
}
ParticleShader::~ParticleShader()
{

}

void ParticleShader::Begin(ID3D11DeviceContext* context)
{
	Shader::Begin(context);
	context->GSSetShader(m_geometryShader, NULL, 0);
	context->GSSetConstantBuffers(0, 1, &m_particleBuffer);
}
bool ParticleShader::Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename, LPCWSTR geometryFilename)
{
	//This method loads and compiles the Vertex and Pixel shader code, sets up the vertex layout and creates a CPU accessable buffer to hold the matrices

	ID3DBlob* vertexShaderBlob = NULL;		//Compiled shader code is stored as a binary blob of data, this is for the vertex shader
	ID3DBlob* pixelShaderBlob = NULL;		//and this one is for the pixel shader
	ID3DBlob* geometryShaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;				//Any compiler errors are stored in this blob, they will be a string which we can output if needed

	const unsigned int numberOfVertexElements = 2;					//The input layout needs to know how many elements make up our vertices
	D3D11_INPUT_ELEMENT_DESC vertexLayout[numberOfVertexElements];	//Each element will have a Description struct which tells us how they should be layed out
	D3D11_BUFFER_DESC matrixBufferDescription;						//We will also need to create a Description struct for the buffer we are creating for the matrices
	D3D11_BUFFER_DESC particleBufferDescription;
	D3D11_SAMPLER_DESC textureSamplerDescription;

	//We use D3DCompileFromFile to compile the HLSL code for our shaders
	if (FAILED(D3DCompileFromFile(vertexFilename,	//The filename of the source code file
		NULL,										//Any marco defines we want to include
		D3D_COMPILE_STANDARD_FILE_INCLUDE,			//Here we can specify include files
		"main",							//This is the name of the entry point method
		"vs_4_0",									//What Shader Model (version) do we want to target, shader model 4 is old but works almost everywhere
		D3DCOMPILE_ENABLE_STRICTNESS, 0,			//Misc flags, more info can be found at MSDN
		&vertexShaderBlob, &errorBlob)))			//Pointers to the shader blob and error blob, these are output parameters, if everything went well then
		//vertexSahderBlob should be something and errorBlob should be NULL
	{
		//If the compilation failed...
		if (errorBlob)	//If there is an error blob then it was most likely a syntax error in the shader
		{
			OutputDebugString((char*)errorBlob->GetBufferPointer());	//We'll output it to the Output window in Visual Studio
			errorBlob->Release();										//And release everything and return false
		}
		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		return false;
	}

	//Compiling the pixel shader is the same process but we use "ps_4_0" as the shader model
	if (FAILED(D3DCompileFromFile(pixelFilename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0,
		&pixelShaderBlob, &errorBlob)))
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

	//Compiling the pixel shader is the same process but we use "ps_4_0" as the shader model
	if (FAILED(D3DCompileFromFile(geometryFilename,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"gs_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0,
		&geometryShaderBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			OutputDebugString((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (vertexShaderBlob)
			vertexShaderBlob->Release();
		if (pixelShaderBlob)
			pixelShaderBlob->Release();
		return false;
	}

	//If we have compiled everything correctly then we can create the actual shader objects, if they fail we release everything and return false
	if (FAILED(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &m_vertexShader)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();

		if (geometryShaderBlob)
			geometryShaderBlob->Release();

		return false;
	}

	if (FAILED(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &m_pixelShader)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();

		if (geometryShaderBlob)
			geometryShaderBlob->Release();

		return false;
	}

	if (FAILED(device->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), NULL, &m_geometryShader)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();

		if (geometryShaderBlob)
			geometryShaderBlob->Release();

		return false;
	}

	//Here we start filling out the Descriptions of each Vertex Element
	//The Input Layout uses a concept known as "Semantics". Each element has a semantic name, these names match semantics that are defined in the shader code
	//The order of these descriptions must match the order of the elements on our Vertex struct, see Mesh.h

	vertexLayout[0].SemanticName = "POSITION";				//The first element is the position vector so we use the "POSITION" semantic
	vertexLayout[0].SemanticIndex = 0;						//Semantics can be indexed (like an array) so that you can pass more then one in. We only have one position though
	vertexLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	//This specifies how the element is interpreted, our vector is made up of 3 32-bit floats so the format needs to 
	vertexLayout[0].InputSlot = 0;							//Shader input can come from lots of different places, we only use one input slot so 0 is fine for us			
	vertexLayout[0].AlignedByteOffset = 0;					//This specifies the offset from the start of the Vertex where this element begins, 
	vertexLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	//This specifies that this element is present for each vertex
	vertexLayout[0].InstanceDataStepRate = 0;				//This is for a technique called instancing, we won't be covering it this semester

	vertexLayout[1].SemanticName = "COLOR";					//The next input element is our colour
	vertexLayout[1].SemanticIndex = 0;
	vertexLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//It is 4 32-bit floats so our format matches that
	vertexLayout[1].InputSlot = 0;
	vertexLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	//Here we would calculate the byte offset by factoring in the size of the last element
	vertexLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexLayout[1].InstanceDataStepRate = 0;


	//After we have described our input elements we can create our input layout, this method needs the descriptions we created 
	//and the vertex shader with the semantics that match the ones in the descriptions
	if (FAILED(device->CreateInputLayout(vertexLayout, numberOfVertexElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_layout)))
	{
		if (errorBlob)
			errorBlob->Release();

		if (vertexShaderBlob)
			vertexShaderBlob->Release();

		if (pixelShaderBlob)
			pixelShaderBlob->Release();

		return false;
	}

	//After the shaders and the input layout are created then we can release the shader blobs
	vertexShaderBlob->Release();
	vertexShaderBlob = NULL;

	pixelShaderBlob->Release();
	pixelShaderBlob = NULL;

	geometryShaderBlob->Release();
	geometryShaderBlob = NULL;

	textureSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;		//This is the Filtering method used for the texture... 
	//...different values will give you different quality texture output
	textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//This defines what happens when we sample outside of the range [0...1]
	textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//In our case we just want it to wrap around so that we always are sampling something
	textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDescription.MipLODBias = 0.0f;							//The rest of these values are really just "Defaults"
	textureSamplerDescription.MaxAnisotropy = 1;							//If you want more info look up D3D11_SAMPLER_DESC on MSDN
	textureSamplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	textureSamplerDescription.BorderColor[0] = 1;
	textureSamplerDescription.BorderColor[1] = 1;
	textureSamplerDescription.BorderColor[2] = 1;
	textureSamplerDescription.BorderColor[3] = 1;
	textureSamplerDescription.MinLOD = 0;
	textureSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	//and create the sampler!
	if (FAILED(device->CreateSamplerState(&textureSamplerDescription, &m_textureSampler)))
	{
		return false;
	}

	//The final step is to configure the buffer that will hold the matrices for the vertex shader
	matrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;				//The buffer is used for dynamic data that changes often
	matrixBufferDescription.ByteWidth = sizeof(MatrixBuffer);			//It's big enough to hold one "MatrixBuffer" sized object
	matrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		//It's a buffer containing shader constants
	matrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//It is directly writable to by the CPU
	matrixBufferDescription.MiscFlags = 0;								//Misc flags
	matrixBufferDescription.StructureByteStride = 0;					//Data offsets

	//Create the buffer based on that description!
	if (FAILED(device->CreateBuffer(&matrixBufferDescription, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	particleBufferDescription.Usage = D3D11_USAGE_DYNAMIC;				//The buffer is used for dynamic data that changes often
	particleBufferDescription.ByteWidth = sizeof(MatrixBuffer);			//It's big enough to hold one "MatrixBuffer" sized object
	particleBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		//It's a buffer containing shader constants
	particleBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//It is directly writable to by the CPU
	particleBufferDescription.MiscFlags = 0;								//Misc flags
	particleBufferDescription.StructureByteStride = 0;					//Data offsets

	//Create the buffer based on that description!
	if (FAILED(device->CreateBuffer(&particleBufferDescription, NULL, &m_particleBuffer)))
	{
		return false;
	}

	return true;
}
bool ParticleShader::SetConstants(ID3D11DeviceContext* context, Matrix world, Matrix view, Matrix projection)
{
	if (!Shader::SetConstants(context, world, view, projection))
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ParticleBuffer* inputData;

	if (FAILED(context->Map(m_particleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	inputData = (ParticleBuffer*)mappedResource.pData;

	inputData->particleRadius = m_particleRadius;
	inputData->cameraPosition = m_camera->GetPosition();
	inputData->world = world;
	inputData->view = view;
	inputData->projection = projection;

	context->Unmap(m_particleBuffer, 0);



	return true;
}
bool ParticleShader::SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView)
{
	context->PSSetShaderResources(0, 1, &textureView);
	return true;
}
void ParticleShader::Release()
{
	Shader::Release();
	if (m_textureSampler)
	{
		m_textureSampler->Release();
		m_textureSampler = NULL;
	}
	if (m_particleBuffer)
	{
		m_particleBuffer->Release();
		m_particleBuffer = NULL;
	}
	if (m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = NULL;
	}
}
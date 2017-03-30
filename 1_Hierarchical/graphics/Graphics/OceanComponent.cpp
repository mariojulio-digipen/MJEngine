#include "OceanComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "FrameRateController.h"
#include "RenderManager.h"
#include "Render3D.h"
#include "ResourceManager.h"
#include "IFFT2.h"
#include "Lighter.h"
#include "UIManager.h"

#include <random>
#include <gtc\matrix_transform.hpp>

#define PI 3.14159265359f

extern Shader* shader;
extern Shader* oceanShader;
extern FrameRateController* frameRateController;
extern RenderManager* renderManager;
extern ResourceManager* resourceManager;
extern UIManager* uiManager;

OceanComponent::OceanComponent()
{
}


OceanComponent::~OceanComponent()
{
}

bool OceanComponent::Init()
{
	t = 0.1f;
	logDim = 6;
	N = 1 << logDim;
	length = N * 8 ;
	N_plus_1 = N + 1;
	height_control = 1;

	trans = GetOwner()->FindComponent<TransformComponent>();


	return true;
}

void OceanComponent::Update()
{
	//t = frameRateController->GetTick()* 0.016f;
	time_step = uiManager->OceanTimeStep;//0.5f;
	t += time_step;

	// ui updates
	//length = uiManager->OceanGridLength;
	height_control = uiManager->HeigthControl;
	//
	Phillips_A = 0.0005f;

	computeWaveHeight(t);
	drawOcean();
	
	/*WaveVertexPOM vertex;
	
	WaveVertexPOM* pVertex = &vertex;

	glm::vec3* vertexAddr = &pVertex->location_i;
	glm::vec3* normalAddr = &pVertex->normal_i;
	glm::vec2* textureAddr = &pVertex->uv;

	void* vertexAddrNum = &pVertex->location_i;
	size_t locS = sizeof(glm::vec3);

	void* normalAddrNum = vertexAddr + 1;
	void* normalAddrNum2 = vertexAddr + 2;


	void* textureAddrNum = vertexAddr + sizeof(glm::vec3);*/


	//printf("elapsed: %f\n", t * 0.016f);
}

void OceanComponent::Destroy()
{
	if(ifft2)
		delete ifft2;
	if(texture)
		delete texture;
}

void OceanComponent::Serialize(std::istringstream * objcontentStream)
{
	
}

void OceanComponent::CompleteExtraSetup()
{
}

void OceanComponent::Start()
{
	
	initializeMatrices();
	ifft2 = new FFT2<float>(logDim, logDim);
	wind_dir = glm::vec2(32.0f, 32.0f);
	gravity = 0.016 * 9.8f;
	Phillips_A = 0.0005f;

	// simulation vertices initialization
	initializeVertexProperties();
	

	// ocean rendering initialization
	GameObject* obj = resourceManager->GetGameObjectByName("Sphere");
	light = obj->FindComponent<LightPointComponent>();

	initializeRenderIndices();
	createGLBuffers();
	buildOceanVBOs();
	createTexture();

}

void OceanComponent::HandleEvent(const Event * const evt)
{
}

glm::vec2 OceanComponent::waveVector(int n_prime, int m_prime)
{
	glm::vec2 K;
	K.x = PI*(2.0f * n_prime - N) / length;
	K.y = PI*(2.0f * m_prime - N) / length;
	return K;
}

float OceanComponent::dispersionRelation(int n_prime, int m_prime)
{
	glm::vec2 K = waveVector(n_prime, m_prime);
	float k_magnitude = glm::length(K);//std::sqrt(K.x*K.x + K.y*K.y);
	float w = std::sqrt(gravity * k_magnitude);
	return w;
}

float OceanComponent::phillipsSpectrum(int n_prime, int m_prime)
{
	// K vector
	glm::vec2 K = waveVector(n_prime, m_prime);
	float K_Magnitude = glm::length(K);
	if (K_Magnitude < 0.000001)
		return 0.0f; // Otherwise, I'll get a division by zero error.

	// L factor (V^2/g)
	float wind_dir_magnitude_V = glm::length(wind_dir);
	float L = (wind_dir_magnitude_V * wind_dir_magnitude_V) / gravity;

	// exponent factor
	float KM2 = K_Magnitude * K_Magnitude;
	float L2 = L * L;
	float exp_powwer = -1.0f / ( KM2 * L2);
	float exp = std::expf(exp_powwer);

	// dot product factor
	K = glm::normalize(K);
	glm::vec2 wind_norm = glm::normalize(wind_dir);
	float dot = glm::dot(K, wind_norm);
	float dot_squared = dot * dot;

	// denominator
	float KM4 = KM2 * KM2;

	// final phillips
	float Ph = Phillips_A * (exp / KM4) * dot_squared;

	return Ph;
}

std::complex<float> OceanComponent::hTildeT0(int n_prime, int m_prime)
{
	float rx, rz;
	// random number
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<> d(0.0, 1.0); // (mean, stddev)
	rx = static_cast<float>(d(gen));
	rz = static_cast<float>(d(gen));
	std::complex<float> r(rx, rz);
	std::complex<float> htildet0;

	// result
	float Ph = phillipsSpectrum(n_prime, m_prime);
	htildet0 = r * std::sqrtf(Ph * 0.5f);
	
	return htildet0;
}

std::complex<float> OceanComponent::hTilde(float t, int n_prime, int m_prime)
{
	// the heights
	int index = m_prime * N_plus_1 + n_prime;
	std::complex<float> h_tilde0 = vertices[index].h_tilde_i * height_control;//hTildeT0(n_prime, m_prime);//
	std::complex<float> h_tilde0_conj = vertices[index].h_tilde_conjugate_i * height_control;//hTildeT0(-n_prime, -m_prime)//

	// the dispersion relation (w or omega)
	float omega = dispersionRelation(n_prime, m_prime);

	// the exponent power
	float power = omega * t;

	// exponential factor (euler's formula)
	// e^(i*power) = cos(power) + i*sin(power)
	std::complex<float> exp, exp_signed;
	float cosf = std::cosf(power);
	float sinf = std::sinf(power);
	exp = std::complex<float>(cosf, sinf);
	exp_signed = std::complex<float>(cosf, -sinf);
	
	// final height tilde on time t
	std::complex<float> htildet = h_tilde0 * exp + h_tilde0_conj * exp_signed;

	return htildet;
}

void OceanComponent::computeWaveHeight(float t)
{
	// we need to solve the following 2D FFT:
	/*

	h'(x,z,t) = (-1^z) * sum<0, M-1>[ e^(i*2*PI*m'*z/N) * h''(x,m',t) ]
	h''(x,m',t) =  (-1^x) * sum<0, N-1>[ e^(i*2*PI*n'*x/N) * h_tilde(n',m',t) ]  

	*/
	glm::vec2 K;
	float Kx, Kz;
	float K_mag;
	// populate the input matrix for our FFT -> h_tilde
	for (int m_prime = 0; m_prime < N; ++m_prime)
	{
		for (int n_prime = 0; n_prime < N; ++n_prime)
		{
			// set up the K vector and its properties
			K = waveVector(n_prime, m_prime);
			Kx = K.x;
			Kz = K.y;
			K_mag = glm::length(K);
			
			// fill the matrix
			h_tilde[m_prime][n_prime] = hTilde(t, n_prime, m_prime) * 0.5f;
			
			// fill the normal data
			h_tilde_slopeX[m_prime][n_prime] = h_tilde[m_prime][n_prime] *
				std::complex<float>(0.0f, Kx) * 0.5f;
			h_tilde_slopeZ[m_prime][n_prime] = h_tilde[m_prime][n_prime] *
				std::complex<float>(0.0f, Kz) * 0.5f;

			// fill the displacement data for choppiness
			if (K_mag < 0.000001f)
			{
				h_tilde_displX[m_prime][n_prime] = 
					std::complex<float>(0.0f, 0.0f);
				h_tilde_displZ[m_prime][n_prime] =
					std::complex<float>(0.0f, 0.0f);
			}
			else
			{
				h_tilde_displX[m_prime][n_prime] =
					h_tilde[m_prime][n_prime] * std::complex<float>(0.0, -Kx / length);
				h_tilde_displZ[m_prime][n_prime] =
					h_tilde[m_prime][n_prime] * std::complex<float>(0.0, -Kz / length);
			}

		}
	}

	// compute 2D Fast Fourier Transform (rows first, then columns)
	
	//IFFT2<float> fft2D 
	try
	{
		ifft2->operator()(h_tilde);
		ifft2->operator()(h_tilde_slopeX);
		ifft2->operator()(h_tilde_slopeZ);
		ifft2->operator()(h_tilde_displX);
		ifft2->operator()(h_tilde_displZ);
	}
	catch (const std::exception& e)
	{
		std::cout << "Something went wrong with FFT in Ocean Component" 
			<< std::endl;
		std::cout << e.what();
		getchar();
		return;
	}

	// since we reformatted the formula to fit the FFT and the grid configuration
	// we need to add the (-1)^x factor to each element of the matrices
	int vIndex;
	float lambda = -1.0f;
	float sign;
	float signs[] = { 1.0f, -1.0f };
	glm::vec3 normal;
	for (int m_prime = 0; m_prime < N; m_prime++)
	{
		for (int n_prime = 0; n_prime < N; n_prime++)
		{
			vIndex = m_prime * N + n_prime;
			sign = signs[(n_prime + m_prime) & 1];

			// actual height
			h_tilde[m_prime][n_prime] = sign * h_tilde[m_prime][n_prime];
			vertices[vIndex].location_i.y = h_tilde[m_prime][n_prime].real();

			// displacement (choppiness)
			h_tilde_displX[m_prime][n_prime] = sign * h_tilde_displX[m_prime][n_prime];
			h_tilde_displZ[m_prime][n_prime] = sign * h_tilde_displZ[m_prime][n_prime];
			vertices[vIndex].location_i.x = vertices[vIndex].start_pos_i.x +
				h_tilde_displX[m_prime][n_prime].real() * lambda;
			vertices[vIndex].location_i.z = vertices[vIndex].start_pos_i.z +
				h_tilde_displZ[m_prime][n_prime].real() * lambda;

			// normal
			h_tilde_slopeX[m_prime][n_prime] = sign * h_tilde_slopeX[m_prime][n_prime];
			h_tilde_slopeZ[m_prime][n_prime] = sign * h_tilde_slopeZ[m_prime][n_prime];
			
			normal = glm::vec3(
				-h_tilde_slopeX[m_prime][n_prime].real(), 
				1.0f, 
				-h_tilde_slopeZ[m_prime][n_prime].real());

			vertices[vIndex].normal_i.x = normal.x;
			vertices[vIndex].normal_i.y = normal.y;
			vertices[vIndex].normal_i.z = normal.z;

			// tiling
			if (n_prime == 0 && m_prime == 0) {
				int idx = vIndex + N + N_plus_1 * N;
				vertices[idx].location_i.y = h_tilde[m_prime][n_prime].real();

				vertices[idx].location_i.x = vertices[idx].start_pos_i.x + 
					h_tilde_displX[m_prime][n_prime].real() * lambda;
				vertices[idx].location_i.z = vertices[idx].start_pos_i.z + 
					h_tilde_displZ[m_prime][n_prime].real() * lambda;

				vertices[idx].normal_i.x = normal.x;
				vertices[idx].normal_i.y = normal.y;
				vertices[idx].normal_i.z = normal.z;
			}
			if (n_prime == 0) {
				int idx = vIndex + N;
				vertices[idx].location_i.y = h_tilde[m_prime][n_prime].real();

				vertices[idx].location_i.x = vertices[idx].start_pos_i.x +
					h_tilde_displX[m_prime][n_prime].real() * lambda;
				vertices[idx].location_i.z = vertices[idx].start_pos_i.z +
					h_tilde_displZ[m_prime][n_prime].real() * lambda;

				vertices[idx].normal_i.x = normal.x;
				vertices[idx].normal_i.y = normal.y;
				vertices[idx].normal_i.z = normal.z;
			}
			if (m_prime == 0) {
				int idx = vIndex + N_plus_1* N;
				vertices[idx].location_i.y = h_tilde[m_prime][n_prime].real();

				vertices[idx].location_i.x = vertices[idx].start_pos_i.x +
					h_tilde_displX[m_prime][n_prime].real() * lambda;
				vertices[idx].location_i.z = vertices[idx].start_pos_i.z +
					h_tilde_displZ[m_prime][n_prime].real() * lambda;

				vertices[idx].normal_i.x = normal.x;
				vertices[idx].normal_i.y = normal.y;
				vertices[idx].normal_i.z = normal.z;
			}
		}
	}



}

void OceanComponent::initializeMatrices()
{
	std::vector<std::complex<float>> rows(N);

	for (int i = 0; i < N; i++)
	{
		h_tilde_slopeX.push_back(rows);
		h_tilde_slopeZ.push_back(rows);
		h_tilde_displX.push_back(rows);
		h_tilde_displZ.push_back(rows);
		h_tilde.push_back(rows);
	}

	std::vector<WaveVertexPOM> v(N_plus_1 * N_plus_1);
	std::vector<unsigned int> ix(N * N_plus_1 * 6);
	vertices = v;
	indices = ix;

}

void OceanComponent::initializeVertexProperties()
{
	int index = 0;
	std::complex<float> c;
	glm::vec3 xyz;

	float startX, startY, startZ;
	startX = trans->GetPosition().x;
	startY = trans->GetPosition().y;
	startZ = trans->GetPosition().z;

	for (int m_prime = 0; m_prime < N_plus_1; m_prime++)
	{
		for (int n_prime = 0; n_prime < N_plus_1; n_prime++)
		{
			index = m_prime * N_plus_1 + n_prime;

			// initialize the h_tilde's
			std::complex<float> htildeT0 = hTildeT0(n_prime, m_prime);
			std::complex<float> htildeT0_c = hTildeT0(-n_prime, -m_prime);

			// set texture uv coords (...??)
			vertices[index].uv.x = htildeT0.real();
			vertices[index].uv.y = htildeT0.imag();

			vertices[index].h_tilde_i = htildeT0;
			vertices[index].h_tilde_conjugate_i = std::conj(htildeT0_c);

			// set the position of the wave
			float x = ((startX + n_prime - N) / 2.0f) * length / N;
			float z = ((startZ + m_prime - N) / 2.0f) * length / N;
			xyz = glm::vec3(x, startY, z);
			vertices[index].location_i = xyz;
			vertices[index].start_pos_i = xyz;

			vertices[index].normal_i = glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}

	
}

void OceanComponent::initializeRenderIndices()
{
	int index = 0;
	indices_count = 0;
	float startX, startZ;
	startX = trans->GetPosition().x;
	startZ = trans->GetPosition().z;
	for (int m_prime = 0; m_prime < N_plus_1; m_prime++)
	{
		for (int n_prime = 0; n_prime < N_plus_1; n_prime++)
		{
			index = m_prime * N_plus_1 + n_prime;

			// for triangles drawing
			if (n_prime == N)
				continue;
			indices[indices_count++] = index;
			indices[indices_count++] = index + N_plus_1;
			indices[indices_count++] = index + N_plus_1 + 1;
			indices[indices_count++] = index;
			indices[indices_count++] = index + N_plus_1 + 1;
			indices[indices_count++] = index + 1;
		}
	}

	indices_count = indices_count - N_plus_1*N_plus_1;
}

void OceanComponent::createGLBuffers()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo_vertices);
	glGenBuffers(1, &vbo_indices);
}

void OceanComponent::deleteGLBuffers()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_indices);
}

void OceanComponent::drawOcean()
{
	
	shader->UnUseProgram();

	glBindVertexArray(vao);

	//int currentShader;
	//glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);

	oceanShader->UseProgram();
	int shaderUsed = oceanShader->GetProgramID();

	

	int sizeInBytes = 0;

	// update VBOs
	/*int vertexLoc = glGetAttribLocation(shaderUsed, "Position");
	GLenum err1 = glGetError();
	int uvLoc = glGetAttribLocation(shaderUsed, "TextureCoord");
	GLenum err2 = glGetError();
	int normalLoc = glGetAttribLocation(shaderUsed, "Normal");
	GLenum err3 = glGetError();*/


	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	sizeInBytes = sizeof(WaveVertexPOM)*(N_plus_1)*(N_plus_1);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, &vertices[0]);

	glEnableVertexAttribArray(positionAttributeIndex);
	glVertexAttribPointer(positionAttributeIndex, 3, GL_FLOAT, GL_FALSE, 
		sizeof(WaveVertexPOM), 0);
	
	glEnableVertexAttribArray(normalAttributeIndex);
	glVertexAttribPointer(normalAttributeIndex, 3, GL_FLOAT, GL_FALSE, 
		sizeof(WaveVertexPOM), (char *)NULL + 1);
	
	glEnableVertexAttribArray(uvAttributeIndex);
	glVertexAttribPointer(uvAttributeIndex, 2, GL_FLOAT, GL_FALSE, 
		sizeof(WaveVertexPOM), (char *)NULL + 2);


	glEnable(GL_DEPTH_TEST);

	// begin draw

	// updates lighting
	Lighter lighter;
	lighter.ProcessLights(shaderUsed, light);

	// select unit and use texture
	oceanShader->ChooseTextureUnit(0, "waterSampler");
	texture->Use();

	glm::mat4 S;
	

	for (int j = 0; j < 1; j++) 
	{
		for (int i = 0; i < 3; i++)
		{
			S = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f)*16.0f);
			S = glm::translate(S, glm::vec3(length * i / 2.0f, 0, length * -j / 2.2f) );
			GLint locc = glGetUniformLocation(oceanShader->GetProgramID(), "OceanS");
			glUniformMatrix4fv(locc, 1, GL_FALSE, &S[0][0]);


			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}


	

	// end draw
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	oceanShader->UnUseProgram();
	glBindVertexArray(0);

	shader->UseProgram();

}

void OceanComponent::createTexture()
{
	this->texture = new Texture("ArtAssets\\DefaultColorBlue.png");
	texture->Create();
}

void OceanComponent::buildOceanVBOs()
{
	int sizeInBytes = 0;
	glBindVertexArray(vao);

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	sizeInBytes = sizeof(WaveVertexPOM)*(N_plus_1)*(N_plus_1);
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &vertices[0], GL_DYNAMIC_DRAW);


	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	sizeInBytes = sizeof(unsigned int) * indices_count;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

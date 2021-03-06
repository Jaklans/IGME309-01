#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Generate Mesh
	vector3 top = vector3(0, a_fHeight /  2, 0);
	vector3 bot = vector3(0, a_fHeight / -2, 0);

	vector3* rim = new vector3[a_nSubdivisions];

	float divAngle = 2 * PI / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++) 
	{
		//Generate all of the points for the base
		*(rim + i) = vector3(glm::cos( i * divAngle) * a_fRadius, a_fHeight / -2, glm::sin(i * divAngle) * a_fRadius);
	}

	for (int i = 0; i < a_nSubdivisions; i++) 
	{
		//Create tris from base to top
		AddTri(top, *(rim + ((i + 1) % a_nSubdivisions)), *(rim + i));
		//Create tris from the rim to the center of the base
		AddTri(bot, *(rim + i), *(rim + ((i + 1) % a_nSubdivisions)));
	}

	delete[] rim;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Generate Mesh
	vector3 top = vector3(0, a_fHeight /  2, 0);
	vector3 bot = vector3(0, a_fHeight / -2, 0);

	vector3* topRim = new vector3[a_nSubdivisions];
	vector3* botRim = new vector3[a_nSubdivisions];

	float divAngle = 2 * PI / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//Generate all of the points for both bases
		*(topRim + i) = vector3(glm::cos(i * divAngle) * a_fRadius, a_fHeight /  2, glm::sin(i * divAngle) * a_fRadius);
		*(botRim + i) = vector3(glm::cos(i * divAngle) * a_fRadius, a_fHeight / -2, glm::sin(i * divAngle) * a_fRadius);
	}

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//Create quads for each of the faces on the side
		AddQuad(*(botRim + ((i + 1) % a_nSubdivisions)), *(botRim + i), *(topRim + ((i + 1) % a_nSubdivisions)), *(topRim + i));

		//Create tris to connect rims to centers of bases
		AddTri(top, *(topRim + ((i + 1) % a_nSubdivisions)), *(topRim + i));
		AddTri(bot, *(botRim + i), *(botRim + ((i + 1) % a_nSubdivisions)));
	}

	delete[] topRim;
	delete[] botRim;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Generate Mesh
	vector3* topOuterRim = new vector3[a_nSubdivisions];
	vector3* topInnerRim = new vector3[a_nSubdivisions];
	vector3* botOuterRim = new vector3[a_nSubdivisions];
	vector3* botInnerRim = new vector3[a_nSubdivisions];

	float divAngle = 2 * PI / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//Generate all of the points for both bases
		*(topOuterRim + i) = vector3(glm::cos(i * divAngle) * a_fOuterRadius, a_fHeight /  2, glm::sin(i * divAngle) * a_fOuterRadius);
		*(topInnerRim + i) = vector3(glm::cos(i * divAngle) * a_fInnerRadius, a_fHeight /  2, glm::sin(i * divAngle) * a_fInnerRadius);
		*(botOuterRim + i) = vector3(glm::cos(i * divAngle) * a_fOuterRadius, a_fHeight / -2, glm::sin(i * divAngle) * a_fOuterRadius);
		*(botInnerRim + i) = vector3(glm::cos(i * divAngle) * a_fInnerRadius, a_fHeight / -2, glm::sin(i * divAngle) * a_fInnerRadius);
	}

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//Create quads for each of the faces on inside and outside
		AddQuad(*(botOuterRim + ((i + 1) % a_nSubdivisions)), *(botOuterRim + i), *(topOuterRim + ((i + 1) % a_nSubdivisions)), *(topOuterRim + i));
		AddQuad(*(topInnerRim + ((i + 1) % a_nSubdivisions)), *(topInnerRim + i), *(botInnerRim + ((i + 1) % a_nSubdivisions)), *(botInnerRim + i));

		//Create quads for the caps
		AddQuad(*(topOuterRim + ((i + 1) % a_nSubdivisions)), *(topOuterRim + i), *(topInnerRim + ((i + 1) % a_nSubdivisions)), *(topInnerRim + i));
		AddQuad(*(botInnerRim + ((i + 1) % a_nSubdivisions)), *(botInnerRim + i), *(botOuterRim + ((i + 1) % a_nSubdivisions)), *(botOuterRim + i));
	}

	delete[] topOuterRim;
	delete[] topInnerRim;
	delete[] botOuterRim;
	delete[] botInnerRim;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;
	
	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;
	
	Release();
	Init();

	// Generate Mesh


	//Quick issue fix
	a_fInnerRadius *= 2;
	a_fOuterRadius *= 2;
	//a_nSubdivisionsA is number of rings
	//a_nSubdivisionsB is verticies per ring

	vector3** rings = new vector3*[a_nSubdivisionsA];

	float divAngle = 2 * PI / a_nSubdivisionsB;

	float shapeRadius = a_fOuterRadius - a_fInnerRadius;
	float ringRadius = shapeRadius / 2;

	*rings = new vector3[a_nSubdivisionsB];

	//Create the base ring
	for (int i = 0; i < a_nSubdivisionsB; i++) {
		*((*rings) + i) = vector3(
			glm::cos(divAngle * i) * ringRadius + shapeRadius,
			glm::sin(divAngle * i) * ringRadius,
			0);
		//std::cout << "    x:" << (*((*rings) + i)).x << "  y:" << (*((*rings) + i)).y << "  z:" << (*((*rings) + i)).z << std::endl;
	}

	//Create rotated copies
	for (int i = 1; i < a_nSubdivisionsA; i++) {
		*(rings + i) = new vector3[a_nSubdivisionsB];
		//Create a rotation matrix
		matrix4 rotationMat = glm::rotate((float)(i * 360 / a_nSubdivisionsA), glm::vec3(0, 1, 0));
		for (int j = 0; j < a_nSubdivisionsB; j++) {
			//Create a temporary vec4 for transformation
			vector4 temp = vector4((*(*rings + j)).x, (*(*rings + j)).y, (*(*rings + j)).z, 1);

			//Rotate
			temp = rotationMat * temp;

			//Set values
			*(*(rings + i) + j) = vector3(temp.x, temp.y, temp.z);
		}
	}

	for (int i = 0; i < a_nSubdivisionsA; i++) {
		for (int j = 0; j < a_nSubdivisionsB; j++) {
			AddQuad(
				*(*(rings + i) + (j + 1) % a_nSubdivisionsB),
				*(*(rings + i) + j),
				*(*(rings + (i + 1) % a_nSubdivisionsA) + (j + 1) % a_nSubdivisionsB),
				*(*(rings + (i + 1) % a_nSubdivisionsA) + j));
		}
	}

	for (int i = 0; i < a_nSubdivisionsA; i++) {
		delete[] *(rings + i);
	}

	delete[] rings;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}

	Release();
	Init();
	
	
	// Generate Mesh
	vector3 top = vector3(0, a_fRadius, 0);
	vector3 bot = vector3(0, -a_fRadius, 0);

	vector3** rims = new vector3*[a_nSubdivisions];

	float divAngle = 2 * PI / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		*(rims + i) = new vector3[a_nSubdivisions];

		//Find the correct height subdivision acording to formula h = -R * sin(theta) + R
		float height = -a_fRadius * glm::sin(PI * i / a_nSubdivisions) + a_fRadius;

		//Find the radius at each height using: r = (h(2R - h))^.5
		float radiusAtHeight = glm::sqrt(height * (2 * a_fRadius - height));

		if (i > .5f * a_nSubdivisions) {
			height *= -1;
			height += 2 * a_fRadius;
		}
		//Fun debug outputs
		//std::cout << i << "  " << height << "  " << radiusAtHeight << std::endl;

		for (int j = 0; j < a_nSubdivisions; j++)
		{
			//Create rims at every height (turnary handles height)
			*(*(rims + i) + j) = vector3(
				glm::cos(j * divAngle) * radiusAtHeight,
				height - a_fRadius,
				glm::sin(j * divAngle) * radiusAtHeight);

			//Fun debug outputs
			//std::cout << "    x:" << (*(*(rims + i) + j)).x << "  y:" << (*(*(rims + i) + j)).y << "  z:" << (*(*(rims + i) + j)).z << std::endl;
		}
	}

	//Create quads at varius heights
	for (int i = 0; i < a_nSubdivisions; i++) {

		//Creates quads around the height belt
		for (int j = 0; j < a_nSubdivisions; j++) {

			//Ignore the caps for now
			if (i == a_nSubdivisions / 2 || 
				(i == a_nSubdivisions / 2 - 1 && a_nSubdivisions % 2 == 0)) {
				continue;
			}
			
			//Add the rings of quads at every height
			AddQuad(
				*(*(rims + (i + 1) % a_nSubdivisions) + (j + 1) % a_nSubdivisions),
				*(*(rims + (i + 1) % a_nSubdivisions) + j),
				*(*(rims + i) + (j + 1) % a_nSubdivisions),
				*(*(rims + i) + j));
		}
	}

	//Create tris caps
	int topRimIndex = a_nSubdivisions / 2 + 1;
	int botRimIndex = a_nSubdivisions / 2;

	//If odd, adjust index
	if (a_nSubdivisions % 2 == 0){
		botRimIndex -= 1;
	}
	
	for (int i = 0; i < a_nSubdivisions; i++) {
		AddTri(*(*(rims + botRimIndex) + i), *(*(rims + botRimIndex) + (i + 1) % a_nSubdivisions), bot);
		AddTri(*(*(rims + topRimIndex) + i), *(*(rims + topRimIndex) + (i + a_nSubdivisions - 1) % a_nSubdivisions), top);
	}

	for (int i = 0; i < a_nSubdivisions; i++) {
		delete[] *(rims + i);
	}
	delete[] rims;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "John Shull  |  JohnShull@mail.rit.edu";
	
	//Initialize timer variables
	fTimer = 0;
	uClock = m_pSystem->GenClock();

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);


	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));


	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}

	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits


	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red


	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	
	//Create arrays for controling shapes
	m_aShapeArray = new vector3*[m_uOrbits];
	
	m_aShapeIndecies = new int[m_uOrbits] { };

	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize + .01f, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager

		//Create a shape and add it to the array
		*(m_aShapeArray + i - uSides) = GeneratePoints(fSize, i);

		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Update the timer
	fTimer += m_pSystem->GetDeltaTime(uClock);

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	bool updateIndex = false;

	if (fTimer > 1) {
		updateIndex = true;
		fTimer = 0;
	}

	// draw the shapes and spheres
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		if (updateIndex) {
			m_aShapeIndecies[i] += 1;
		}

		/* Done with Array indexing. Ill calm down on the pointer arithmatic I swear

		vector3 startPoint = m_aShapeArray[i][m_aShapeIndecies[i % (i + 3)] % (i + 3)];
		vector3 endPoint = m_aShapeArray[i][(m_aShapeIndecies[i % (i + 3)] + 1) % (i + 3)];
		*/

		vector3 v3CurrentPos =
			glm::lerp(
				*(*(m_aShapeArray + i) + (*(m_aShapeIndecies + i % (i + 3)) % (i + 3))),
				*(*(m_aShapeArray + i) + ((*(m_aShapeIndecies + i % (i + 3)) + 1) % (i + 3))),
				fTimer);

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.15)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Delete ShapeArray
	for (int i = 0; i < m_uOrbits; i++) {
		if (*(m_aShapeArray + i) != nullptr) {
			delete[] *(m_aShapeArray + i);
			*(m_aShapeArray + i) = nullptr;
		}
	}
	delete[] m_aShapeArray;

	delete[] m_aShapeIndecies;

	//release GUI
	ShutdownGUI();
}

vector3* Application::GeneratePoints(float a_fRadius, int a_nSubdivisions)
{
	vector3* shape = new vector3[a_nSubdivisions];

	*shape = vector3(a_fRadius, 0, 0);

	for (int i = 1; i < a_nSubdivisions; i++) {
		std::cout << i * 360.0f / a_nSubdivisions << std::endl;
		*(shape + i) =  (vector3)glm::rotateZ(vector4(*shape, 1), i * 360.0f / a_nSubdivisions);
		std::cout << "   X:" << (shape + i)->x << " Y:" << (shape + i)->y << " Z:" << (shape + i)->z << std::endl;
	}

	return shape;
}
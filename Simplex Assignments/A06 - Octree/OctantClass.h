#pragma once

#include "MyEntity.h"

namespace Simplex {

	class OctantClass
	{
	public:
		
		//List of all entities in the MyOctant
		std::vector<void*> entities;

		//std::vector<MyEntity*> entities;

		//Center of the MyOctant
		vector3 center;

		float maxSize;
		
		OctantClass(float maxSize, Simplex::vector3 center);
		
		void subDivide(int maxDepth, std::vector<OctantClass>* octants);

		void CheckCollisions();

		void Draw(MeshManager* meshManager);
		
		
	};
}

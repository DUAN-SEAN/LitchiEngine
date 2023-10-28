
#include "Model.h"

#include <algorithm>

#include "Runtime/Core/DataStruct/BoundingSphere.h"
#include "Runtime/Function/Renderer/Rendering/Mesh.h"


const LitchiRuntime::BoundingSphere& LitchiRuntime::Model::GetBoundingSphere() const
{
	return boundingSphere;
}

LitchiRuntime::Model::Model(const std::string & p_path) : path(p_path)
{
}

LitchiRuntime::Model::~Model()
{
	for (auto mesh : mesheArr)
		delete mesh;
}

void LitchiRuntime::Model::ComputeBoundingSphere()
{
	/*if (mesheArr.size() == 1)
	{
		boundingSphere = mesheArr[0]->GetBoundingSphere();
	}
	else
	{
		boundingSphere.position = Vector3(0);
		boundingSphere.radius = 0.0f;

		if (!mesheArr.empty())
		{
			float minX = std::numeric_limits<float>::max();
			float minY = std::numeric_limits<float>::max();
			float minZ = std::numeric_limits<float>::max();

			float maxX = std::numeric_limits<float>::min();
			float maxY = std::numeric_limits<float>::min();
			float maxZ = std::numeric_limits<float>::min();

			for (const auto& mesh : mesheArr)
			{
				const auto& boundingSphere = mesh->GetBoundingSphere();
				minX = std::min(minX, boundingSphere.position.x - boundingSphere.radius);
				minY = std::min(minY, boundingSphere.position.y - boundingSphere.radius);
				minZ = std::min(minZ, boundingSphere.position.z - boundingSphere.radius);

				maxX = std::max(maxX, boundingSphere.position.x + boundingSphere.radius);
				maxY = std::max(maxY, boundingSphere.position.y + boundingSphere.radius);
				maxZ = std::max(maxZ, boundingSphere.position.z + boundingSphere.radius);
			}

			boundingSphere.position = Vector3{ minX + maxX, minY + maxY, minZ + maxZ } / 2.0f;
			boundingSphere.radius = glm::distance(boundingSphere.position, Vector3{ minX, minY, minZ });
		}
	}*/
}

const std::vector<LitchiRuntime::Mesh*>& LitchiRuntime::Model::GetMeshes() const
{
	return mesheArr;
}

const std::vector<std::string>& LitchiRuntime::Model::GetMaterialNames() const
{
	return materialNameArr;
}

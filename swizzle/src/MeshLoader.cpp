#include <swizzle/gfxutils/MeshLoader.hpp>

#include <fstream>
#include <vector>
#include <string>

namespace meshloader
{

	void loadObjMeshIntoBuffer(const char* fileName, swizzle::Resource<swizzle::Buffer>& vertexBuffer)
	{

		struct vertex
		{
			float x, y, z;
		};

		struct triangle
		{
			int t1, t2, t3;
		};


		std::ifstream input(fileName);
		if (input.is_open())
		{
			std::vector<vertex> vertices;
			std::vector<triangle> triangles;

			std::string line;
			int offset = 0;
			while (!input.eof())
			{
				std::getline(input, line);

				size_t off0 = line.find_first_of(' ');
				size_t off1 = line.find_first_of(' ', off0 + 1);
				size_t off2 = line.find_first_of(' ', off1 + 1);
				size_t off3 = line.find_first_of(' ', off2 + 1);

				if (line[0] == 'v')
				{
					float v1 = std::strtof(line.substr(off0, off1 - off0).c_str(), nullptr);
					float v2 = std::strtof(line.substr(off1, off2 - off1).c_str(), nullptr);
					float v3 = std::strtof(line.substr(off2, off3 - off2).c_str(), nullptr);
					vertices.push_back({ v1, v2, v3 });
				}
				else if (line[0] == 'f')
				{
					int v1 = std::strtol(line.substr(off0, off1 - off0).c_str(), nullptr, 0);
					int v2 = std::strtol(line.substr(off1, off2 - off1).c_str(), nullptr, 0);
					int v3 = std::strtol(line.substr(off2, off3 - off2).c_str(), nullptr, 0);
					triangles.push_back({ v1 + offset, v2 + offset, v3 + offset });
				}
				/*else if (line[0] == 'o')
				{
					offset = triangles.size();
				}*/
			}

			std::vector<vertex> newVerts;

			for (const auto& t : triangles)
			{
				size_t p1 = static_cast<size_t>(t.t1) - 1U;
				size_t p2 = static_cast<size_t>(t.t2) - 1U;
				size_t p3 = static_cast<size_t>(t.t3) - 1U;
				newVerts.push_back({
					vertices[p1].x,
					vertices[p1].y,
					vertices[p1].z,
					});
				newVerts.push_back({
					vertices[p2].x,
					vertices[p2].y,
					vertices[p2].z,
					});
				newVerts.push_back({
					vertices[p3].x,
					vertices[p3].y,
					vertices[p3].z,
					});
			}

			vertexBuffer->setBufferData(newVerts.data(), newVerts.size() * sizeof(vertex));

		}

	}

}
/*
 * DataStructure.h
 *
 *  Created on: Feb 15, 2018
 *      Author: gb1609
 */

#ifndef SRC_SUPPORTOBJECTS_H_
#define SRC_SUPPORTOBJECTS_H_
#include <vector>
using namespace std;
class SupportObjects
{
	private:
		glm::vec3 guardPosition;
		glm::vec3 conePosition;
		glm::vec3 spherePosition;
		glm::vec3 cubePosition;
		float radiusCone;
		float radiusSphere;
		float cubeSize;
		float sizeGuard;
	public:
		SupportObjects() // @suppress("Class members should be properly initialized")
		{
		}

		void generateFloor(vector<float>& vertex, vector<float>& texture, vector<float>&normal, float cellSize,
				int nCols, int nRows)
		{
			for (int i = 0; i < nRows + 1; i++)
				for (int j = 0; j < nCols + 1; j++)
				{
					vertex.push_back(i * cellSize);
					vertex.push_back(0.0f);
					vertex.push_back(j * cellSize);
					normal.push_back(0.0f);
					normal.push_back(1.0f);
					normal.push_back(0.0f);
					texture.push_back((cellSize * j) / (cellSize * nCols));
					texture.push_back(((cellSize * nRows) - (cellSize * i)) / (cellSize * nRows));
				}

		}

		void setGuardPosition(glm::vec3 pos, float sizeG)
		{
			guardPosition = pos;
			sizeGuard = abs(sizeG);
		}

		void setCubePosition(glm::vec3 pos, float cs)
		{
			cubePosition = pos;
			cubeSize = abs(cs);
		}

		bool checkGeneralCollision(glm::vec3 one, glm::vec3 two, int type)
		{
			glm::vec3 verify;
			float size;
			switch (type)
			{
				case 1:
					verify = guardPosition;
					size = sizeGuard;
					break;
				case 2:
					verify = conePosition;
					size = radiusCone;
					break;
				case 3:
					verify = spherePosition;
					size = radiusSphere;
					break;
				case 4:
					verify = cubePosition;
					size = cubeSize;
					cout << size << endl;
					cout << verify.x << "-" << verify.y << "-" << verify.z << endl;
					break;
				default:
					cout << "errore" << endl;
			}

			if (one.z > verify.z + size + 50 || one.z < verify.z - size - 50)
				return true;
			if (one.x > verify.x + size + 50 || one.x < verify.x - size - 50)
				return true;

			if ((verify.x - size) > one.x && verify.x - size >= two.x)
				return true;
			if ((verify.z - size) > one.z && verify.z - size >= two.z)
				return true;
			if ((verify.x + size) < one.x && verify.x + size <= two.x)
				return true;
			if ((verify.z + size) < one.z && verify.z + size <= two.z)
				return true;

			if (verify.x - size > one.x && verify.x - size < two.x)
				return false;
			if (verify.z - size > one.z && verify.z - size < two.z)
				return false;
			if (verify.x + size < one.x && verify.x + size > two.x)
				return false;
			if (verify.x + size < one.z && verify.z + size > two.z)
				return false;

			return false;

		}

		bool checkCollision(glm::vec3 one, glm::vec3 two) // AABB - AABB collision
		{
			if (checkGeneralCollision(one, two, 1) && checkGeneralCollision(one, two, 2)
					&& checkGeneralCollision(one, two, 3) && checkGeneralCollision(one, two, 4))
				return true;
			return false;
		}

		void generateIndexFloor(vector<unsigned int>& index, int nCols, int nRows)
		{
			index.resize(nCols * nRows * 6);
			int a = 0;
			for (int i = 0; i < nRows; i++)
				for (int j = 0; j < nCols; j++)
				{
					index[a++] = (i * (nCols + 1)) + j;
					index[a++] = ((i + 1) * (nCols + 1)) + j;
					index[a++] = (i * (nCols + 1)) + j + 1;
					index[a++] = ((i + 1) * (nCols + 1)) + j;
					index[a++] = ((i + 1) * (nCols + 1)) + j + 1;
					index[a++] = (i * (nCols + 1)) + j + 1;
				}
		}

		void printIndex(vector<unsigned int>& finalIndex, vector<float>& fVertex)
		{
			cout << "NUMERO DI COORDINATE: " << finalIndex.size() / 3 << endl;
			int c = 0;
			for (int i = 0; i < finalIndex.size(); i++)
				if (c++ == 2)
				{
					cout << "[" << finalIndex[i] << "->" << fVertex[finalIndex[i] * 3] << ";"
							<< fVertex[(finalIndex[i] * 3) + 1] << ";" << fVertex[(finalIndex[i] * 3) + 2] << "]"
							<< endl;
					c = 0;
				}
				else
					cout << "[" << finalIndex[i] << "->" << fVertex[finalIndex[i] * 3] << ";"
							<< fVertex[(finalIndex[i] * 3) + 1] << ";" << fVertex[(finalIndex[i] * 3) + 2] << "]";
		}
		void printVertex(vector<float>& vertex)
		{
			cout << "NUMERO DI COORDINATE: " << vertex.size() / 3 << endl;
			int c = 0;
			for (int i = 0; i < vertex.size(); i++)
				if (c++ == 2)
				{
					cout << "[" << vertex[i] << "]" << endl;
					c = 0;
				}
				else
					cout << "[" << vertex[i] << "]";
		}

		const glm::vec3& getGuardPosition() const
		{
			return guardPosition;
		}

		const glm::vec3& getConePosition() const
		{
			return conePosition;
		}

		const glm::vec3& getSpherePosition() const
		{
			return spherePosition;
		}

		void setConePosition(const glm::vec3& conePosition, float radiusCone)
		{
			this->conePosition = conePosition;
			this->radiusCone = radiusCone;
		}
		void setSPherePosition(const glm::vec3& conePosition, float radiusCone)
		{
			this->spherePosition = conePosition;
			this->radiusSphere = radiusCone;
		}

		const glm::vec3& getCubePosition() const
		{
			return cubePosition;
		}

		float getCubeSize() const
		{
			return cubeSize;
		}

		float getRadiusCone() const
		{
			return radiusCone;
		}

		float getRadiusSphere() const
		{
			return radiusSphere;
		}
};

#endif /* SRC_SUPPORTOBJECTS_H_ */

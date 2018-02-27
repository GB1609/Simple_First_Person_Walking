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

		void setGuardPosition(glm::vec3 pos)
		{
			guardPosition = pos;
		}

		bool checkGuardCollision(glm::vec3 one, glm::vec3 two) // AABB - AABB collision
		{

			two.x += 30;
			two.z += 30;

			if (one.z > guardPosition.z + 100 || one.z < guardPosition.z - 100)
				return true;
			if (one.x > guardPosition.x + 100 || one.x < guardPosition.x - 100)
				return true;

			if (guardPosition.x > one.x && guardPosition.x < two.x)
				return false;
			if (guardPosition.z > one.z && guardPosition.z < two.z)
				return false;
			if (guardPosition.x < one.x && guardPosition.x > two.x)
				return false;
			if (guardPosition.x < one.z && guardPosition.z > two.z)
				return false;

			return true;
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
};

#endif /* SRC_SUPPORTOBJECTS_H_ */

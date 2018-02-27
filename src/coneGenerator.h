#ifndef CONEGENERATOR_H
#define CONEGENERATOR_H

#include <cmath>
class Cone
{

	public:
		float radius;
		void createCone(float* vertex, float* normal, float* texture, float lenght, int increment)
		{
			float factor = M_PI / 180;
			float toe[] =
			{ 0.0, lenght, 0.0f };
			double vertexA[3], vertexB[3];
			vertexA[1] = vertexB[1] = 0.0f;
			int cont = 0;
			int contT = 0;
			int a = 0;
			bool first = true;
			for (float angle = 0.0f; angle <= 360; angle += increment)
			{
				vertexB[0] = vertexA[0];
				vertexB[2] = vertexA[2];

				vertexA[0] = cos(angle * factor);
				vertexA[2] = sin(angle * factor);
				if (first)
				{
					int distancex = (vertexA[0]) * (vertexA[0]);
					int distancey = (vertexA[2]) * (vertexA[0]);

					float distance = sqrt(distancex - distancey);
					radius = distance;
					first = false;
				}
				if (angle != 0)
				{

					normal[cont] = toe[0] * (lenght / radius);
					vertex[cont++] = toe[0];
					normal[cont] = (lenght / radius);
					vertex[cont++] = toe[1];
					normal[cont] = toe[2] * (lenght / radius);
					vertex[cont++] = toe[2];
					normal[cont] = vertexA[0] * (lenght / radius);
					vertex[cont++] = vertexA[0];
					normal[cont] = (lenght / radius);
					vertex[cont++] = vertexA[1];
					normal[cont] = vertexA[2] * (lenght / radius);
					vertex[cont++] = vertexA[2];
					normal[cont] = vertexB[0] * (lenght / radius);
					vertex[cont++] = vertexB[0];
					normal[cont] = (lenght / radius);
					vertex[cont++] = vertexB[1];
					normal[cont] = vertexB[2] * (lenght / radius);
					vertex[cont++] = vertexB[2];
					texture[contT++] = 0.5f;
					texture[contT++] = 1.0f;
					texture[contT++] = 0.0f;
					texture[contT++] = 0.0f;
					texture[contT++] = 1.0f;
					texture[contT++] = 0.0f;
				}
			}
		}
		int dimensionCone(int increment)
		{
			return (360 / increment) * 9;
		}
		int numberTextureCone(int increment)
		{
			return (360 / increment) * 6;
		}
}
;

#endif

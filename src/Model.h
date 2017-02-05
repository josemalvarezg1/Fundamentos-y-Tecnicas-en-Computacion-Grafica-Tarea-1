#pragma once
#include "Main.h"

//Estructura de modelos
class model {
	public:
		GLuint vbo;
		GLuint vao;
		GLuint vindex;
		vector<float> vertices;
		vector<float> coord_texturas;
		vector<float> normales_vertice_fin;
		float rotacion[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float rotacionInicial[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float color_material_ambiental[4] = { 0.0f, 0.0f, 0.0f, 0.0 };
		float color_material_difuso[4] = { 1.0f, 1.0f, 1.0f, 0.0 };
		float color_material_especular[4] = { 1.0f, 1.0f, 1.0f, 0.0 };
		double scaleT = 10.00;
		double scaleX = 10.00;
		double scaleY = 10.00;
		double scaleZ = 10.00;
		float mat[4 * 4];
		float ejeX = 0.0;
		float ejeY = 1.0;
		float ejeZ = 0.0;

		float shinyPhong = 128.0;
		float shinyBlinn = 128.0;
		float F0 = 0.8;
		float albedoValue = 0.7;
		float roughnessOren = 0.9;
		float roughnessCook = 0.9;
		diffuse currentDiff = Lambert;
		specular currentSpec = PhongS;

		double minX = INT_MAX, minY = INT_MAX, minZ = INT_MAX, maxX = INT_MIN, maxY = INT_MIN, maxZ = INT_MIN;
		bool autoScalating = false;
		bool autoRotatingInX = false;
		bool autoRotatingInY = false;
		bool autoRotatingInZ = false;
		bool autoTranslatingInX = false;
		bool autoTranslatingInY = false;
		bool autoTranslatingInZ = false;
		int texturing = 1;

		void read_obj(char *filename);
		void initVBO(int size);
		unsigned int LoadTexture(const char* filename, bool text);
		int getTimeMs();
		void setQuat(const float *eje, float angulo, float *quat);
		void multiplicarQuat(const float *q1, const float *q2, float *qout);
		glm::mat4 scale_en_matriz(float scale_tx);
		glm::mat4 translate_en_matriz(float translate_tx, float translate_ty, float translate_tz);
		glm::mat4 rotacion_en_matriz(float rotacion_tx, float rotacion_ty, float rotacion_tz, float rotacion_ta);
		bool initGlew();
};

struct vertice {
	double x;
	double y;
	double z;
};

struct face {
	int n_vertex;
	vector<int> f;
	vector<int> t;
};
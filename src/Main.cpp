#include "Main.h"
#include "Model.h"
#include "Interface.h"

using namespace std;
//Para las normales y su offset
#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

int width = 1024, height = 768, selectedModel = 0, imageWidth, imageHeight, imageFormat, id, autoRotar = 0, tiempoRotacion = 0;
GLuint texture, frame = 0;
vector<const GLchar*> faces;
bool toggleMouse = false, toggleTexture = true, toggleLight = true, autoScalating = false, autoRotatingInX = false, autoRotatingInY = false, autoRotatingInZ = false, autoTranslatingInX = false, autoTranslatingInY = false, autoTranslatingInZ = false, firstMouse = true, selecting = false;
double xGlobal, yGlobal;
bool keys[1024]; //Todas las teclas				
Camera camera(glm::vec3(0.0f, 0.75f, 1.1f));
GLfloat lastX = 400, lastY = 300, deltaTime = 0.0f, lastFrame = 0.0f, currenttime = 0.0f, timebase = 0.0f;
model m;
Interface i;
TwBar *menuTW;
TwBar *modelTW;

//Variables globales a usar en AntTweakBar
float color_material_ambiental[] = { 0.0f, 0.0f, 0.0f, 0.0 };
float color_material_difuso[] = { 1.0f, 1.0f, 0.0f, 0.0 };
float color_material_especular[] = { 1.0f, 1.0f, 1.0f, 0.0 };
float color_luz_ambiental[] = { 0.0f, 0.0f, 0.0f, 0.0 };
float color_luz_difuso[] = { 1.0f, 1.0f, 1.0f, 0.0 };
float color_luz_especular[] = { 1.0f, 1.0f, 1.0f, 0.0 };
float shinyPhong = 128.0, shinyBlinn = 128.0, scaleT = 1.00, ejeX = 0.0, ejeY = 0.0, ejeZ = 0.0, ejeXL = 1.29, ejeYL = 1.13, ejeZL = 0.05, constant = 1.0, linear = 0.09, quadratic = 0.032, inCos = 0.91, outCos = 0.82, roughnessOren = 0.9, roughnessCook = 0.9, F0 = 0.8, albedoValue = 0.7;

shader currentShader = Phong;
diffuse currentDiff = Lambert;
specular currentSpec = PhongS;
lightning currentLight = Directional;
float rotacion[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float rotacionPrincipal[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float rotacionInicial[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDirection[] = { -1.31, -0.12, 1.10 };

CGLSLProgram glslProgram; //Programa de shaders

glm::mat4 project_mat; //Matriz de Proyección
glm::mat4 view_mat; //Matriz de View
glm::vec3 eye(0.0f, 0.0f, 2.0f); // Ojo

vector<model> models; //Todos los modelos irán en este vector

GLuint texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, texture10, texture11, texture12, texture13, texture14, texture15, texture16;

//Para separar un string
vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

void loadTextures() {
	const char* file = "Texturas/floor.jpg";
	texture1 = m.LoadTexture(file, true);

	file = "Texturas/adidas.jpg";
	texture2 = m.LoadTexture(file, true);

	file = "Texturas/mickey.jpg";
	texture3 = m.LoadTexture(file, true);

	file = "Texturas/flash.jpg";
	texture4 = m.LoadTexture(file, true);

	file = "Texturas/arrow.jpg";
	texture5 = m.LoadTexture(file, true);

	file = "Texturas/joker.jpg";
	texture6 = m.LoadTexture(file, true);

	file = "Texturas/spiderman.jpg";
	texture7 = m.LoadTexture(file, true);

	file = "Texturas/silver.jpg";
	texture8 = m.LoadTexture(file, true);

	file = "Texturas/ironman.jpg";
	texture9 = m.LoadTexture(file, true);

	file = "Texturas/wall.jpg";
	texture10 = m.LoadTexture(file, true);

	file = "Texturas/roof.jpg";
	texture11 = m.LoadTexture(file, true);

	file = "Texturas/mario.jpg";
	texture12 = m.LoadTexture(file, true);

	file = "Texturas/luigi.jpg";
	texture13 = m.LoadTexture(file, true);

	file = "Texturas/car.jpg";
	texture14 = m.LoadTexture(file, true);

	file = "Texturas/plant.jpg";
	texture15 = m.LoadTexture(file, true);

	file = "Texturas/transparent.jpg";
	texture16 = m.LoadTexture(file, true);
}

void reshape(GLFWwindow* window, int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	project_mat = glm::perspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);

	glm::vec3 eye(0.0f, 0.0f, 2.0f);
	glm::vec3 norm(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	view_mat = glm::lookAt(eye, norm, up);
	gluLookAt(eye[0], eye[1], eye[2], norm[0], norm[1], norm[2], up[0], up[1], up[2]);

	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0);
	glColor3f(1.0, 1.0, 1.0);
	TwWindowSize(w, h);
}

//Funcion que mueve la cámara con WASD y la deshabilita con T
void moverme() {
	if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_T]) toggleMouse = true;
	else toggleMouse = false;
}

//Función que dibuja el modelo VBO con texturas
void draw() {

	//Validaciones en AntTweakBar

	if (inCos < outCos) inCos = outCos;

	if (currentSpec == BlinnPhong) {
		TwDefine("Figura/Blinn-Phong visible=true");
		TwDefine("Figura/Phong visible=false");
	}
	else if (currentSpec == PhongS) {
		TwDefine("Figura/Phong visible=true");
		TwDefine("Figura/Blinn-Phong visible=false");
	}
	else {
		TwDefine("Figura/Phong visible=false");
		TwDefine("Figura/Blinn-Phong visible=false");
	}

	if (selectedModel == 20) {
		TwDefine("Figura/Phong visible=false");
		TwDefine("Figura/Blinn-Phong visible=false");
	}

	if (currentLight == Point) TwDefine("Figura/Puntual visible=true");
	else TwDefine("Figura/Puntual visible=false");

	if (currentLight == Spot) TwDefine("Figura/Reflector visible=true");
	else TwDefine("Figura/Reflector visible=false");

	if (currentDiff == OrenNayar) TwDefine("Figura/Oren-Nayar visible=true");
	else TwDefine("Figura/Oren-Nayar visible=false");

	if (currentSpec == CookTorrance) TwDefine("Figura/Cook-Torrance visible=true");
	else TwDefine("Figura/Cook-Torrance visible=false");

	if (selectedModel >= models.size()) selectedModel = models.size() - 1;

	//Se modifica la ubicación del objeto de la luz con respecto a la posición luz
	models[20].ejeX = ejeXL;
	models[20].ejeY = ejeYL;
	models[20].ejeZ = ejeZL;

	for (int i = 0; i<models.size(); i++) { //Para los modelos

		glStencilFunc(GL_ALWAYS, i, -1);
		glslProgram.enable();
		GLuint view_matr_loc = glslProgram.getLocation("view_matrix");
		GLuint model_matr_loc = glslProgram.getLocation("model_matrix");
		GLuint proj_matr_loc = glslProgram.getLocation("projection_matrix");
		GLuint light_loc = glslProgram.getLocation("lightPos");
		GLuint view_loc = glslProgram.getLocation("view");
		GLuint shinyPhong_loc = glslProgram.getLocation("shinyPhong");
		GLuint shinyBlinn_loc = glslProgram.getLocation("shinyBlinn");
		GLuint model = glslProgram.getLocation("modelNumber");
		GLuint interpol = glslProgram.getLocation("interpol");
		GLuint tipo_difuso = glslProgram.getLocation("tipo_difuso");
		GLuint tipo_especular = glslProgram.getLocation("tipo_especular");
		GLuint lightType_loc = glslProgram.getLocation("lightType");
		GLuint lightDir_loc = glslProgram.getLocation("lightSpotDir");
		GLuint texturing_loc = glslProgram.getLocation("texturing");
		GLuint F0Loc = glslProgram.getLocation("F0");
		GLuint roughOrenLoc = glslProgram.getLocation("roughnessOren");
		GLuint roughCookLoc = glslProgram.getLocation("roughnessCook");
		GLuint albLoc = glslProgram.getLocation("albedoValue");
		GLuint lightningLoc = glslProgram.getLocation("lightning");
		GLuint bb_loc = glslProgram.getLocation("bb");
		GLuint constantLoc = glslProgram.getLocation("constant");
		GLuint linearLoc = glslProgram.getLocation("linear");
		GLuint quadraticLoc = glslProgram.getLocation("quadratic");
		GLuint inCosLoc = glslProgram.getLocation("inCos");
		GLuint outCosLoc = glslProgram.getLocation("outCos");

		GLuint ambMat_loc = glslProgram.getLocation("color_material_ambiental");
		GLuint difMat_loc = glslProgram.getLocation("color_material_difuso");
		GLuint specMat_loc = glslProgram.getLocation("color_material_especular");

		GLuint ambLight_loc = glslProgram.getLocation("color_luz_ambiental");
		GLuint difLight_loc = glslProgram.getLocation("color_luz_difuso");
		GLuint specLight_loc = glslProgram.getLocation("color_luz_especular");

		glUniform3f(view_loc, 0.0, 0.0, 3.0);
		glUniform3f(lightDir_loc, lightDirection[0], lightDirection[1], lightDirection[2]);
		glUniform3f(light_loc, ejeXL, ejeYL, ejeZL);

		glUniform1f(shinyPhong_loc, models[i].shinyPhong);
		glUniform1f(shinyBlinn_loc, models[i].shinyBlinn);
		glUniform1i(lightType_loc, currentLight);
		glUniform1i(lightningLoc, toggleLight);
		glUniform1i(bb_loc, 0);

		glUniform1f(F0Loc, models[i].F0);
		glUniform1f(roughOrenLoc, models[i].roughnessOren);
		glUniform1f(roughCookLoc, models[i].roughnessCook);
		glUniform1f(albLoc, models[i].albedoValue);

		glUniform4f(ambMat_loc, models[i].color_material_ambiental[0], models[i].color_material_ambiental[1], models[i].color_material_ambiental[2], models[i].color_material_ambiental[3]);
		glUniform4f(difMat_loc, models[i].color_material_difuso[0], models[i].color_material_difuso[1], models[i].color_material_difuso[2], models[i].color_material_difuso[3]);
		glUniform4f(specMat_loc, models[i].color_material_especular[0], models[i].color_material_especular[1], models[i].color_material_especular[2], models[i].color_material_especular[3]);

		glUniform4f(ambLight_loc, color_luz_ambiental[0], color_luz_ambiental[1], color_luz_ambiental[2], color_luz_ambiental[3]);
		glUniform4f(difLight_loc, color_luz_difuso[0], color_luz_difuso[1], color_luz_difuso[2], color_luz_difuso[3]);
		glUniform4f(specLight_loc, color_luz_especular[0], color_luz_especular[1], color_luz_especular[2], color_luz_especular[3]);

		glUniform1f(constantLoc, constant);
		glUniform1f(linearLoc, linear);
		glUniform1f(quadraticLoc, quadratic);
		glUniform1f(inCosLoc, inCos);
		glUniform1f(outCosLoc, outCos);

		glUniform1i(interpol, currentShader);
		glUniform1i(tipo_difuso, models[i].currentDiff);
		glUniform1i(tipo_especular, models[i].currentSpec);
		glUniform1i(model, i);
		glUniform1i(texturing_loc, models[i].texturing);
		//Matrices de view y projection
		glm::mat4 model_mat;
		glm::vec3 norm(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		view_mat = glm::lookAt(eye, norm, up);
		view_mat = camera.GetViewMatrix();
		gluLookAt(eye[0], eye[1], eye[2], norm[0], norm[1], norm[2], up[0], up[1], up[2]);

		model_mat = m.translate_en_matriz(models[i].ejeX, models[i].ejeY, models[i].ejeZ);
		model_mat = model_mat * m.rotacion_en_matriz(models[i].rotacion[0], models[i].rotacion[1], models[i].rotacion[2], models[i].rotacion[3]);
		model_mat = model_mat * m.scale_en_matriz(models[i].scaleT);

		glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model_mat));
		glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat));
		project_mat = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 1000.0f);
		glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		if (i == 0 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture1);
		else if (i == 1 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture2);
		else if (i == 2 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture3);
		else if (i == 3 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture4);
		else if (i == 4 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture5);
		else if (i == 5 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture6);
		else if (i == 6 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture7);
		else if (i == 7 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture8);
		else if (i == 8 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture9);
		else if (i == 9 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture10);
		else if (i == 10 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture10);
		else if (i == 11 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture10);
		else if (i == 12 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture10);
		else if (i == 13 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture11);
		else if (i == 14 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture12);
		else if (i == 15 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture13);
		else if (i == 16 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture14);
		else if (i == 17 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture15);
		else if (i == 18 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture15);
		else if (i == 19 && models[i].texturing) glBindTexture(GL_TEXTURE_2D, texture15);

		glBindBuffer(GL_ARRAY_BUFFER, models[i].vbo);
		//Se bindean los vértices, normales y coordenadas de texturas
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(models[i].vertices.size() * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(models[i].vertices.size() * sizeof(float) + (models[i].coord_texturas.size() * sizeof(float))));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, models[i].vertices.size() / 3);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glslProgram.disable();

		glPushMatrix();
		glPopMatrix();

		//Se verifica si el objeto está auto-rotando
		if (models[i].autoRotatingInX || models[i].autoRotatingInY || models[i].autoRotatingInZ) {
			float eje[3] = { 0, 0, 0 };

			if (models[i].autoRotatingInX) eje[0] = 1;
			else eje[0] = 0;

			if (models[i].autoRotatingInY) eje[1] = 1;
			else eje[1] = 0;

			if (models[i].autoRotatingInZ) eje[2] = 1;
			else eje[2] = 0;

			float angulo = (float)(m.getTimeMs() - tiempoRotacion) / 1000.0f;
			float quat[4];
			m.setQuat(eje, angulo, quat);
			m.multiplicarQuat(models[i].rotacionInicial, quat, rotacion);

			models[i].rotacion[0] = rotacion[0];
			models[i].rotacion[1] = rotacion[1];
			models[i].rotacion[2] = rotacion[2];
			models[i].rotacion[3] = rotacion[3];
		}

		//Se verifica si el objeto está auto-trasladándose
		if (models[i].autoTranslatingInX || models[i].autoTranslatingInY || models[i].autoTranslatingInZ) {
			int t = ((float)(m.getTimeMs()) / 1000.0f - (int)(m.getTimeMs()) / 1000) * 10;

			if (models[i].autoTranslatingInX)
				if (t >= 0 && t < 5) models[i].ejeX += 0.00025;
				else models[i].ejeX -= 0.00025;

				if (models[i].autoTranslatingInY)
					if (t >= 0 && t < 5) models[i].ejeY += 0.00025;
					else models[i].ejeY -= 0.00025;

					if (models[i].autoTranslatingInZ)
						if (t >= 0 && t < 5) models[i].ejeZ += 0.00025;
						else models[i].ejeZ -= 0.00025;
		}

		//Se verifica si el objeto está auto-escalando
		if (models[i].autoScalating) {

			int t = ((float)(m.getTimeMs()) / 1000.0f - (int)(m.getTimeMs()) / 1000) * 10;
			if (t >= 0 && t < 5) models[i].scaleT += 0.00025;
			else models[i].scaleT -= 0.00025;

		}

		if (selecting && i == selectedModel) {

			models[selectedModel].shinyBlinn = shinyBlinn;
			models[selectedModel].shinyPhong = shinyPhong;
			models[selectedModel].albedoValue = albedoValue;
			models[selectedModel].F0 = F0;
			models[selectedModel].roughnessCook = roughnessCook;
			models[selectedModel].roughnessOren = roughnessOren;
			models[selectedModel].currentDiff = currentDiff;
			models[selectedModel].currentSpec = currentSpec;

			models[selectedModel].texturing = toggleTexture;
			models[selectedModel].autoRotatingInX = autoRotatingInX;
			models[selectedModel].autoRotatingInY = autoRotatingInY;
			models[selectedModel].autoRotatingInZ = autoRotatingInZ;
			models[selectedModel].autoTranslatingInX = autoTranslatingInX;
			models[selectedModel].autoTranslatingInY = autoTranslatingInY;
			models[selectedModel].autoTranslatingInZ = autoTranslatingInZ;
			models[selectedModel].autoScalating = autoScalating;

			models[selectedModel].color_material_ambiental[0] = color_material_ambiental[0];
			models[selectedModel].color_material_ambiental[1] = color_material_ambiental[1];
			models[selectedModel].color_material_ambiental[2] = color_material_ambiental[2];
			models[selectedModel].color_material_ambiental[3] = color_material_ambiental[3];

			models[selectedModel].color_material_difuso[0] = color_material_difuso[0];
			models[selectedModel].color_material_difuso[1] = color_material_difuso[1];
			models[selectedModel].color_material_difuso[2] = color_material_difuso[2];
			models[selectedModel].color_material_difuso[3] = color_material_difuso[3];

			models[selectedModel].color_material_especular[0] = color_material_especular[0];
			models[selectedModel].color_material_especular[1] = color_material_especular[1];
			models[selectedModel].color_material_especular[2] = color_material_especular[2];
			models[selectedModel].color_material_especular[3] = color_material_especular[3];

			if (selectedModel == 2) {	//Si se selecciona al modelo principal (Mickey)

				models[selectedModel].scaleT = scaleT;

				models[selectedModel].ejeX = ejeX;
				models[selectedModel].ejeY = ejeY;
				models[selectedModel].ejeZ = ejeZ;

				models[selectedModel].rotacion[0] = rotacionPrincipal[0];
				models[selectedModel].rotacion[1] = rotacionPrincipal[1];
				models[selectedModel].rotacion[2] = rotacionPrincipal[2];
				models[selectedModel].rotacion[3] = rotacionPrincipal[3];
				TwDefine("Figura/Objeto_principal visible=true");
				TwDefine("Figura/Animaciones_objeto_no_principal visible=false");
				TwDefine("Figura/Luz visible=false");
				TwDefine("Figura/Material visible=true");
				TwDefine("Figura/toggleTextures visible=true");
				TwDefine("Figura/Difuso visible=true");
				TwDefine("Figura/Especular visible=true");

			}
			else if (selectedModel == 20) { //Si se selecciona la luz

				TwDefine("Figura/Objeto_principal visible=false");
				TwDefine("Figura/Animaciones_objeto_no_principal visible=false");
				TwDefine("Figura/Luz visible=true");
				TwDefine("Figura/Material visible=false");
				TwDefine("Figura/toggleTextures visible=false");
				TwDefine("Figura/Difuso visible=false");
				TwDefine("Figura/Especular visible=false");

			}
			else {	//Si se selecciona algún objeto no principal

				TwDefine("Figura/Objeto_principal visible=false");
				TwDefine("Figura/Animaciones_objeto_no_principal visible=true");
				TwDefine("Figura/Material visible=true");
				TwDefine("Figura/Luz visible=false");
				TwDefine("Figura/toggleTextures visible=true");
				TwDefine("Figura/Difuso visible=true");
				TwDefine("Figura/Especular visible=true");

			}

			glDisable(GL_CULL_FACE);

			glLineWidth(2.0);

			glslProgram.enable();

			bb_loc = glslProgram.getLocation("bb");
			glUniform1i(bb_loc, 1);

			glBegin(GL_LINE_STRIP);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].minY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].minY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].minZ);
			glEnd();

			glBegin(GL_LINE_STRIP);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].minY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glEnd();

			glBegin(GL_LINE_STRIP);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].maxY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glEnd();

			glBegin(GL_LINE_STRIP);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].minY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].minY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glEnd();

			glBegin(GL_LINE_STRIP);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].maxY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].maxY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].maxZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].minZ);
			glEnd();

			glBegin(GL_LINE_STRIP);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].maxY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].maxY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].maxX, models[selectedModel].minY, models[selectedModel].minZ);
			glVertex3f(models[selectedModel].minX, models[selectedModel].minY, models[selectedModel].minZ);
			glEnd();

			glslProgram.disable();
		}
	}
	glDisable(GL_STENCIL_TEST);
}

//Función de display
void display() {
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	currenttime = currentFrame;
	lastFrame = currentFrame;
	glLoadIdentity();
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Dibujo la escena
	glslProgram.disable();
	draw();
	TwDraw();
	moverme();
}

int main(int argc, char* argv[]) {

	if (!glfwInit()) {
		std::cerr << "Error inicializando glfw...\n";
		return 1;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "F.C.G. Tarea #1 - Jose Manuel Alvarez - CI 25038805", nullptr, nullptr);
	if (!window) {
		std::cerr << "Error creando ventana...\n";
		glfwTerminate();
		return 0;
	}

	glfwSetFramebufferSizeCallback(window, reshape);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	TwInit(TW_OPENGL, NULL);
	m.initGlew();
	i.initAntTweakBar();
		
	glfwSetWindowSizeCallback(window, i.TwWindowSizeGLFW3);
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)i.TwEventMouseButtonGLFW3);
	glfwSetCursorPosCallback(window, i.cursorPos);
	glfwSetScrollCallback(window, i.cursorScroll);
	glfwSetKeyCallback(window, (GLFWkeyfun)i.TwEventKeyGLFW3);
	glfwSetCharCallback(window, (GLFWcharfun)i.TwEventCharGLFW3);
	reshape(window, 800, 600);
	glewInit();

	//Todos los objetos
	m.read_obj("Modelos/obj/floor.obj");
	m.read_obj("Modelos/obj/sphere.obj");
	m.read_obj("Modelos/obj/mickey.obj");
	m.read_obj("Modelos/obj/flash.obj");
	m.read_obj("Modelos/obj/arrow.obj");
	m.read_obj("Modelos/obj/joker.obj");
	m.read_obj("Modelos/obj/spiderman.obj");
	m.read_obj("Modelos/obj/silver.obj");
	m.read_obj("Modelos/obj/ironman.obj");
	m.read_obj("Modelos/obj/wall.obj");
	m.read_obj("Modelos/obj/wall.obj");
	m.read_obj("Modelos/obj/wall.obj");
	m.read_obj("Modelos/obj/wall.obj");
	m.read_obj("Modelos/obj/floor.obj");
	m.read_obj("Modelos/obj/mario.obj");
	m.read_obj("Modelos/obj/luigi.obj");
	m.read_obj("Modelos/obj/car.obj");
	m.read_obj("Modelos/obj/plant.obj");
	m.read_obj("Modelos/obj/plant.obj");
	m.read_obj("Modelos/obj/plant.obj");
	m.read_obj("Modelos/obj/sphere.obj");
	loadTextures();
	while (!glfwWindowShouldClose(window)) {
		//Este es el Main loop
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	TwTerminate();
	glfwTerminate();
	return 0;
}
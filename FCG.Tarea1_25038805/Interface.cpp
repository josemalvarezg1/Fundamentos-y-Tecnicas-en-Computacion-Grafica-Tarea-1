#include "Interface.h"

extern vector<model> models; //Todos los modelos irán en este vector
extern model m;
extern TwBar *menuTW, *modelTW;
extern Camera camera;
extern int width, height, selectedModel, autoRotar, tiempoRotacion;
extern bool toggleMouse, toggleTexture, toggleLight, autoScalating, autoRotatingInX, autoRotatingInY, autoRotatingInZ, autoTranslatingInX, autoTranslatingInY, autoTranslatingInZ, firstMouse, selecting;
extern double xGlobal, yGlobal;
extern bool keys[1024]; //Todas las teclas				
extern float rotacionInicial[4], rotacionPrincipal[4], lightDirection[4], color_material_ambiental[4], color_material_difuso[4], color_material_especular[4], color_luz_ambiental[4], color_luz_difuso[4], color_luz_especular[4], rotacion[4];
extern float shinyPhong, shinyBlinn, scaleT, ejeX, ejeY, ejeZ, ejeXL, ejeYL, ejeZL, constant, linear, quadratic, inCos, outCos, roughnessOren, roughnessCook, F0, albedoValue;
extern GLfloat lastX, lastY;
extern shader currentShader;
extern diffuse currentDiff;
extern specular currentSpec;
extern lightning currentLight;
extern glm::mat4 project_mat; //Matriz de Proyección
extern glm::mat4 view_mat; //Matriz de View

void Interface::TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods) {
	if (TwEventMouseButtonGLFW(button, action)) return;
	if (action == GLFW_PRESS) {
		GLint index;
		glReadPixels(xGlobal, height - yGlobal, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
		bool ISelected = false;

		if (index > 0 && !(index >= 9 && index <= 13)) { //Validando que no se seleccionen ni el piso, ni las paredes, ni el techo
			ISelected = true;
			//Se absorben las propiedades y se cambia el menú de AntTweakBar
			selectedModel = index;
			TwDefine("Menú visible=false");
			TwDefine("Figura visible=true");
			selecting = true;
			rotacion[0] = models[selectedModel].rotacion[0];
			rotacion[1] = models[selectedModel].rotacion[1];
			rotacion[2] = models[selectedModel].rotacion[2];
			rotacion[3] = models[selectedModel].rotacion[3];
			scaleT = models[selectedModel].scaleT;

			toggleTexture = models[selectedModel].texturing;
			autoScalating = models[selectedModel].autoScalating;
			autoRotatingInX = models[selectedModel].autoRotatingInX;
			autoRotatingInY = models[selectedModel].autoRotatingInY;
			autoRotatingInZ = models[selectedModel].autoRotatingInZ;
			autoTranslatingInX = models[selectedModel].autoTranslatingInX;
			autoTranslatingInY = models[selectedModel].autoTranslatingInY;
			autoTranslatingInZ = models[selectedModel].autoTranslatingInZ;

			shinyBlinn = models[selectedModel].shinyBlinn;
			shinyPhong = models[selectedModel].shinyPhong;
			albedoValue = models[selectedModel].albedoValue;
			F0 = models[selectedModel].F0;
			roughnessCook = models[selectedModel].roughnessCook;
			roughnessOren = models[selectedModel].roughnessOren;
			currentDiff = models[selectedModel].currentDiff;
			currentSpec = models[selectedModel].currentSpec;

			ejeX = models[selectedModel].ejeX;
			ejeY = models[selectedModel].ejeY;
			ejeZ = models[selectedModel].ejeZ;

			color_material_ambiental[0] = models[selectedModel].color_material_ambiental[0];
			color_material_ambiental[1] = models[selectedModel].color_material_ambiental[1];
			color_material_ambiental[2] = models[selectedModel].color_material_ambiental[2];
			color_material_ambiental[3] = models[selectedModel].color_material_ambiental[3];

			color_material_difuso[0] = models[selectedModel].color_material_difuso[0];
			color_material_difuso[1] = models[selectedModel].color_material_difuso[1];
			color_material_difuso[2] = models[selectedModel].color_material_difuso[2];
			color_material_difuso[3] = models[selectedModel].color_material_difuso[3];

			color_material_especular[0] = models[selectedModel].color_material_especular[0];
			color_material_especular[1] = models[selectedModel].color_material_especular[1];
			color_material_especular[2] = models[selectedModel].color_material_especular[2];
			color_material_especular[3] = models[selectedModel].color_material_especular[3];

		}
		
		if (!ISelected) {
			selecting = false;
			TwDefine("Menú visible=true");
			TwDefine("Figura visible=false");
		}
	}
}

void Interface::cursorPos(GLFWwindow* window, double x, double y) {
	xGlobal = x;
	yGlobal = y;
	if (TwEventMousePosGLFW(x, y)) {
		lastX = x;
		lastY = y;
		return;
	}
	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}
	GLfloat xoffset = x - lastX;
	GLfloat yoffset = lastY - y;
	lastX = x;
	lastY = y;
	if (!toggleMouse) camera.ProcessMouseMovement(xoffset, yoffset);
}

void Interface::TwEventMousePosGLFW3(GLFWwindow* window, double x, double y) {
	TwMouseMotion(int(x), int(y));
}

void Interface::cursorScroll(GLFWwindow* window, double xoffset, double yoffset) {
	if (TwEventMouseWheelGLFW(yoffset)) return;
	//Para escalar el objeto principal cuando es seleccionado
	if (yoffset == 1 && selecting && selectedModel == 2) {
		scaleT += 0.01;
	}
	if (yoffset == -1 && selecting && selectedModel == 2) {
		scaleT -= 0.01;
	}
}

void Interface::TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods) {
	TwEventKeyGLFW(key, action);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)	glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}
}

void Interface::TwEventCharGLFW3(GLFWwindow* window, int codepoint) {
	TwEventCharGLFW(codepoint, GLFW_PRESS);
}

void Interface::TwWindowSizeGLFW3(GLFWwindow* window, int width, int height) {
	TwWindowSize(width, height);
}


//Función de salir
void TW_CALL exit(void *clientData) {
	exit(1);
}

//Función de activar el AutoRotate
void TW_CALL SetAutoRotateCB(const void *value, void *clientData) {
	autoRotar = *(const int *)value;
	if (autoRotar != 0) {
		tiempoRotacion = m.getTimeMs();
		rotacionInicial[0] = rotacion[0];
		rotacionInicial[1] = rotacion[1];
		rotacionInicial[2] = rotacion[2];
		rotacionInicial[3] = rotacion[3];
	}
}

//Función de AutoRotate
void TW_CALL GetAutoRotateCB(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoRotar;
}

//Activar o desactivar texturas
void TW_CALL setTexture(const void *value, void *clientData) {
	toggleTexture = *(const int *)value;
}

void TW_CALL getTexture(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = toggleTexture;
}

//Activar o desactivar auto-rotación en X 
void TW_CALL setRotateX(const void *value, void *clientData) {
	autoRotatingInX = *(const int *)value;
}

void TW_CALL getRotateX(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoRotatingInX;
}

//Activar o desactivar auto-rotación en Y
void TW_CALL setRotateY(const void *value, void *clientData) {
	autoRotatingInY = *(const int *)value;
}

void TW_CALL getRotateY(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoRotatingInY;
}

//Activar o desactivar auto-rotación en Z
void TW_CALL setRotateZ(const void *value, void *clientData) {
	autoRotatingInZ = *(const int *)value;
}

void TW_CALL getRotateZ(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoRotatingInZ;
}

//Activar o desactivar auto-traslación en X
void TW_CALL setTranslateX(const void *value, void *clientData) {
	autoTranslatingInX = *(const int *)value;
}

void TW_CALL getTranslateX(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoTranslatingInX;
}

//Activar o desactivar auto-traslación en Y
void TW_CALL setTranslateY(const void *value, void *clientData) {
	autoTranslatingInY = *(const int *)value;
}

void TW_CALL getTranslateY(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoTranslatingInY;
}

//Activar o desactivar auto-traslación en Z
void TW_CALL setTranslateZ(const void *value, void *clientData) {
	autoTranslatingInZ = *(const int *)value;
}

void TW_CALL getTranslateZ(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoTranslatingInZ;
}

//Activar o desactivar auto-escalamiento
void TW_CALL setScale(const void *value, void *clientData) {
	autoScalating = *(const int *)value;
}

void TW_CALL getScale(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = autoScalating;
}

//Activar o desactivar la luz
void TW_CALL setLight(const void *value, void *clientData) {
	toggleLight = *(const int *)value;
}

void TW_CALL getLight(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = toggleLight;
}

//Función para seleciconar un modelo
void TW_CALL selectModel(void *clientData) {
	if (models.size() > 0) {
		if (!selecting) {
			TwDefine("Menú visible=false");
			TwDefine("Figura visible=true");
			selecting = true;
			rotacion[0] = models[selectedModel].rotacion[0];
			rotacion[1] = models[selectedModel].rotacion[1];
			rotacion[2] = models[selectedModel].rotacion[2];
			rotacion[3] = models[selectedModel].rotacion[3];
			scaleT = models[selectedModel].scaleT;

			toggleTexture = models[selectedModel].texturing;
			autoScalating = models[selectedModel].autoScalating;
			autoRotatingInX = models[selectedModel].autoRotatingInX;
			autoRotatingInY = models[selectedModel].autoRotatingInY;
			autoRotatingInZ = models[selectedModel].autoRotatingInZ;
			autoTranslatingInX = models[selectedModel].autoTranslatingInX;
			autoTranslatingInY = models[selectedModel].autoTranslatingInY;
			autoTranslatingInZ = models[selectedModel].autoTranslatingInZ;

			shinyBlinn = models[selectedModel].shinyBlinn;
			shinyPhong = models[selectedModel].shinyPhong;
			albedoValue = models[selectedModel].albedoValue;
			F0 = models[selectedModel].F0;
			roughnessCook = models[selectedModel].roughnessCook;
			roughnessOren = models[selectedModel].roughnessOren;
			currentDiff = models[selectedModel].currentDiff;
			currentSpec = models[selectedModel].currentSpec;

			ejeX = models[selectedModel].ejeX;
			ejeY = models[selectedModel].ejeY;
			ejeZ = models[selectedModel].ejeZ;

			color_material_ambiental[0] = models[selectedModel].color_material_ambiental[0];
			color_material_ambiental[1] = models[selectedModel].color_material_ambiental[1];
			color_material_ambiental[2] = models[selectedModel].color_material_ambiental[2];
			color_material_ambiental[3] = models[selectedModel].color_material_ambiental[3];

			color_material_difuso[0] = models[selectedModel].color_material_difuso[0];
			color_material_difuso[1] = models[selectedModel].color_material_difuso[1];
			color_material_difuso[2] = models[selectedModel].color_material_difuso[2];
			color_material_difuso[3] = models[selectedModel].color_material_difuso[3];

			color_material_especular[0] = models[selectedModel].color_material_especular[0];
			color_material_especular[1] = models[selectedModel].color_material_especular[1];
			color_material_especular[2] = models[selectedModel].color_material_especular[2];
			color_material_especular[3] = models[selectedModel].color_material_especular[3];
		}
		else {
			TwDefine("Menú visible=true");
			TwDefine("Figura visible=false");
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
			selecting = false;
		}
	}
}

void Interface::initAntTweakBar() {

	menuTW = TwNewBar("Menú");
	TwDefine("Menú size='270 250' position='20 20' color='128 0 0' label='F.C.G. Tarea #1 - Jose M. Alvarez'");
	TwAddButton(menuTW, "exit", exit, NULL, " label='Salir' key=Esc");

	modelTW = TwNewBar("Figura");
	TwWindowSize(200, 400);
	TwDefine("Figura visible=false size='270 620' position='20 20' color='128 0 0' label='Objeto'");

	TwAddButton(modelTW, "select", selectModel, NULL, " label='Volver al Menú' key=m");
	{
		TwEnumVal styleShader[2] = { { Gouraud, "Gouraud" },{ Phong, "Phong" } };
		TwType shader = TwDefineEnum("ShaderType", styleShader, 2);
		TwAddVarRW(modelTW, "Interpolación", shader, &currentShader, "");
	}
	{
		TwEnumVal styleDiffuse[2] = { { Lambert, "Lambert" },{ OrenNayar, "Oren-Nayar" } };
		TwType diff = TwDefineEnum("DiffType", styleDiffuse, 2);
		TwAddVarRW(modelTW, "Difuso", diff, &currentDiff, "");
	}

	TwAddVarRW(modelTW, "albedo", TW_TYPE_FLOAT, &albedoValue, "min=0.01 step=0.01 max=1.0 label='Albedo' group='Oren-Nayar'");
	TwAddVarRW(modelTW, "roughnessOren", TW_TYPE_FLOAT, &roughnessOren, "min=0.01 step=0.01 max=1.0 label='Rugosidad' group='Oren-Nayar'");

	{
		TwEnumVal styleSpecular[3] = { { PhongS, "Phong" },{ BlinnPhong, "Blinn-Phong" },{ CookTorrance, "Cook-Torrance" } };
		TwType spec = TwDefineEnum("SpecType", styleSpecular, 3);
		TwAddVarRW(modelTW, "Especular", spec, &currentSpec, "");
	}

	TwAddVarRW(modelTW, "F0", TW_TYPE_FLOAT, &F0, "min=0.01 step=0.01 max=1.0 label='Fresnel' group='Cook-Torrance'");
	TwAddVarRW(modelTW, "roughnessCook", TW_TYPE_FLOAT, &roughnessCook, "min=0.01 step=0.01 max=1.0 label='Rugosidad' group='Cook-Torrance'");

	TwAddVarRW(modelTW, "BrilloPhong", TW_TYPE_FLOAT, &shinyPhong, "min=1.0 max=400.0 step=1.0 label='Brillo' group='Phong'");
	TwAddVarRW(modelTW, "BrilloBlinn", TW_TYPE_FLOAT, &shinyBlinn, "min=1.0 max=400.0 step=1.0 label='Brillo' group='Blinn-Phong'");

	TwAddVarCB(modelTW, "toggleTextures", TW_TYPE_BOOL32, setTexture, getTexture, NULL, " label='Textura'");

	TwAddVarCB(modelTW, "toggleRotateX", TW_TYPE_BOOL32, setRotateX, getRotateX, NULL, " label='Auto-rotación en X' group='Animaciones_objeto_no_principal'");
	TwAddVarCB(modelTW, "toggleRotateY", TW_TYPE_BOOL32, setRotateY, getRotateY, NULL, " label='Auto-rotación en Y' group='Animaciones_objeto_no_principal'");
	TwAddVarCB(modelTW, "toggleRotateZ", TW_TYPE_BOOL32, setRotateZ, getRotateZ, NULL, " label='Auto-rotación en Z' group='Animaciones_objeto_no_principal'");
	TwAddVarCB(modelTW, "toggleTranslateX", TW_TYPE_BOOL32, setTranslateX, getTranslateX, NULL, " label='Auto-traslación en X' group='Animaciones_objeto_no_principal'");
	TwAddVarCB(modelTW, "toggleTranslateY", TW_TYPE_BOOL32, setTranslateY, getTranslateY, NULL, " label='Auto-traslación en Y' group='Animaciones_objeto_no_principal'");
	TwAddVarCB(modelTW, "toggleTranslateZ", TW_TYPE_BOOL32, setTranslateZ, getTranslateZ, NULL, " label='Auto-traslación en Z' group='Animaciones_objeto_no_principal'");
	TwAddVarCB(modelTW, "toggleScaleX", TW_TYPE_BOOL32, setScale, getScale, NULL, " label='Auto-escalamiento' group='Animaciones_objeto_no_principal'");

	TwAddVarRW(modelTW, "scale", TW_TYPE_FLOAT, &scaleT, "min=0.01 step=0.01 label='Escalar' group='Objeto_principal'");
	TwAddVarRW(modelTW, "ejeX", TW_TYPE_FLOAT, &ejeX, "step=0.01 label='Traslación x' group='Objeto_principal'");
	TwAddVarRW(modelTW, "ejeY", TW_TYPE_FLOAT, &ejeY, "step=0.01 label='Traslación y' group='Objeto_principal'");
	TwAddVarRW(modelTW, "ejeZ", TW_TYPE_FLOAT, &ejeZ, "step=0.01 label='Traslación z' group='Objeto_principal'");
	TwAddVarRW(modelTW, "rotation", TW_TYPE_QUAT4F, &rotacionPrincipal, " label='Rotación' opened=true group='Objeto_principal'");

	TwAddVarRW(modelTW, "Color AmbientalM", TW_TYPE_COLOR3F, &color_material_ambiental, "label='Color Ambiental' group='Material'");
	TwAddVarRW(modelTW, "Color DifusoM", TW_TYPE_COLOR3F, &color_material_difuso, "label='Color Difuso' group='Material'");
	TwAddVarRW(modelTW, "Color EspecularM", TW_TYPE_COLOR3F, &color_material_especular, "label='Color Especular' group='Material'");

	TwAddVarCB(modelTW, "toggleLight", TW_TYPE_BOOL32, setLight, getLight, NULL, " label='Prender/Apagar' group='Luz'");
	{
		TwEnumVal styleLight[3] = { { Directional, "Direccional" },{ Point, "Puntual" },{ Spot, "Reflector" } };
		TwType light = TwDefineEnum("LightType", styleLight, 3);
		TwAddVarRW(modelTW, "Tipo de luz", light, &currentLight, "group='Luz'");
	}

	TwAddVarRW(modelTW, "constantP", TW_TYPE_FLOAT, &constant, "min=0.001 max=1.0 step=0.001 label='Constante' group='Puntual'");
	TwAddVarRW(modelTW, "linearP", TW_TYPE_FLOAT, &linear, "min=0.001 max=1.0 step=0.001 label='Lineal' group='Puntual'");
	TwAddVarRW(modelTW, "quadraticP", TW_TYPE_FLOAT, &quadratic, "min=0.001 max=1.8 step=0.001 label='Cuadrática' group='Puntual'");

	TwAddVarRW(modelTW, "inCos", TW_TYPE_FLOAT, &inCos, "min=0.001 max=1.0 step=0.001 label='Coseno interno' group='Reflector'");
	TwAddVarRW(modelTW, "outCos", TW_TYPE_FLOAT, &outCos, "min=0.001 max=1.0 step=0.001 label='Coseno externo' group='Reflector'");
	TwAddVarRW(modelTW, "constantR", TW_TYPE_FLOAT, &constant, "min=0.001 max=1.0 step=0.001 label='Constante' group='Reflector'");
	TwAddVarRW(modelTW, "linearR", TW_TYPE_FLOAT, &linear, "min=0.001 max=1.0 step=0.001 label='Lineal' group='Reflector'");
	TwAddVarRW(modelTW, "quadraticR", TW_TYPE_FLOAT, &quadratic, "min=0.001 max=1.8 step=0.001 label='Cuadrática' group='Reflector'");
	TwAddVarRW(modelTW, "LightDir", TW_TYPE_DIR3F, &lightDirection, " label='Dirección' opened=true group='Reflector'");

	TwAddVarRW(modelTW, "ejeXL", TW_TYPE_FLOAT, &ejeXL, "step=0.01 label='x' group='Trasladar luz' group='Luz'");
	TwAddVarRW(modelTW, "ejeYL", TW_TYPE_FLOAT, &ejeYL, "step=0.01 label='y' group='Trasladar luz' group='Luz'");
	TwAddVarRW(modelTW, "ejeZL", TW_TYPE_FLOAT, &ejeZL, "step=0.01 label='z' group='Trasladar luz' group='Luz'");
	TwAddVarRW(modelTW, "Color AmbientalL", TW_TYPE_COLOR3F, &color_luz_ambiental, "label='Color Ambiental' group='Luz'");
	TwAddVarRW(modelTW, "Color DifusoL", TW_TYPE_COLOR3F, &color_luz_difuso, "label='Color Difuso' group='Luz'");
	TwAddVarRW(modelTW, "Color EspecularL", TW_TYPE_COLOR3F, &color_luz_especular, "label='Color Especular' group='Luz'");
	TwAddButton(modelTW, "exitF", exit, NULL, " label='Salir' key=Esc");
}
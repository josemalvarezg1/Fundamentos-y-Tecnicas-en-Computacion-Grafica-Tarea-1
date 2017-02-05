#version 330

uniform mat4 view_matrix; //Matriz de View
uniform mat4 projection_matrix; //Matriz de proyeccion
uniform mat4 model_matrix; //Matriz de model
uniform vec3 lightPos; //Posicion de luz
uniform vec3 view; //Vector de EYE
uniform int modelNumber; //Numero del modelo
uniform int interpol; //Tipo de interpolacion
uniform int tipo_difuso; //Tipo de modelo difuso
uniform int tipo_especular; //Tipo de modelo especular
uniform int lightType;
uniform vec3 lightSpotDir;
uniform bool lightning;
uniform bool bb;
uniform float F0;
uniform float roughnessCook;
uniform float roughnessOren;
uniform float shinyBlinn;
uniform float shinyPhong;
uniform float albedoValue;
uniform float constant;
uniform float linear;
uniform float quadratic;
uniform float inCos;
uniform float outCos;

uniform vec4 color_material_ambiental;
uniform vec4 color_material_difuso;
uniform vec4 color_material_especular;

uniform vec4 color_luz_ambiental;
uniform vec4 color_luz_difuso;
uniform vec4 color_luz_especular;

uniform sampler2D texture;

layout (location=0) in vec3 position; //Posicion de los vertices
layout (location=1) in vec2 coord_texturas;
layout (location=2) in vec3 normal; //Posicion de normales

out vec4 colorVertex;
out vec3 FragPos;
out vec3 normales;
out vec3 viewPos;
out vec2 coordTex;
out float brillo;
out vec3 lightPosfrag;



void main() {

	//Posicion de los vertices
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0); //La posicion viene por la mult de matrices
	coordTex = coord_texturas;
	mat4 model = model_matrix;

	FragPos = vec3(model * vec4(position, 1.0));
	lightPosfrag = lightPos;
	viewPos = view;
	normales = mat3(transpose(inverse(model))) * normal;
	if (interpol == 1) {
		
		if (!lightning) {
			colorVertex = vec4(0.0, 0.0, 0.0, 0.0);
		} else {

			vec4 result = vec4(0.0, 0.0, 0.0, 0.0);

			vec3 norm = normalize(normales);
			vec3 lightDir = normalize(lightPosfrag - FragPos);
		
			//Refleccion Ambiental
			
			vec4 ambient = color_material_ambiental;

			//Refleccion Difusa

			vec4 diffuse = vec4(0.0, 0.0, 0.0, 0.0);

			if (tipo_difuso == 1) {	//Lambert		
				float diff = max(dot(norm, lightDir), 0.0);
				diffuse = vec4(diff,diff,diff,1.0);
			}

			if (tipo_difuso == 2) {	//Oren-Nayar		
				vec3 viewDir = normalize(viewPos - FragPos);
				float LdotV = dot(lightDir, viewDir);
				float NdotL = dot(lightDir, norm);
				float NdotV = dot(norm, viewDir);
				float s = LdotV - NdotL * NdotV;
				float t = 1;
				if (s > 0) t = max(NdotL, NdotV);
				float sigma = roughnessOren * roughnessOren;
				float A = 1.0 + sigma * (albedoValue / (sigma + 0.13) + 0.5 / (sigma + 0.33));
				float B = 0.45 * sigma / (sigma + 0.09);
				float diff = albedoValue * max(0.0, NdotL) * (A + B * s / t) / 3.141593;
				diffuse = vec4(diff,diff,diff,1.0);
			}

			//Refleccion Especular

			vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);
			float intensity = 0.0;

			if (tipo_especular == 1) {	//Phong
				vec3 viewDir = normalize(viewPos - FragPos);
				vec3 reflectDir = reflect(-lightDir, norm);
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), shinyPhong);
				specular = vec4(spec,spec,spec,1.0);
			} else

			if (tipo_especular == 2) {	//Blinn-Phong
				vec3 viewDir = normalize(viewPos - FragPos);
				vec3 halfwayDir = normalize(lightDir + viewDir);
				float spec = pow(max(dot(norm, halfwayDir), 0.0), shinyBlinn);
				specular = vec4(spec,spec,spec,1.0);
			} else

			if (tipo_especular == 3) {	//Cook-Torrance   
				vec3 viewDirection = normalize(viewPos - FragPos);
				float VdotN = max(dot(viewDirection, norm), 0.0);
				float LdotN = max(dot(lightDir, norm), 0.0);
				vec3 halfwayDir = normalize(lightDir + viewDirection);
				float NdotH = max(dot(norm, halfwayDir), 0.0);
				float VdotH = max(dot(viewDirection, halfwayDir), 0.000001);
				float LdotH = max(dot(lightDir, halfwayDir), 0.000001);
				float Gb = (2*NdotH*VdotN)/(VdotH);
				float Gc = (2*NdotH*LdotN)/(LdotH);
				float G = min(1.0, min(Gb, Gc));			  
				float r1 = 1.0 / ( 4.0 * roughnessCook * roughnessCook * pow(NdotH, 4.0));
			    float r2 = (NdotH * NdotH - 1.0) / (roughnessCook * roughnessCook * NdotH * NdotH);
			    float D = r1 * exp(r2);
				float F = F0 + ((1 - F0) * pow((1 - VdotH), 5));
				float spec = G * F * D / max(3.141593 * VdotN * LdotN, 0.000001); //Para evitar que se divida entre cero
				specular = vec4(spec,spec,spec,1.0);
			}

			if (lightType == 1) {	//Luz Direccional

				//No se modifican los parámetros
							
			} else if (lightType == 2) {	//Luz puntual 

				//Atenuación
				float distance = length(lightPosfrag - FragPos);
				float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));    
							
				ambient  *= attenuation;  
				diffuse  *= attenuation;
				specular *= attenuation;  

			} else if (lightType == 3) {	//Luz reflector

				float theta = dot(lightDir, normalize(-lightSpotDir)); 
				float epsilon = (inCos - outCos);	
				intensity = clamp((theta - outCos) / epsilon, 0.0, 1.0);			

				ambient  *= intensity;
				diffuse  *= intensity;
				specular *= intensity;
	    
				//Atenuación
				float distance = length(lightPosfrag - FragPos);
				float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));    

				ambient  *= attenuation;
				diffuse  *= attenuation;
				specular *= attenuation;
			}	  
			    
			//Resultado final
			result = (color_luz_ambiental * ambient + color_luz_difuso * color_material_difuso * diffuse + color_luz_especular * color_material_especular * specular) ;
			if (bb) colorVertex = vec4(0.0, 0.0, 0.0, 1.0);
	    	else colorVertex = result;

			if (modelNumber == 20) {
				
				if (bb) colorVertex = vec4(1.0, 1.0, 1.0, 1.0);
				else {

					if (lightType == 1 || lightType == 2) colorVertex = color_luz_especular * color_luz_difuso;
					else colorVertex = color_luz_especular * color_luz_difuso * intensity ;

				}

			}

		}

	}

}
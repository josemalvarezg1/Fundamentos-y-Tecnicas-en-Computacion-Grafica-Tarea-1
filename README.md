# Tarea 1 - Fundamentos y Técnicas en Computación Gráfica

## Tabla de contenido

* [Escena](#escena)
* [Selección de objetos](#selección)
* [Herramientas](#herramientas)
* [Instalación y uso](#instalación)
* [Creador](#creador)


# Escena 

La escena de esta tarea está compuesta por 20 objetos texturizados:
  - 2 planos que representan al piso y al techo.
  - 4 planos adicionales que representan las paredes.
  - 13 modelos de personajes o modelos 3D.
  - 1 esfera que indica de dónde proviene la luz.

Para desplazarse en la escena se pueden utilizar las teclas W, A, S, D y el ratón para mover la cámara. Si por comodidad se desea desactivar la cámara del ratón, se debe mantener presionada la tecla T. 

El objeto principal es el personaje de Mickey Mouse, ubicado en el centro de la escena. Los demás objetos, que no son principales, tendrán diferentes animaciones, exceptuando a los 6 planos y a la esfera de luz. 

El tipo de interpolación por vértice o por fragmento para toda la escena puede cambiarse en cualquier momento en el que se seleccione algún modelo.


# Selección de objetos

Cada objeto puede ser seleccionado con el click (a excepción de los planos ya que no tienen animaciones automáticas ni son objetos principales). 

Al seleccionar al modelo principal se le pueden aplicar libremente transformaciones como rotación, traslación y escalamiento. Este último se puede ajustar con la rueda del ratón. 

Al seleccionar un modelo no principal se le pueden desactivar sus animaciones autómaticas o activar y desactivar cualquiera de las otras que se muestran en el menú. 

A cada objeto se le puede cambiar el modelo de iluminación difuso y especular con sus respectivos parámetros, sus colores ambiental, difuso y especular, y activar o desactivar su textura asociada.  

Al seleccionar la esfera de luz se puede cambiar la ubicación de la luz, encenderla y apagarla, sus colores ambiental, difuso y especular, y su tipo. Para cada tipo de luz se mostrarán sus respectivos parámetros.


# Herramientas

En la presente tarea se utilizaron las siguientes herramientas con sus respectivas versiones:

| Herramienta                         	 | Versión   													   |                            
|----------------------------------------|-----------------------------------------------------------------|
| Microsoft Visual Studio        	 	 | 2015      													   |


# Instalación y uso
Se deberá descargar el siguiente repositorio o clonarlo con el comando:

git clone https://github.com/josemalvarezg1/Fundamentos-y-Tecnicas-en-Computacion-Grafica-Tarea-1.git

Se puede abrir el archivo .sln en Visual Studio o ejecutar directamente el proyecto desde la carpeta bin/.

![alt tag](http://i.imgur.com/8F5WiKa.png)


# Creador

**José Manuel Alvarez García - CI 25038805**
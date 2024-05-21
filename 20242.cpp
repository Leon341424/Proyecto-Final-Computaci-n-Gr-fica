#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <irrKlang/irrKlang.h>			//Audio

#include <shader_m.h>
#include <camera.h>
//#include <cameraXZ.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

// Cambiar musica
irrklang::ISoundEngine* SoundEngine;
bool	switchMusica = false;

//sonic
float	rotacionSonic = 0.0f,
		orientacionSonic = 180.0f;
int adelanteSonic = 1,
pasitos = 0;

//Camera
Camera camera(glm::vec3(0.0f, 20.0f, 120.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
		lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la práctica 6
unsigned int generateTextures(char*, bool, bool);	// De la práctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Variables Helicoptero
float	rotHAspas = 0.0f,
		banderaHelicoptero = 0.0f,
		orientacionHelicoptero = -90.0f,
		mov_Helip_x = 400.0f,
		mov_Helip_z = 400.0f;
int		heliAdelante = 1,
		heliIzquierda = 0;

//Variables Botnik
float	botnikRotate = 0.0f,
		botnikAlas = 45.0f;
int		arribaAlasBotnik = 1;

//Variables Nudillos
float	orientacionNudillos = 0.0f,
		orientacionAlcantarilla = 0.0f,
		MovY_Nudillos = -18.0f;

//Animacion Eggman
float	movEggman_x = 0.0f,
		movEggman_y = 0.0f,
		movEggman_z = 0.0f,
		rotEggman = 0.0f,
		orientaEggman;
bool	animEggman = false;
int		volando = 1,
		adelanteEggman = 1;
//Animacion Tiro Parabólico Eggman
float	g = 9.81f, v = 60.0f, ang = 60.0f, t = 45.0f;
double	n = 3.1416;
float	i = 0.0f;

//Animacion moto
float	movMoto_x = -450.0f,
movMoto_z = 250.0f,
girarLlantaMoto = 0.0f,
orientacionMoto = 90.0f,
caballito = 0.0f;
int		motoAdelante = 1,
motoGiro = 0;  
boolean ruedas = false;

//Animacion carro
float	movAutoSonic_x = -450.0f,
movAutoSonic_z = -250.0f,
girarLlantaCarro = 0.0f,
orientacionCarro = 90.0f;
boolean ruedasAuto = false,
derrape = false;
int carroAdelante = 1,
carroGiro = 0,
derrapeSonido = 0;

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_ladrillos,
t_piso,
t_pista,
t_carretera,
t_edificio,
t_metal,
t_puertaOficina;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, 400.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

glm::vec3 prenderluz = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 luz1 = glm::vec3(0.0f, 0.0f, 0.0f);
bool luzencedida1, luzencedida2, luzencedida3, luzencedida4, luzencedida5, luzencedida6 = false;
float light0, light1, light2, light0_2, light0_3, light0_4, light0_5, light0_6 = 0.0f;

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


//Animaciones
bool	animacion = false,
animacion2 = false,
animacion3 = false,
animacionBotnik = false,
animacionNudillos = false;


#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;



unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	
	if(isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{

	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	t_piso = generateTextures("Texturas/piso.jpg", 0, true);
	t_pista = generateTextures("Texturas/pistacomp.jpg", 0, true);
	t_carretera = generateTextures("Texturas/carreteras.jpg", 0, true);
	t_edificio = generateTextures("Texturas/Edificio.png", 0, true);
	t_metal = generateTextures("Texturas/metal.jpg", 0, true);
	t_puertaOficina = generateTextures("Texturas/puertaOficina.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}



void animate(void) 
{
	//Animación carro sonic
	if (ruedasAuto) {
		girarLlantaCarro += 20;
	}
	if (derrape) {
		orientacionCarro += 3.3;
	}
	if (animacion)
	{
		if (carroAdelante == 1 && carroGiro == 0) {
			if (movAutoSonic_x <= -285.0f) {
				movAutoSonic_x += 4.0f;
			}
			else {
				orientacionCarro -= 90.0f;
				carroAdelante = 0;
				carroGiro = 1;
			}
		}

		if (carroAdelante == 0 && carroGiro == 1) {
			if (movAutoSonic_z <= 250.0f) {
				movAutoSonic_z += 4.0;
			}
			else {
				orientacionCarro += 90;
				carroAdelante = 2;
				carroGiro = 0;
			}
		}

		if (carroAdelante == 2 && carroGiro == 0) {
			if (movAutoSonic_x <= 230.0f) {
				movAutoSonic_x += 4.0;
			}
			else {
				carroAdelante = 0;
				carroGiro = 2;
			}
		}

		if (carroAdelante == 0 && carroGiro == 2) {
			derrape = true;
			derrapeSonido += 1;
			if (derrapeSonido == 1) {
				SoundEngine->play2D("derrape.mp3", false);
			}
			if (movAutoSonic_z >= 120.0f && movAutoSonic_x <= 370.0f) {
				movAutoSonic_z -= 4.0f;
				movAutoSonic_x += 4.0;
			}
			else {
				orientacionCarro = 180.0f;
				carroAdelante = 2;
				carroGiro = 0;
				ruedasAuto = false;
				derrape = false;
			}
		}
	}

	//Moto animacion
	if (ruedas) {
		girarLlantaMoto += 20;
	}
	
	if (animacion3)
	{
		if (motoAdelante == 1 && motoGiro == 0) {
			caballito = 15.0f;
			if (movMoto_x >= -305.0f)
				caballito = 0.0f;
			if (movMoto_x <= -285.0f) {
				movMoto_x += 4.0f;
			}
			else {
				orientacionMoto += 90.0f;
				motoAdelante = 0;
				motoGiro = 1;
			}
		}

		if (motoAdelante == 0 && motoGiro == 1) {
			if (movMoto_z >= -250.0f) {
				movMoto_z -= 4.0;
			}
			else {
				orientacionMoto -= 90;
				motoAdelante = 2;
				motoGiro = 0;
			}
		}

		if (motoAdelante == 2 && motoGiro == 0) {
			if (movMoto_x <= 0) {
				movMoto_x += 4.0;
			}
			else {
				orientacionMoto -= 90;
				motoAdelante = 0;
				motoGiro = 2;
			}
		}

		if (motoAdelante == 0 && motoGiro == 2) {
			if (movMoto_z <= -40.0f) {
				movMoto_z += 4.0f;
			}
			else {
				orientacionMoto += 90.0f;
				caballito = 15.0f;
				motoAdelante = 2;
				motoGiro = 0;
				girarLlantaMoto = false;
			}
		}
	}
	//Botnik
	if (animacionBotnik)
	{
		botnikRotate -= 3.0;
		if (arribaAlasBotnik == 1) {
			if (botnikAlas >= -45)
				botnikAlas -= 7.0;
			else
				arribaAlasBotnik = 0;
		}
		
		if (arribaAlasBotnik == 0) {
			if (botnikAlas <= 45)
				botnikAlas += 7.0;
			else
				arribaAlasBotnik = 1;
		}
	}

	//Nudillos
	if (animacionNudillos)
	{
		if (orientacionAlcantarilla <= 90.0f)
			orientacionAlcantarilla += 1.0;
		else {
			if (MovY_Nudillos <= 1.5)
				MovY_Nudillos += 0.25;
			else
				orientacionNudillos += 2.0;
		}
	}

	if (volando == 1) {
		if (adelanteEggman == 1) {
			orientaEggman = 0.0f;
			if (movEggman_x <= 60.0f)
				movEggman_x += 5.5;
			else
				adelanteEggman = 0;
		}
		if (adelanteEggman == 0) {
			orientaEggman = 180.0;
			if (movEggman_x >= -60.0f)
				movEggman_x -= 5.5;
			else
				adelanteEggman = 1;
		}
	}

	//Egman
	if (animEggman)
	{
		rotEggman -= 6.1f;
		i += 0.12f;
		movEggman_x = 0 + i * v * cos(ang * n / 180);
		movEggman_y = 0 + (i * v * sin(ang * n / 180) - (g * i * i) / 2);
		if (movEggman_y <= 0.0f)
		{
			i = 0.0f;
			animEggman = false;
		}
	}

	//Helicoptero
	rotHAspas += 20.0f;
	if (animacion2)
	{
		if (heliAdelante == 1 && heliIzquierda == 0) {
			if (mov_Helip_z >= 150) {
				mov_Helip_z -= 3.0;
			}
			else {
				orientacionHelicoptero += 90;
				heliAdelante = 0;
				heliIzquierda = 1;
			}
		}
		
		if(heliAdelante == 0 && heliIzquierda == 1){
			if (mov_Helip_x >= 200) {
				mov_Helip_x -= 3.0;
			}
			else {
				orientacionHelicoptero -= 90;
				heliAdelante = 2;
				heliIzquierda = 0;
			}
		}
		if (heliAdelante == 2 && heliIzquierda == 0) {
			if (mov_Helip_z >= -300) {
				mov_Helip_z -= 3.0;
			}
			else {
				orientacionHelicoptero += 90;
				heliAdelante = 0;
				heliIzquierda = 2;
			}
		}

		if (heliAdelante == 0 && heliIzquierda == 2) {
			if (mov_Helip_x >= -200) {
				mov_Helip_x -= 3.0;
			}
			else {
				orientacionHelicoptero -= 90;
				banderaHelicoptero = 0.0f;
				heliAdelante = 2;
				heliIzquierda = 0;
			}
		}
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 13.0f,  13.0f, 0.0f,   1.0f, 1.0f, // top right
		 13.0f, -13.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-13.0f, -13.0f, 0.0f,   0.0f, 0.0f, // bottom left
		-13.0f,  13.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,	//V3
		-0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V0

		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Final Teoria", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);

	// configure music
	SoundEngine = irrklang::createIrrKlangDevice();
	SoundEngine->play2D("Studiopolis.mp3", true);

	

	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 
	
	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------

	Model piso("resources/objects/piso/piso.obj");
	Model escenario1("resources/objects/SonicObjects/obj1.obj");
	Model escenario5("resources/objects/SonicObjects/obj5.obj");
	Model palacio("resources/objects/BellasArtes/BellasArtes.obj");

	Model botnik("resources/objects/SonicObjects/botnikCuerpo.obj");
	Model botnikDer("resources/objects/SonicObjects/botnikAlaDer.obj");
	Model botnikIzq("resources/objects/SonicObjects/botnikAlaIzq.obj");
	Model gato("resources/objects/gato/12221_Cat_v1_l3.obj");
	Model perro("resources/objects/Perro/perro.obj");
	Model amy("resources/objects/SonicObjects/amy.obj");
	Model bat("resources/objects/SonicObjects/bat.obj");
	Model chaos("resources/objects/SonicObjects/chaos.obj");

	Model sonicPanza ("resources/objects/Sonic/SonicPanza.obj");
	Model sonicCabeza("resources/objects/Sonic/SonicCabeza.obj");
	Model sonicBrazoIzq("resources/objects/Sonic/SonicBrazoIzq.obj");
	Model sonicBrazoDer("resources/objects/Sonic/SonicBrazoDer.obj");
	Model sonicPiernaIzq("resources/objects/Sonic/SonicPiernaIzq.obj");
	Model sonicPiernaDer("resources/objects/Sonic/SonicPiernaDer.obj");

	Model hCuerpo("resources/objects/Helicoptero/HelicopteroCuerpo.obj");
	Model hAspas("resources/objects/Helicoptero/HelicopteroAspasTraseras.obj");
	Model hAspas2("resources/objects/Helicoptero/HelicopteroAspas.obj");

	Model carroceriaSonic("resources/objects/CarroSonic/carroceriaSonic.obj");
	Model llantaSonic("resources/objects/CarroSonic/llantaSonic.obj");

	Model motonudillos("resources/objects/CarroSonic/motoCarroceria.obj");
	Model motollanta("resources/objects/CarroSonic/motoLlanta.obj");
	Model motollanta2("resources/objects/CarroSonic/motonudillos2.obj");

	Model edificio1("resources/objects/Edificio1/Edificio1.obj");
	Model edificio3("resources/objects/Edificio1/edificio3prueba.obj");
	
	Model fuente("resources/objects/fuente alameda/fuente.obj");
	Model cono("resources/objects/StreetObjects/cono.obj");
	Model estacion("resources/objects/StreetObjects/estacion.obj");
	Model banca("resources/objects/StreetObjects/banca.obj");
	Model letrero("resources/objects/StreetObjects/letrero.obj");
	Model basura("resources/objects/StreetObjects/basura.obj");
	Model metrosalida("resources/objects/StreetObjects/metrosalida.obj");
	Model alcantarilla("resources/objects/StreetObjects/alcantarilla.obj");
	Model hidrante("resources/objects/StreetObjects/hidrante.obj");
	Model lampara("resources/objects/StreetObjects/lampara.obj");

	Model flores("resources/objects/StreetObjects/flor1.obj");
	Model jardinera1("resources/objects/StreetObjects/jardinera1.obj");
	Model jardinera2("resources/objects/StreetObjects/jardinera2.obj");
	Model florecillas("resources/objects/StreetObjects/florecillas.obj");
	Model floresotas("resources/objects/StreetObjects/floresotas.obj");
	Model arbol("resources/objects/StreetObjects/arbol.obj");

	Model eggman("resources/objects/SonicObjects/eggman.obj");

	Model knuckles("resources/objects/knuckles/knuckles.obj");

	Model supersonic("resources/objects/SuperSonic/supersonic.obj");
	Model esmeralda("resources/objects/SuperSonic/esmeralda.obj");


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", glm::vec3(0.0f, 15.0f, 400.0f));
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(light0, light1, light2));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.00009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(0.0f, 65.0f, 1.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, light0_2, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 0.008f);
		staticShader.setFloat("pointLight[1].linear", 0.00009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.00032f);

		staticShader.setVec3("pointLight[2].position", glm::vec3(camera.Position.x, camera.Position.y - 10.0f, camera.Position.z - 50.0f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, light0_3));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 0.08f);
		staticShader.setFloat("pointLight[2].linear", 0.0009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.00032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(movEggman_x + 300.0f, movEggman_y + 20.0f, movEggman_z - 400.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(light0_4));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 0.08f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setVec3("spotLight[1].position", glm::vec3(388.0f, 30.0f, 65.0f));
		staticShader.setVec3("spotLight[1].direction", glm::vec3(0.0f, -1.0f, -1.0f));
		staticShader.setVec3("spotLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[1].diffuse", glm::vec3(light0_5, 0.0f, light0_5));
		staticShader.setVec3("spotLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[1].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[1].constant", 0.0008f);
		staticShader.setFloat("spotLight[1].linear", 0.0009f);
		staticShader.setFloat("spotLight[1].quadratic", 0.0005f);

		staticShader.setVec3("spotLight[2].position", glm::vec3(-280.0f, 2.5f, 400.0f));
		staticShader.setVec3("spotLight[2].direction", glm::vec3(-1.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[2].diffuse", glm::vec3(light0_6, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[2].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[2].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[2].constant", 0.08f);
		staticShader.setFloat("spotLight[2].linear", 0.0009f);
		staticShader.setFloat("spotLight[2].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar código aquí
		//modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		//modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		//modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//myShader.setMat4("model", modelOp);
		//myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		//glBindTexture(GL_TEXTURE_2D, t_piso);
		////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(38.0f, 2.0f, 4.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 0.1f, -65.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_pista);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(38.0f, 2.0f, 4.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 0.1f, 65.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_pista);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, 16.1f));
		modelOp = glm::translate(modelOp, glm::vec3(-70.0f, 0.1f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_pista);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO[0]);
		//Colocar código aquí
		//modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		//modelOp = glm::scale(modelOp, glm::vec3(5.0f, 5.0f, 1.0f));
		//myShader.setMat4("model", modelOp);
		//myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		//glBindTexture(GL_TEXTURE_2D, t_unam);
		////glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		/***   Cubo  **/

		glBindVertexArray(VAO[1]);
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-425.0f, 30.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(60.0f, 60.0f, 60.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_edificio);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-425.0f, 62.5f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(80.0f, 5.0f, 80.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_metal);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-425.0f, 95.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(60.0f, 60.0f, 60.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_edificio);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-425.0f, 122.7f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(80.0f, 5.0f, 80.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_metal);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-394.5f, 10.0f, 15.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 20.0f, 14.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_puertaOficina);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 0.0f, -150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		modelOp = glm::rotate(modelOp, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		escenario1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, 33.5f, -146.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.7f));
		staticShader.setMat4("model", modelOp);
		arbol.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, 0.0f, -150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		modelOp = glm::rotate(modelOp, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		escenario1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-112.75f, 33.5f, -144.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.7f));
		staticShader.setMat4("model", modelOp);
		arbol.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 0.0f, 150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		modelOp = glm::rotate(modelOp, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		escenario1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(110.0f, 33.5f, 146.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.7f));
		staticShader.setMat4("model", modelOp);
		arbol.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, 0.0f, 150.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		escenario1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-112.75f, 33.5f, 146.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.7f));
		staticShader.setMat4("model", modelOp);
		arbol.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, 75.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f, 0.2f, 0.2f));
		staticShader.setMat4("model", modelOp);
		escenario5.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, 20.0f, -75.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.8f, 1.5f, 1.0f));
		staticShader.setMat4("model", modelOp);
		flores.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 20.0f, 78.5f));
		modelOp = glm::scale(modelOp, glm::vec3(1.8f, 1.5f, 1.0f));
		staticShader.setMat4("model", modelOp);
		flores.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, 0.0f, 75.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f, 0.2f, 0.2f));
		staticShader.setMat4("model", modelOp);
		escenario5.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, 20.0f, 75.0f));
		staticShader.setMat4("model", modelOp);
		jardinera2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 0.0f, -75.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f, 0.2f, 0.2f));
		staticShader.setMat4("model", modelOp);
		escenario5.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 20.0f, -75.0f));
		staticShader.setMat4("model", modelOp);
		jardinera2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-80.0f, 0.0f, -75.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f, 0.2f, 0.2f));
		staticShader.setMat4("model", modelOp);
		escenario5.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(390.0f, 0.0f, -53.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.35f));
		staticShader.setMat4("model", modelOp);
		palacio.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-400.0f, 0.0f, 400.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.8f));
		staticShader.setMat4("model", modelOp);
		edificio1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -400.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.9f));
		staticShader.setMat4("model", modelOp);
		edificio3.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(480.0f, 0.0f, 290.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.5f));
		staticShader.setMat4("model", modelOp);
		cono.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(480.0f, 0.0f, 258.5f));
		modelOp = glm::scale(modelOp, glm::vec3(2.5f));
		staticShader.setMat4("model", modelOp);
		cono.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(480.0f, 0.0f, 225.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.5f));
		staticShader.setMat4("model", modelOp);
		cono.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		fuente.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(450.0f, 0.0f, -350.0f));
		staticShader.setMat4("model", modelOp);
		estacion.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-450.0f, 0.0f, -350.0f));
		staticShader.setMat4("model", modelOp);
		estacion.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(450.0f, 0.0f, 400.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		estacion.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-250.0f, 0.0f, -350.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		banca.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -350.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		banca.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 0.0f, 370.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		banca.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 370.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		banca.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-200.0f, 7.0f, -350.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		letrero.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-225.0f, 5.0f, 400.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		letrero.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(328.0f, 5.0f, 130.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		letrero.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(435.0f, 0.0f, 115.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		basura.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-435.0f, 0.0f, -435.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		basura.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(435.0f, 0.0f, 435.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		basura.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, -10.5f, -115.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		metrosalida.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, -10.5f, 115.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		metrosalida.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(450.0f, 0.5f, -258.5f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		alcantarilla.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(420.0f, 0.5f, 258.5f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		staticShader.setMat4("model", modelOp);
		alcantarilla.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-293.0f, 0.5f, -115.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.0f));
		modelOp = glm::rotate(modelOp, glm::radians(orientacionAlcantarilla), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", modelOp);
		alcantarilla.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-284.0f, MovY_Nudillos, -115.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(orientacionNudillos), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		knuckles.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, -350.0f));
		staticShader.setMat4("model", modelOp);
		hidrante.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, -425.0f));
		staticShader.setMat4("model", modelOp);
		hidrante.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, 425.0f));
		staticShader.setMat4("model", modelOp);
		hidrante.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-160.0f, 0.0f, 425.0f));
		staticShader.setMat4("model", modelOp);
		hidrante.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, -105.0f));
		staticShader.setMat4("model", modelOp);
		lampara.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-400.0f, 0.0f, -425.0f));
		staticShader.setMat4("model", modelOp);
		lampara.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, 0.0f, -425.0f));
		staticShader.setMat4("model", modelOp);
		lampara.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(170.0f, 0.0f, 105.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.75f));
		staticShader.setMat4("model", modelOp);
		perro.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 0.0f, 105.0f));
		staticShader.setMat4("model", modelOp);
		amy.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 0.0f, 450.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", modelOp);
		gato.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(170.0f, 0.0f, -450.0f));
		staticShader.setMat4("model", modelOp);
		bat.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		piso.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 400.0f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.5f));
		staticShader.setMat4("model", modelOp);
		supersonic.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 50.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(2.25f));
		staticShader.setMat4("model", modelOp);
		esmeralda.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-280.0f, 0.0f, 400.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		chaos.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(130.0f, 0.0f, 200.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, 200.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(130.0f, 0.0f, -200.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-130.0f, 0.0f, -200.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 0.0f, 90.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 0.0f, -95.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-215.0f, 0.0f, 90.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-215.0f, 0.0f, -95.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(1.4f));
		staticShader.setMat4("model", modelOp);
		floresotas.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Botnik
		// -------------------------------------------------------------------------------------------------------------------------
		glm::mat4 cuerpoBotnik = glm::mat4(1.0f);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 85.0f, -90.0f));
		cuerpoBotnik = modelOp = glm::rotate(modelOp, glm::radians(botnikRotate), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		botnik.Draw(staticShader);

		modelOp = glm::translate(cuerpoBotnik, glm::vec3(-3.117f, 13.51f, -58.655));
		modelOp = glm::rotate(modelOp, glm::radians(botnikAlas), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		botnikDer.Draw(staticShader);

		modelOp = glm::translate(cuerpoBotnik, glm::vec3(-2.96f, 13.254f, -68.811));
		modelOp = glm::rotate(modelOp, glm::radians(-botnikAlas), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		botnikIzq.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		// Carro Sonic
		// -------------------------------------------------------------------------------------------------------------------------

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movAutoSonic_x, 0.0f, movAutoSonic_z));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(orientacionCarro), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		carroceriaSonic.Draw(staticShader);

		modelOp = glm::translate(tmp, glm::vec3(-8.883f, 4.1f, -9.188f));
		modelOp = glm::rotate(modelOp, glm::radians(girarLlantaCarro), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		llantaSonic.Draw(staticShader);

		modelOp = glm::translate(tmp, glm::vec3(-8.883f, 4.1f, 11.061f));
		modelOp = glm::rotate(modelOp, glm::radians(girarLlantaCarro), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		llantaSonic.Draw(staticShader);

		modelOp = glm::translate(tmp, glm::vec3(9.957f, 4.1f, -9.188f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-girarLlantaCarro), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		llantaSonic.Draw(staticShader);

		modelOp = glm::translate(tmp, glm::vec3(9.957f, 4.1f, 11.061f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-girarLlantaCarro), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		llantaSonic.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Moto Knuckles
		// -------------------------------------------------------------------------------------------------------------------------
		glm::mat4 cuerpoMoto = glm::mat4(1.0f);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movMoto_x, 1.5f, movMoto_z));
		modelOp = glm::rotate(modelOp, glm::radians(orientacionMoto), glm::vec3(0.0f, 1.0f, 0.0f));
		cuerpoMoto = modelOp = glm::rotate(modelOp, glm::radians(-caballito), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		motonudillos.Draw(staticShader);

		modelOp = glm::translate(cuerpoMoto, glm::vec3(-0.098f, 3.902f, 18.228f));
		modelOp = glm::rotate(modelOp, glm::radians(girarLlantaMoto), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		motollanta.Draw(staticShader);

		modelOp = glm::translate(cuerpoMoto, glm::vec3(-0.098f, 3.441f, -6.549f));
		modelOp = glm::rotate(modelOp, glm::radians(girarLlantaMoto), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		motollanta2.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Sonic
		// -------------------------------------------------------------------------------------------------------------------------
		glm::mat4 cuerpoSonic = glm::mat4(1.0f);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(camera.Position.x, camera.Position.y-10.0f, camera.Position.z-50.0f));
		cuerpoSonic = modelOp = glm::rotate(modelOp, glm::radians(orientacionSonic), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		sonicPanza.Draw(staticShader);
		
		modelOp = glm::translate(cuerpoSonic, glm::vec3(0.0f, 0.983f, 0.0f));
		staticShader.setMat4("model", modelOp);
		sonicCabeza.Draw(staticShader);
		
		modelOp = glm::translate(cuerpoSonic, glm::vec3(-1.455f, 0.55f, 0.115f));
		modelOp = glm::rotate(modelOp, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelOp = glm::rotate(modelOp, glm::radians(rotacionSonic), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		sonicBrazoIzq.Draw(staticShader);

		modelOp = glm::translate(cuerpoSonic, glm::vec3(1.38f, 0.45f, -0.198f));
		modelOp = glm::rotate(modelOp, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelOp = glm::rotate(modelOp, glm::radians(rotacionSonic), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		sonicBrazoDer.Draw(staticShader);
		
		modelOp = glm::translate(cuerpoSonic, glm::vec3(-1.063f, -1.563f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(rotacionSonic), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		sonicPiernaIzq.Draw(staticShader);
		
		modelOp = glm::translate(cuerpoSonic, glm::vec3(0.88f, -1.444f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-rotacionSonic), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		sonicPiernaDer.Draw(staticShader);
		
		// -------------------------------------------------------------------------------------------------------------------------
		// Helicoptero
		// -------------------------------------------------------------------------------------------------------------------------
		glm::mat4 cuerpoHelicoptero = glm::mat4(1.0f);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(mov_Helip_x, 125.0f, mov_Helip_z));
		modelOp = glm::rotate(modelOp, glm::radians(orientacionHelicoptero), glm::vec3(0.0f, 1.0f, 0.0f));
		cuerpoHelicoptero = modelOp = glm::rotate(modelOp, glm::radians(banderaHelicoptero), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", modelOp);
		hCuerpo.Draw(staticShader);
		
		modelOp = glm::translate(cuerpoHelicoptero, glm::vec3(40.816f, 17.721f, 0.207f));
		modelOp = glm::rotate(modelOp, glm::radians(rotHAspas), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", modelOp);
		hAspas.Draw(staticShader);

		modelOp = glm::translate(cuerpoHelicoptero, glm::vec3(3.769f, 18.933f, 0.171f));
		modelOp = glm::rotate(modelOp, glm::radians(rotHAspas), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		hAspas2.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Eggman
		// -------------------------------------------------------------------------------------------------------------------------

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movEggman_x + 300.0f, movEggman_y+50.0f, movEggman_z-400.0f));
		modelOp = glm::rotate(modelOp, glm::radians(orientaEggman), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(rotEggman), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", modelOp);
		eggman.Draw(staticShader);

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
		if (adelanteSonic == 1) {
			if (rotacionSonic >= -45)
				rotacionSonic -= 14.0;
			else
				adelanteSonic = 0;
		}

		if (adelanteSonic == 0) {
			if (rotacionSonic <= 45)
				rotacionSonic += 14.0;
			else
				adelanteSonic = 1;
		}
		SoundEngine->play2D("pruebauno.mp3", false);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
		if (adelanteSonic == 1) {
			if (rotacionSonic >= -45)
				rotacionSonic -= 14.0;
			else
				adelanteSonic = 0;
		}

		if (adelanteSonic == 0) {
			if (rotacionSonic <= 45)
				rotacionSonic += 14.0;
			else
				adelanteSonic = 1;
		}
		SoundEngine->play2D("pruebauno.mp3", false);
	}
		
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
		if (adelanteSonic == 1) {
			if (rotacionSonic >= -45)
				rotacionSonic -= 14.0;
			else
				adelanteSonic = 0;
		}

		if (adelanteSonic == 0) {
			if (rotacionSonic <= 45)
				rotacionSonic += 14.0;
			else
				adelanteSonic = 1;
		}
		SoundEngine->play2D("pruebauno.mp3", false);
	}
		
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
		if (adelanteSonic == 1) {
			if (rotacionSonic >= -45)
				rotacionSonic -= 14.0;
			else
				adelanteSonic = 0;
		}

		if (adelanteSonic == 0) {
			if (rotacionSonic <= 45)
				rotacionSonic += 14.0;
			else
				adelanteSonic = 1;
		}
		SoundEngine->play2D("pruebauno.mp3", false);
	}
		
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		orientacionSonic -= 2.5;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		orientacionSonic += 2.5;

	//Teclas Helicptero
	//reseteo
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		banderaHelicoptero = 0.0f;
		orientacionHelicoptero = -90.0f;
		mov_Helip_x = 400.0f;
		mov_Helip_z = 400.0f;
		heliAdelante = 1;
		heliIzquierda = 0;
		animacion2 = false;
	}
	//animacion 
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		animacion2 ^= true;
		if(banderaHelicoptero == 0.0f)
			banderaHelicoptero = 15.0f;
		else
			banderaHelicoptero = 0.0f;
	}

	//Carro
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		animacion ^= true;
		ruedasAuto ^= true;
	}
		

	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		movAutoSonic_x = -450.0f;
			movAutoSonic_z = -250.0f;
			girarLlantaCarro = 0.0f;
		orientacionCarro = 90.0f;
		ruedasAuto = false;
		carroAdelante = 1;
			carroGiro = 0;
			animacion = false;
			derrape = false;
			derrapeSonido = 0;
	}


	//Moto
	//Animacion
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		animacion3 ^= true;
		ruedas ^= true;
	}
	//reseteo
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		movMoto_x = -450.0f;
			movMoto_z = 250.0f;
			girarLlantaMoto = 0.0f;
			orientacionMoto = 90.0f;
		caballito = 0.0f;
		motoAdelante = 1;
		motoGiro = 0;
		animacion3 = false;
		ruedas = false;
	}

	//Botnik animacion
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		animacionBotnik ^= true;
	}
		
	//Animación nudillos
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		animacionNudillos ^= true;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		orientacionNudillos = 0.0f,
		orientacionAlcantarilla = 0.0f,
		MovY_Nudillos = -18.0f;
		animacionNudillos = false;
	}
	// Animación Eggman
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		i = 0.0;
		rotEggman = 0.0f;
		movEggman_x = 0.0f;
		movEggman_y = 0.0f;
		animEggman = false;
		volando = 1;
		orientaEggman = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		volando = 0;
		animEggman ^= true;
	}


	//Luces
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		luzencedida1 ^= true;
		if (luzencedida1) {
			light0 = 1.0f;
			light1 = 1.0f;
			light2 = 0.0f;
		}
		else {
			light0 = 0.0f;
			light1 = 0.0f;
			light2 = 0.0f;
		}

	}
	
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		luzencedida2 ^= true;
		if (luzencedida2) {
			light0_2 = 1.0f;
		}
		else {
			light0_2 = 0.0f;
		}

	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		luzencedida3 ^= true;
		if (luzencedida3) {
			light0_3 = 1.0f;
		}
		else {
			light0_3 = 0.0f;
		}

	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		luzencedida4 ^= true;
		if (luzencedida4) {
			light0_4 = 1.0f;
		}
		else {
			light0_4 = 0.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		luzencedida5 ^= true;
		if (luzencedida5) {
			light0_5 = 1.0f;
		}
		else {
			light0_5 = 0.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		luzencedida6 ^= true;
		if (luzencedida6) {
			light0_6 = 1.0f;
		}
		else {
			light0_6 = 0.0f;
		}
	}
	// Cambiar la musica
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		switchMusica ^= true;
		if (switchMusica)
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("Find your flame.mp3", true);
		}
		else
		{
			SoundEngine->stopAllSounds();
			SoundEngine->play2D("Studiopolis.mp3", true);
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
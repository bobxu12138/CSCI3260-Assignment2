/*
Student Information
Student ID:1155173847
Student Name:Xu Chenyan
*/

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>


// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
float lastX=400;
float lastY=300;
bool firstMouse = true;
float pitch =0.0f;
float yaw =-90.0f;
GLuint theme_dog =0;
GLuint theme_ground =2;
GLuint brightness=5;
bool press=false;
bool Random =false;
int start_time=-1;
float x_delta = 0.1f;
int x_press_num = 0;
float z_delta =0.1f;
int z_press_num = 0;
int grass_press_number=1;
int rotate_number=0;
float cameraSpeed =0.3;
int s_press_num=0;
float cx =0.0f;
float cy =7.0f;
float cz =15.0f;
int y_press_num=0;
float y_delta=0.1f;
Shader shader0;
Texture texture[4];
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp ;

// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model loadOBJ(const char* objPath)
{
    // function to load the obj file
    // Note: this simple function cannot load all obj files.

    struct V {
        // struct for identify if a vertex has showed up
        unsigned int index_position, index_uv, index_normal;
        bool operator == (const V& v) const {
            return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
        }
        bool operator < (const V& v) const {
            return (index_position < v.index_position) ||
                (index_position == v.index_position && index_uv < v.index_uv) ||
                (index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
        }
    };
	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

GLuint VAO1,VBO1,EBO1;
Model obj1,obj2;
GLuint VAO2,VBO2,EBO2;

void sendDataToOpenGL()
{
	//TODO
	//Load objects and bind to VAO and VBO
	//Load textures
    obj1=loadOBJ("dog.obj");
    texture[0].setupTexture("dog_01.jpg");
    texture[1].setupTexture("dog_02.jpg");
    glGenVertexArrays(1,&VAO1);
    glBindVertexArray(VAO1);
    glGenBuffers(1,&VBO1);
    glBindBuffer(GL_ARRAY_BUFFER,VBO1);
    glBufferData(GL_ARRAY_BUFFER,obj1.vertices.size()*sizeof(Vertex),&obj1.vertices[0],GL_STATIC_DRAW);
    glGenBuffers(1,&EBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,obj1.indices.size()*sizeof(unsigned int),&obj1.indices[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    obj2=loadOBJ("ground.obj");
    texture[2].setupTexture("ground_01.jpg");
    texture[3].setupTexture("ground_02.jpg");
    glGenVertexArrays(1,&VAO2);
    glBindVertexArray(VAO2);
    glGenBuffers(1,&VBO2);
    glBindBuffer(GL_ARRAY_BUFFER,VBO2);
    glBufferData(GL_ARRAY_BUFFER,obj2.vertices.size()*sizeof(Vertex),&obj2.vertices[0],GL_STATIC_DRAW);
    glGenBuffers(1,&EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,obj2.indices.size()*sizeof(unsigned int),&obj2.indices[0],GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
}


void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	//TODO: set up the camera parameters	
	//TODO: set up the vertex shader and fragment shader
    shader0.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
    shader0.use();
   
    cameraPos = glm::vec3(cx,cy,cz);
    cameraFront = glm::vec3(-cx,-cy,-cz);
    cameraUp = glm::vec3(0.0f,1.0f,+0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}


void cooridinatetransform (int index){
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix =glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    if (index==0){
        
        if (Random) {
           int a=(rand()%(10-1+1)-4.5)*0.3;
           int b=(rand()%(10-1+1)-4.5)*0.3;
            if(x_press_num+a<=40&&x_press_num+b>=-40)
                x_press_num+=a;
            if(z_press_num+b<=40&&z_press_num+b>=-40)
                z_press_num+=b;
        }
        modelMatrix = glm::translate(modelMatrix,glm::vec3(x_delta * x_press_num, y_delta*y_press_num, z_delta * z_press_num));
        modelMatrix= glm::translate(modelMatrix,glm::vec3(0.0f,-1.8f,0.0f));
        modelMatrix= glm::rotate(modelMatrix,glm::radians(0.0f+rotate_number*10),glm::vec3(0.0f,1.0f,0.0f));
       modelMatrix= glm::rotate(modelMatrix,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
        modelMatrix = glm::scale(modelMatrix,glm::vec3(0.025+0.05*s_press_num,0.025+0.05*s_press_num,0.05+0.05*s_press_num));
    }
    if (index==1){
        modelMatrix= glm::translate(modelMatrix,glm::vec3(0.0f,-0.9f,0.0f));
        
         }
    viewMatrix = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
    projectionMatrix=glm::perspective(glm::radians(45.0f),1.0f,1.0f,100.0f);
    shader0.setMat4("modelMatrix",modelMatrix);
    shader0.setMat4("viewMatrix",viewMatrix);
    shader0.setMat4("projectionMatrix",projectionMatrix);
    shader0.setVec3("ambientLight",glm::vec3(0.1f,0.1f,0.1f));
    shader0.setVec3("lightPositionWorld",glm::vec3(0.0f,0.8f,0.8f));
    shader0.setVec3("eyePosition",glm::vec3(cx,cy,cz));
    shader0.setVec3("lightDirectionWorld",glm::vec3(-1.0f,-1.0f,-1.0f));
    shader0.setVec3("ambientLight1", glm::vec3(0.1f+0.1f*brightness,0.1f+0.1f*brightness,0.1f+0.1f*brightness));

};


void paintGL(void)  //always run
{
    glClearColor(0.54f, 0.78f, 0.97f, 0.5f); //specify the background color, this is just an example
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //TODO:
    //Set lighting information, such as position and color of lighting source
    //Set transformation matrix
    //Bind different texture
    glBindVertexArray(VAO1);
    cooridinatetransform(0);
    GLuint slot=0;
    texture[theme_dog].bind(slot);
    shader0.setInt("myTextureSampler0", slot);
    glDrawElements(GL_TRIANGLES, obj1.indices.size(), GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(VAO2);
    cooridinatetransform(1);
    slot=0;
    texture[theme_ground].bind(slot);
    shader0.setInt("myTextureSampler0", slot);
    glDrawElements(GL_TRIANGLES, obj2.indices.size(), GL_UNSIGNED_INT, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
{
    press = true;
}
if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
{
    press = false;
}
	// Sets the mouse-button callback for the current window.
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{   if(press==true){
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}
    // Sets the cursor position callback for the current window
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{   if(xoffset>0)
    cameraPos += cameraSpeed * cameraFront;
    if(yoffset>0)
    cameraPos -= cameraSpeed * cameraFront;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{ if (glfwGetKey(window, GLFW_KEY_Y)== GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
 if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
 if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
 if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        theme_dog = 0;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        theme_dog = 1;
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        theme_ground = 2;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        theme_ground = 3;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        brightness++;
        if (brightness >  10) brightness =  10;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        brightness--;
        if (brightness <1) brightness= 1;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS&&y_press_num>=0) {
        y_press_num -= 1;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS&&y_press_num<=25) {
        y_press_num += 1;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        rotate_number += 1;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        rotate_number -= 1;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        if (Random ==true)
            Random =false ;
        else Random = true;
    }
    

	// Sets the Keyboard callback for the current window.
}


int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	initializedGL();

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}







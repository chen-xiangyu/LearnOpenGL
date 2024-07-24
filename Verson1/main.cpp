#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void adjustWindowSize(GLFWwindow* vWindow, int vWidth, int vHeight);
void processInput(GLFWwindow* vWindow);

const unsigned int WindowWidth = 800;
const unsigned int WindowHeight = 600;
const unsigned int InfoLength = 512;
const glm::vec4 ClearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

const char* VertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 vPos;\n"
	"layout (location = 1) in vec3 vColor;\n"
	"layout (location = 2) in vec3 vNormal;\n"
	"out vec3 Color;\n"
	"out vec3 Normal;\n"
	"out vec3 FragPos;\n"
	"uniform mat4 Model;\n"
	"uniform mat4 View;\n"
	"uniform mat4 Projection;\n"
    "void main()\n"
    "{\n"
	"   FragPos = vec3(Model * vec4(vPos, 1.0f));\n"
    "   gl_Position = Projection * View * vec4(FragPos, 1.0f);\n"
	"   Color = vColor;\n"
	"   Normal = mat3(transpose(inverse(Model))) * vNormal;\n"
	"}\0";

const char* FragmentShaderSource = "#version 330 core\n"
	"in vec3 Color;\n"
	"in vec3 Normal;\n"
	"in vec3 FragPos;\n"
	"out vec4 FragColor;\n"
	"uniform vec3 Ambient;\n"
	"uniform vec3 Diffuse;\n"
	"uniform vec3 Specular;\n"
	"uniform float Shininess;\n"
	"uniform vec3 ViewPos;\n"
	"uniform vec3 LightColor;\n"
	"void main()\n"
	"{\n"
	"   vec3 LightDir = vec3(0.0f, 0.0f, 1.0f);\n"
	"	vec3 NormalDir = normalize(Normal);\n"
	"   vec3 DiffuseColor = max(dot(LightDir, NormalDir), 0.0f) * LightColor * Diffuse;\n"
	"   vec3 ReflectDir = reflect(-LightDir, NormalDir);\n"
	"   vec3 ViewDir = normalize(ViewPos - FragPos);\n"
	"   vec3 SpecularColor = pow(max(dot(ViewDir, ReflectDir), 0.0f), Shininess) * LightColor * Specular;\n"
	"   vec3 AmbientColor = Ambient * LightColor;\n"
	"   vec3 FinalColor = (DiffuseColor + SpecularColor + AmbientColor) * Color;\n"
	"   FragColor = vec4(FinalColor, 1.0f);\n"
	"}\n\0";

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* Window = glfwCreateWindow(WindowWidth, WindowHeight, "Learn OpenGL", NULL, NULL);
	if (Window == NULL)
	{
		std::cout << "failed to create GLFW window.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, adjustWindowSize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize GLAD.\n";
		return -1;
	}

	unsigned int VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
	glCompileShader(VertexShader);

	int IsSuccess;
	char InfoLog[InfoLength];
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &IsSuccess);
	if (!IsSuccess)
	{
		glGetShaderInfoLog(VertexShader, InfoLength, NULL, InfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << InfoLog << "\n";
	}

	unsigned int FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL);
	glCompileShader(FragmentShader);

	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &IsSuccess);
	if (!IsSuccess)
	{
		glGetShaderInfoLog(FragmentShader, InfoLength, NULL, InfoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << InfoLog << "\n";
	}

	unsigned int ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &IsSuccess);
	if (!IsSuccess)
	{
		glGetProgramInfoLog(ShaderProgram, InfoLength, NULL, InfoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << InfoLog << "\n";
	}
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	float Vertices[] = {
		//   pisition              color            normal
		 0.5f,  0.5f, -3.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -3.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -3.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -3.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
	};
	unsigned int Indices[] = {
		0, 1, 2,  
		0, 3, 2   
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(ShaderProgram);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 View = glm::mat4(1.0f);
	float FOV = 45.0f, ZNear = 0.1f, ZFar = 100.f;
	glm::mat4 Projection = glm::perspective(glm::radians(FOV), (float)WindowWidth / WindowHeight, ZNear, ZFar);
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "Model"), 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "View"), 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "Projection"), 1, GL_FALSE, glm::value_ptr(Projection));

	glm::vec3 Diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 Specular = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 Ambient = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 ViewPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float Shininess = 100.0f;
	glUniform3fv(glGetUniformLocation(ShaderProgram, "Diffuse"), 1, glm::value_ptr(Diffuse));
	glUniform3fv(glGetUniformLocation(ShaderProgram, "Specular"), 1, glm::value_ptr(Specular));
	glUniform3fv(glGetUniformLocation(ShaderProgram, "Ambient"), 1, glm::value_ptr(Ambient));
	glUniform3fv(glGetUniformLocation(ShaderProgram, "ViewPos"), 1, glm::value_ptr(ViewPos));
	glUniform3fv(glGetUniformLocation(ShaderProgram, "LightColor"), 1, glm::value_ptr(LightColor));
	glUniform1f(glGetUniformLocation(ShaderProgram, "Shininess"), Shininess);

	while (!glfwWindowShouldClose(Window))
	{
		processInput(Window);

		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ShaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(Indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(ShaderProgram);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* vWindow)
{
	if (glfwGetKey(vWindow, GLFW_KEY_ESCAPE) == GLFW_TRUE)
	{
		glfwSetWindowShouldClose(vWindow, true);
	}
}

void adjustWindowSize(GLFWwindow* vWindow, int vWidth, int vHeight)
{
	glViewport(0, 0, vWidth, vHeight);
}
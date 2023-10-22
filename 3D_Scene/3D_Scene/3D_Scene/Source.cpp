#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <camera.h> // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "7-1 Project: Creating 3D Scene"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handle for the vertex buffer object
        GLuint nVertices;    // Number of indices of the mesh
    };

    // Main GLFW window3
    GLFWwindow* gWindow = nullptr;

    // Triangle mesh data
    GLMesh gTableMesh;
    GLMesh gScreenMesh;
    GLMesh gScreenBaseMesh;
    GLMesh gStandMesh;
    GLMesh gStandBaseMesh;
    GLMesh gLaptopTopMesh;
    GLMesh gLaptopBottomMesh;
    GLMesh gPadMesh;
    GLMesh gMouseMesh;

    // Texture ids
    GLuint gTableTextureId;
    GLuint gScreenTextureId;
    GLuint gScreenBaseTextureId;
    GLuint gStandTextureId;
    GLuint gStandBaseTextureId;
    GLuint gLaptopTopTextureId;
    GLuint gLaptopBottomTextureId;
    GLuint gPadTextureId;
    GLuint gMouseTextureId;



    glm::vec2 gUVScale(1.0f, 1.0f);
    GLint gTexWrapMode = GL_REPEAT;
   
    // Shader program
    GLuint gCubeProgramId;
    GLuint gLampProgramId;
    
    // camera
    glm::vec3 gCameraPos = glm::vec3(0.0f, 0.0f, 7.0f);
    glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;


    bool gFirstMouse = true;
    float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float fov = 45.0f;
    float sensitivity = 0.1f;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    glm::mat4 viewModel = glm::perspective(glm::radians(fov), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 viewModelOne = glm::perspective(glm::radians(fov), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 viewModelTwo = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

   
    // Subject position and scale
    glm::vec3 gCubePosition(0.0f, 0.0f, 0.0f);
    glm::vec3 gCubeScale(2.0f);

    // Cube and light color
    //m::vec3 gObjectColor(0.6f, 0.5f, 0.75f);
    glm::vec3 gObjectColor(1.0f, 0.2f, 0.0f);
    glm::vec3 gLightColor(1.0f, 1.0f, 1.0f); // Key light color
    glm::vec3 gLightColorTwo(0.0f, 0.0f, 1.0f); // Fill light color

    // Light position and scale
    glm::vec3 gLightPosition(-1.5f, 1.6f, 2.0f); // Key light
    glm::vec3 gLightPositionTwo(1.5f, -0.5f, 2.0f); //  Fill light
    glm::vec3 gLightScale(0.3f);

}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateTableMesh(GLMesh& mesh);
void UCreateScreenMesh(GLMesh& mesh);
void UCreateStandBaseMesh(GLMesh& mesh);
void UCreateStandMesh(GLMesh& mesh);
void UCreateScreenBaseMesh(GLMesh& mesh);
void UCreateLaptopTopMesh(GLMesh& mesh);
void UCreateLaptopBottomMesh(GLMesh& mesh);
void UCreatePadMesh(GLMesh& mesh);
void UCreateMouseMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Cube Vertex Shader Source Code*/
const GLchar* cubeVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
layout(location = 1) in vec3 normal; // VAP position 1 for normals
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal; // For outgoing normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate;

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 modelTwo;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;
}
);

/* Cube Fragment Shader Source Code*/
const GLchar* cubeFragmentShaderSource = GLSL(440,

    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;

uniform vec3 objectColorTwo;
uniform vec3 lightColorTwo;
uniform vec3 lightPosTwo;
uniform vec3 viewPositionTwo;

uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    //Phong lighting model calculations to generate ambient, diffuse, and specular components

    //Calculate Ambient lighting for the key light
    float ambientStrength = 1.0f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Ambient lighting for the fill light
    float ambientStrengthTwo = 0.1f; // Set ambient or global lighting strength
    vec3 ambientTwo = ambientStrengthTwo * lightColorTwo; // Generate ambient light color


    /* Calculations to set the Key light*/

   //Calculate Diffuse lighting
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting
    float specularIntensity = 0.8f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;


    /*Calculations for the fill light*/

    //Calculate Diffuse lighting
    vec3 normTwo = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirectionTwo = normalize(lightPosTwo - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impactTwo = max(dot(normTwo, lightDirectionTwo), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuseTwo = impactTwo * lightColorTwo; // Generate diffuse light color

    //Calculate Specular lighting
    float specularIntensityTwo = 0.8f; // Set specular light strength
    float highlightSizeTwo = 16.0f; // Set specular highlight size
    vec3 viewDirTwo = normalize(viewPositionTwo - vertexFragmentPos); // Calculate view direction
    vec3 reflectDirTwo = reflect(-lightDirectionTwo, normTwo);// Calculate reflection vector
    //Calculate specular component
    float specularComponentTwo = pow(max(dot(viewDirTwo, reflectDirTwo), 0.0), highlightSizeTwo);
    vec3 specularTwo = specularIntensityTwo * specularComponentTwo * lightColorTwo;


    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = ((ambient + diffuse + specular) * textureColor.xyz) + ((ambientTwo + diffuseTwo + specularTwo) * textureColor.xyz);

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU

}
);

/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateTableMesh(gTableMesh); // Calls the function to create the Vertex Buffer Object
    UCreateScreenMesh(gScreenMesh);
    UCreateStandBaseMesh(gStandBaseMesh);
    UCreateScreenBaseMesh(gScreenBaseMesh);
    UCreateStandMesh(gStandMesh);
    UCreateLaptopTopMesh(gLaptopTopMesh);
    UCreateLaptopBottomMesh(gLaptopBottomMesh);
    UCreatePadMesh(gPadMesh);
    UCreateMouseMesh(gMouseMesh);

    // Create the shader programs
    if (!UCreateShaderProgram(cubeVertexShaderSource, cubeFragmentShaderSource, gCubeProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
        return EXIT_FAILURE;

    // Load texture
    const char* tableImage = "resources/textures/Tinted glass.jpg";
    if (!UCreateTexture(tableImage, gTableTextureId))
    {
        cout << "Failed to load texture " << tableImage << endl;
        return EXIT_FAILURE;
    }

    const char* screenImage = "resources/textures/computerScreen.jpg";
    if (!UCreateTexture(screenImage, gScreenTextureId))
    {
        cout << "Failed to load texture " << screenImage << endl;
        return EXIT_FAILURE;
    }

    const char* standImage = "resources/textures/Stand_image.jpg";
    if (!UCreateTexture(standImage, gScreenBaseTextureId))
    {
        cout << "Failed to load texture " << standImage << endl;
        return EXIT_FAILURE;
    }

    const char* LaptopBottomImage = "resources/textures/LapTopKeyBoard.jpg";
    if (!UCreateTexture(LaptopBottomImage, gLaptopBottomTextureId))
    {
        cout << "Failed to load texture " << LaptopBottomImage << endl;
        return EXIT_FAILURE;
    }

    const char* PadImage = "resources/textures/pad3.jpg";
    if (!UCreateTexture(PadImage, gPadTextureId))
    {
        cout << "Failed to load texture " << PadImage << endl;
        return EXIT_FAILURE;
    }

    const char* MouseImage = "resources/textures/Computer mouse.jpg";
    if (!UCreateTexture(MouseImage, gMouseTextureId))
    {
        cout << "Failed to load texture " << MouseImage << endl;
        return EXIT_FAILURE;
    }


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gCubeProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gCubeProgramId, "uTexture"), 0);

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gTableMesh);
    UDestroyMesh(gScreenMesh);
    UDestroyMesh(gScreenBaseMesh);
    UDestroyMesh(gStandMesh);
    UDestroyMesh(gStandBaseMesh);
    UDestroyMesh(gScreenBaseMesh);
    UDestroyMesh(gLaptopTopMesh);
    UDestroyMesh(gLaptopBottomMesh);
    UDestroyMesh(gPadMesh);
    UDestroyMesh(gMouseMesh);

    // Release texture
    UDestroyTexture(gTableTextureId);
    UDestroyTexture(gScreenTextureId);
    UDestroyTexture(gScreenBaseTextureId);
    UDestroyTexture(gStandTextureId);
    UDestroyTexture(gStandBaseTextureId);
    UDestroyTexture(gLaptopBottomTextureId);
    UDestroyTexture(gPadTextureId);
    UDestroyTexture(gMouseTextureId);
    

    // Release shader programs
    UDestroyShaderProgram(gCubeProgramId);
    UDestroyShaderProgram(gLampProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}





// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraOffset = cameraSpeed * gDeltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCameraPos += cameraOffset * gCameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCameraPos -= cameraOffset * gCameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCameraPos -= glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraOffset;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCameraPos += glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraOffset;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCameraPos -= gCameraUp * cameraOffset;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCameraPos += gCameraUp * cameraOffset;


    // This statement handles the toggle back and fourth from 2D to 3D
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)

        if (viewModel == viewModelOne)
            viewModel = viewModelTwo;
        else
            viewModel = viewModelOne;


}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top
    gLastX = xpos;
    gLastY = ypos;


    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;



    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    gCameraFront = glm::normalize(front);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    sensitivity -= (float)yoffset;
    if (sensitivity < 0.01f)
        sensitivity = 0.01f;
    if (sensitivity > 0.3f)
        sensitivity = 0.3f;
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Functioned called to render a frame
void URender()
{

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate the cube VAO (used by cube and lamp)
    glBindVertexArray(gTableMesh.vao);
    glBindVertexArray(gScreenMesh.vao);
    glBindVertexArray(gStandBaseMesh.vao);
    //glBindVertexArray(gStandBaseMesh.vao);

    // CUBE: draw cube
   //----------------
   // Set the shader to be used
    glUseProgram(gCubeProgramId);

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = glm::translate(gCubePosition) * glm::scale(gCubeScale);
    //---------------------------------------------------------------------------------
    // camera/view transformation
    glm::mat4 view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);

    // Creates a perspective projection
    glm::mat4 projection = viewModel;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gCubeProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gCubeProgramId, "view");
    GLint projLoc = glGetUniformLocation(gCubeProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // table
    glBindVertexArray(gTableMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gTableTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gTableMesh.nVertices);// Draws the triangles

    // Screen
    glBindVertexArray(gScreenMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gScreenTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gScreenMesh.nVertices);// Draws the triangles

    // Stand Base
    glBindVertexArray(gStandBaseMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gScreenBaseTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gStandBaseMesh.nVertices);// Draws the triangles

    // Stand 
    glBindVertexArray(gStandMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gScreenBaseTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gStandMesh.nVertices);// Draws the triangles

    // Screen Base
    glBindVertexArray(gScreenBaseMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gScreenBaseTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gScreenBaseMesh.nVertices);// Draws the triangles

    // Laptop Top
    glBindVertexArray(gLaptopTopMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gScreenTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gLaptopTopMesh.nVertices);// Draws the triangles

    // Mouse Pad
    glBindVertexArray(gPadMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gPadTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gPadMesh.nVertices);// Draws the triangles

    // Mouse 
    glBindVertexArray(gMouseMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gMouseTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gMouseMesh.nVertices);// Draws the triangles
   
    // Laptop Bottom
    glBindVertexArray(gLaptopBottomMesh.vao);// Activate the cube VAO (used by cube and lamp)
    glActiveTexture(GL_TEXTURE0);// bind textures on corresponding texture units
    glBindTexture(GL_TEXTURE_2D, gLaptopBottomTextureId);
    glDrawArrays(GL_TRIANGLES, 0, gLaptopBottomMesh.nVertices);// Draws the triangles



    // Reference matrix uniforms from the Cube Shader program for the cub color, light color, light position, and camera position
    GLint objectColorLoc = glGetUniformLocation(gCubeProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gCubeProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gCubeProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gCubeProgramId, "viewPosition");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);


    GLint objectColorLocTwo = glGetUniformLocation(gCubeProgramId, "objectColorTwo");
    GLint lightColorLocTwo = glGetUniformLocation(gCubeProgramId, "lightColorTwo");
    GLint lightPositionLocTwo = glGetUniformLocation(gCubeProgramId, "lightPosTwo");
    GLint viewPositionLocTwo = glGetUniformLocation(gCubeProgramId, "viewPositionTwo");


    glUniform3f(objectColorLocTwo, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLocTwo, gLightColorTwo.r, gLightColorTwo.g, gLightColorTwo.b);
    glUniform3f(lightPositionLocTwo, gLightPositionTwo.x, gLightPositionTwo.y, gLightPositionTwo.z);

    const glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gCubeProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    

    

    // LAMP: draw lamp
    //----------------
    glUseProgram(gLampProgramId);

    //used as a visual que for the Key light source
    model = glm::translate(gLightPosition) * glm::scale(gLightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLampProgramId, "model");
    viewLoc = glGetUniformLocation(gLampProgramId, "view");
    projLoc = glGetUniformLocation(gLampProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, gStandBaseMesh.nVertices);

    //used as a visual que for the fill light source
    model = glm::translate(gLightPositionTwo) * glm::scale(gLightScale);

    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLampProgramId, "model");
    viewLoc = glGetUniformLocation(gLampProgramId, "view");
    projLoc = glGetUniformLocation(gLampProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, gStandBaseMesh.nVertices);

    // Deactivate the Vertex Array Object and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateTableMesh function
void UCreateTableMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
  
            //Positions          //Normals
            // ------------------------------------------------------
        

         //Bottom Face        //Negative Y Normal
        -0.5f, -1.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -1.0f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -1.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -1.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -1.0f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -1.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateScreenMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {

        //Positions          //Normals
          // ------------------------------------------------------
          //Back Face          //Negative Z Normal  Texture Coords.
          0.2f, -0.8f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
          0.6f, -0.8f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
          0.6f, -0.4f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
          0.6f, -0.4f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
          0.2f, -0.4f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
          0.2f, -0.8f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

         //Front Face         //Positive Z Normal
         0.2f, -0.8f,  -0.48f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.6f, -0.8f,  -0.48f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.6f, -0.4f,  -0.48f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.6f, -0.4f,  -0.48f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.2f, -0.4f,  -0.48f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
         0.2f, -0.8f,  -0.48f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        //Left Face          //Negative X Normal
        0.2f, -0.4f, -0.48f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.2f, -0.4f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.2f, -0.8f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.2f, -0.8f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.2f, -0.8f, -0.48f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.2f, -0.4f, -0.48f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

       //Right Face         //Positive X Normal
       0.6f, -0.4f, -0.48f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       0.6f, -0.4f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       0.6f, -0.8f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       0.6f, -0.8f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       0.6f, -0.8f, -0.48f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       0.6f, -0.4f, -0.48f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

       //Bottom Face        //Negative Y Normal
       0.2f, -0.8f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
       0.6f, -0.8f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
       0.6f, -0.8f, -0.48f, 0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       0.6f, -0.8f, -0.48f, 0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       0.2f, -0.8f, -0.48f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
       0.2f, -0.8f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

      //Top Face           //Positive Y Normal
      0.2f, -0.4f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
      0.6f, -0.4f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.6f, -0.4f, -0.48f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      0.6f, -0.4f, -0.48f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      0.2f, -0.4f, -0.48f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
      0.2f, -0.4f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f



    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateStandBaseMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
       0.3f, -1.0f,  -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
       0.5f, -1.0f,  -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
       0.5f, -0.97f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       0.5f, -0.97f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       0.3f, -0.97f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       0.3f, -1.0f,  -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

       //Front Face         //Positive Z Normal
       0.3f, -1.0f,  -0.3f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       0.5f, -1.0f,  -0.3f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       0.5f, -0.97f, -0.3f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.5f, -0.97f, -0.3f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.3f, -0.97f, -0.3f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
       0.3f, -1.0f,  -0.3f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      //Left Face          //Negative X Normal
      0.3f,  -0.97f, -0.3f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      0.3f,  -0.97f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      0.3f,  -1.0f,  -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.3f,  -1.0f,  -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.3f,  -1.0f,  -0.3f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
      0.3f,  -0.97f, -0.3f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

      //Right Face         //Positive X Normal
      0.5f,  -0.97f,  -0.3f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      0.5f,  -0.97f,  -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  -1.0f,   -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  -1.0f,   -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  -1.0f,   -0.3f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
      0.5f,  -0.97f,  -0.3f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

      //Bottom Face        //Negative Y Normal
      0.3f, -1.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
      0.5f, -1.0f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
      0.5f, -1.0f, -0.3f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      0.5f, -1.0f, -0.3f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      0.3f, -1.0f, -0.3f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
      0.3f, -1.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

     //Top Face           //Positive Y Normal
     0.3f,  -0.97f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  -0.97f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  -0.97f, -0.3f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  -0.97f, -0.3f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.3f,  -0.97f, -0.3f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
     0.3f,  -0.97f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateStandMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
        0.38f, -0.97f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.43f, -0.97f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.43f, -0.85f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.43f, -0.85f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.38f, -0.85f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        0.38f, -0.97f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        //Front Face         //Positive Z Normal
        0.38f, -0.97f, -0.48f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        0.43f, -0.97f, -0.48f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        0.43f, -0.85f, -0.48f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        0.43f, -0.85f, -0.48f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        0.38f, -0.85f, -0.48f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        0.38f, -0.97f, -0.48f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        //Left Face          //Negative X Normal
        0.38f, -0.85f, -0.48f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.38f, -0.85f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.38f, -0.97f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.38f, -0.97f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.38f, -0.97f, -0.48f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.38f, -0.85f, -0.48f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Right Face         //Positive X Normal
        0.43f, -0.85f, -0.48f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.43f, -0.85f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.43f, -0.97f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.43f, -0.97f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.43f, -0.97f, -0.48f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.43f, -0.85f, -0.48f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        //Bottom Face        //Negative Y Normal
        0.38f, -0.97f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.43f, -0.97f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.43f, -0.97f, -0.48f, 0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.43f, -0.97f, -0.48f, 0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.38f, -0.97f, -0.48f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        0.38f, -0.97f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        //Top Face           //Positive Y Normal
        0.38f, -0.85f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.43f, -0.85f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.43f, -0.85f, -0.48f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.43f, -0.85f, -0.48f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.38f, -0.85f, -0.48f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        0.38f, -0.85f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateScreenBaseMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
        0.2f, -0.85f,  -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.6f, -0.85f,  -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.6f, -0.80f,  -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.6f, -0.80f,  -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.2f, -0.80f,  -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        0.2f, -0.85f,  -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

       //Front Face         //Positive Z Normal
       0.2f, -0.85f,  -0.45f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       0.6f, -0.85f,  -0.45f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       0.6f, -0.80f,  -0.45f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.6f, -0.80f,  -0.45f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.2f, -0.80f,  -0.45f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
       0.2f, -0.85f,  -0.45f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      //Left Face          //Negative X Normal
     0.2f, -0.80f, -0.45f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.2f, -0.80f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.2f, -0.85f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.2f, -0.85f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.2f, -0.85f, -0.45f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.2f, -0.80f, -0.45f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Right Face         //Positive X Normal
     0.6f, -0.80f, -0.45f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.6f, -0.80f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.6f, -0.85f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.6f, -0.85f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.6f, -0.85f, -0.45f, 1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.6f, -0.80f, -0.45f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Bottom Face        //Negative Y Normal
     0.2f, -0.85f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.6f, -0.85f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.6f, -0.85f, -0.45f, 0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.6f, -0.85f, -0.45f, 0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.2f, -0.85f, -0.45f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     0.2f, -0.85f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    //Top Face           //Positive Y Normal
    0.2f, -0.80f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    0.6f, -0.80f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    0.6f, -0.80f, -0.45f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.6f, -0.80f, -0.45f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.2f, -0.80f, -0.45f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    0.2f, -0.80f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateLaptopTopMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
       -0.2f, -0.99f, 0.195f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.1f, -0.99f, 0.195f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.1f, -0.79f, 0.195f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.1f, -0.79f, 0.195f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       -0.2f, -0.79f, 0.195f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       -0.2f, -0.99f, 0.195f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

       //Front Face         //Positive Z Normal
      -0.2f, -0.99f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       0.1f, -0.99f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       0.1f, -0.79f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.1f, -0.79f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      -0.2f, -0.79f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
      -0.2f, -0.99f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      //Left Face          //Negative X Normal
     -0.2f, -0.79f,  0.2f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     -0.2f, -0.79f,  0.195f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     -0.2f, -0.99f,  0.195f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.2f, -0.99f,  0.195f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.2f, -0.99f,  0.2f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     -0.2f, -0.79f,  0.2f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Right Face         //Positive X Normal
     0.1f, -0.79f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.1f, -0.79f,  0.195f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.1f, -0.99f,  0.195f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.1f, -0.99f,  0.195f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.1f, -0.99f,  0.2f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.1f, -0.79f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Bottom Face        //Negative Y Normal
    -0.2f, -0.99f,  0.195f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.1f, -0.99f,  0.195f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.1f, -0.99f,  0.2f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.1f, -0.99f,  0.2f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.2f, -0.99f,  0.2f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.2f, -0.99f,  0.195f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    //Top Face           //Positive Y Normal
   -0.2f, -0.79f,  0.195f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    0.1f, -0.79f,  0.195f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    0.1f, -0.79f,  0.2f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.1f, -0.79f,  0.2f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
   -0.2f, -0.79f,  0.2f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
   -0.2f, -0.79f,  0.195f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateLaptopBottomMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
       -0.2f, -1.0f,   0.2f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.1f, -1.0f,   0.2f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.1f, -0.99f,  0.2f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.1f, -0.99f,  0.2f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       -0.2f, -0.99f,  0.2f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       -0.2f, -1.0f,   0.2f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
       
       //Front Face         //Positive Z Normal
      -0.2f, -1.0f,  0.4f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       0.1f, -1.0f,  0.4f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       0.1f, -0.99f, 0.4f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.1f, -0.99f, 0.4f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      -0.2f, -0.99f, 0.4f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
      -0.2f, -1.0f,  0.4f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      //Left Face          //Negative X Normal
     -0.2f, -0.99f,  0.4f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     -0.2f, -0.99f,  0.2f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     -0.2f, -1.0f,   0.2f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.2f, -1.0f,   0.2f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.2f, -1.0f,   0.4f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     -0.2f, -0.99f,  0.4f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Right Face         //Positive X Normal
     0.1f, -0.99f,  0.4f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.1f, -0.99f,  0.2f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.1f, -1.0f,   0.2f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.1f, -1.0f,   0.2f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.1f, -1.0f,   0.4f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.1f, -0.99f,  0.4f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Bottom Face        //Negative Y Normal
    -0.2f, -1.0f,  0.2f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.1f, -1.0f,  0.4f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.1f, -1.0f,  0.2f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.1f, -1.0f,  0.4f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.2f, -1.0f,  0.4f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.2f, -1.0f,  0.2f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    //Top Face           //Positive Y Normal
   -0.2f, -0.99f,  0.2f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    0.1f, -0.99f,  0.2f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    0.1f, -0.99f,  0.4f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.1f, -0.99f,  0.4f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
   -0.2f, -0.99f,  0.4f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
   -0.2f, -0.99f,  0.2f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreatePadMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
        0.15f, -1.0f,   0.3f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.3f, -1.0f,   0.3f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.3f, -0.998f, 0.3f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.3f, -0.998f, 0.3f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.15f, -0.998f, 0.3f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        0.15f, -1.0f,   0.3f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

       //Front Face         //Positive Z Normal
       0.15f, -1.0f,   0.4f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       0.3f, -1.0f,   0.4f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       0.3f, -0.998f, 0.4f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.3f, -0.998f, 0.4f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       0.15f, -0.998f, 0.4f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
       0.15f, -1.0f,   0.4f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      //Left Face          //Negative X Normal
      0.15f, -0.998f,  0.4f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      0.15f, -0.998f,  0.3f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      0.15f, -1.0f,    0.3f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.15f, -1.0f,    0.3f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.15f, -1.0f,    0.4f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
      0.15f, -0.998f,  0.4f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Right Face         //Positive X Normal
     0.3f, -0.998f,  0.4f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.3f, -0.998f,  0.3f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.3f, -1.0f,    0.3f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.3f, -1.0f,    0.3f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.3f, -1.0f,    0.4f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.3f, -0.998f,  0.4f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     //Bottom Face        //Negative Y Normal
     0.15f, -1.0f,  0.3f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.3f, -1.0f,  0.4f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.3f, -1.0f,  0.3f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.3f, -1.0f,  0.4f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.15f, -1.0f,  0.4f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     0.15f, -1.0f,  0.3f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    //Top Face           //Positive Y Normal
    0.15f, -0.998f,  0.3f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    0.3f, -0.998f,  0.3f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    0.3f, -0.998f,  0.4f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.3f, -0.998f,  0.4f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.15f, -0.998f,  0.4f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    0.15f, -0.998f,  0.3f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

// Implements the UCreateMesh function
void UCreateMouseMesh(GLMesh& mesh)
{
    // Position and Color data
    GLfloat verts[] = {
        //Positions          //Normals
        // ------------------------------------------------------
        //Back Face          //Negative Z Normal  Texture Coords.
        0.2f,  -1.0f,   0.35f,  0.0f,  0.0f, -1.0f,  0.4f, 0.4f,
        0.22f, -1.0f,   0.35f,  0.0f,  0.0f, -1.0f,  0.4f, 0.4f,
        0.22f, -0.99f,  0.35f,  0.0f,  0.0f, -1.0f,  0.4f, 0.4f,
        0.22f, -0.99f,  0.35f,  0.0f,  0.0f, -1.0f,  0.4f, 0.4f,
        0.2f,  -0.99f,  0.35f,  0.0f,  0.0f, -1.0f,  0.4f, 0.4f,
        0.2f,  -1.0f,   0.35f,  0.0f,  0.0f, -1.0f,  0.4f, 0.4f,

       //Front Face         //Positive Z Normal
       0.2f,  -1.0f,  0.4f,  0.0f,  0.0f,  1.0f,  0.4f, 0.4f,
       0.22f, -1.0f,  0.4f,  0.0f,  0.0f,  1.0f,  0.4f, 0.4f,
       0.22f, -0.99f, 0.4f,  0.0f,  0.0f,  1.0f,  0.4f, 0.4f,
       0.22f, -0.99f, 0.4f,  0.0f,  0.0f,  1.0f,  0.4f, 0.4f,
       0.2f,  -0.99f, 0.4f,  0.0f,  0.0f,  1.0f,  0.4f, 0.4f,
       0.2f,  -1.0f,  0.4f,  0.0f,  0.0f,  1.0f,  0.4f, 0.4f,

      //Left Face          //Negative X Normal
      0.2f, -0.99f,  0.4f,  -1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
      0.2f, -0.99f,  0.35f, -1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
      0.2f, -1.0f,   0.35f, -1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
      0.2f, -1.0f,   0.35f, -1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
      0.2f, -1.0f,   0.4f,  -1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
      0.2f, -0.99f,  0.4f,  -1.0f,  0.0f,  0.0f,  0.4f, 0.4f,

     //Right Face         //Positive X Normal
     0.22f, -0.99f,  0.4f,  1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -0.99f,  0.35f, 1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -1.0f,   0.35f, 1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -1.0f,   0.35f, 1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -1.0f,   0.4f,  1.0f,  0.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -0.99f,  0.4f,  1.0f,  0.0f,  0.0f,  0.4f, 0.4f,

     //Bottom Face        //Negative Y Normal
     0.2f,  -1.0f,  0.35f,  0.0f, -1.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -1.0f,  0.4f,   0.0f, -1.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -1.0f,  0.35f,  0.0f, -1.0f,  0.0f,  0.4f, 0.4f,
     0.22f, -1.0f,  0.4f,   0.0f, -1.0f,  0.0f,  0.4f, 0.4f,
     0.2f,  -1.0f,  0.4f,   0.0f, -1.0f,  0.0f,  0.4f, 0.4f,
     0.2f,  -1.0f,  0.35f,  0.0f, -1.0f,  0.0f,  0.4f, 0.4f,

    //Top Face           //Positive Y Normal
    0.2f,  -0.99f,  0.35f,  0.0f,  1.0f,  0.0f,  0.4f,  0.7f,
    0.22f, -0.99f,  0.35f,  0.0f,  1.0f,  0.0f,  0.65f, 0.7f,
    0.22f, -0.99f,  0.4f,   0.0f,  1.0f,  0.0f,  0.65f, 0.2f,
    0.22f, -0.99f,  0.4f,   0.0f,  1.0f,  0.0f,  0.65f, 0.2f,
    0.2f,  -0.99f,  0.4f,   0.0f,  1.0f,  0.0f,  0.4f,  0.2f,
    0.2f,  -0.99f,  0.35f,  0.0f,  1.0f,  0.0f,  0.4f,  0.7f
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}





void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
}


/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

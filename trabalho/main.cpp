#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "textFile.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
// imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
namespace GLMAIN
{
    GLFWwindow *window; // Storage For glfw window
    int frameBufferWidth = 800, frameBufferHeight = 800;
    bool paused = false; // Store whether the animation is paused
    GLuint vao;          // Storage For vao
    // Storage locations of uniforms
    GLint planetLocaionLoc, planetColorLoc, radiusLoc, mvpLoc, mvLoc,
        lightAmbientLoc, reflectAmbientLoc, lightPosLoc, planetAngleloc, textureLoc, textureVerLoc;
    GLuint elementBufferHandle;
    int highlightSphere = -3;

    // Store the locations of 6 spheres
    float planetlocations[12][3] =
        {
            {0.0f, 0.0f, 0.0f},
            {0.0f, 10.0f, 0.0f},
            {0.0f, 16.0f, 0.0f},
            {0.0f, 25.0f, 0.0f},
            {0.0f, 34.0f, 0.0f},
            {0.0f, 50.0f, 0.0f},
            {0.0f, 65.0f, 0.0f},
            {0.0f, 71.0f, 0.0f},
            {0.0f, 77.0f, 0.0f},
            {0.0f, 3.0f, 0.0f},
            {0.0f, 3.0f, 0.0f},
            {0.0f, 5.0f, 0.0f}};
    // Store the radius of 6 spheres
    float planerRadius[12] = {9.55f / 2.0f, 2.02f / 2.0f, 3.75f / 2.0f, 3.88f / 2.0f, 3.39f / 2.0f, 5.44f / 2.0f, 4.90f / 2.0f, 4.01f / 2.0f, 4.01f / 2.0f, 1.f / 2.0f, 1.f / 2.0f, 1.f / 2.0f};
    // Store the rotate speed of 6 spheres
    float planetSpeed[12] = {0.0f, 4.5f, 3.5f, 3.0f, 2.5f, 1.0f, .75f, .65f, .60f, 5.0f, 5.0f, 4.0f};
    // Store the angles of 6 spheres
    float planetAngle[12] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 45.0f, 90.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    // Store the distances to the star of the 6 spheres
    float planetDistance[12] = {0.0f, 10.0f, 16.0f, 25.0f, 34.0f, 50.0f, 65.0f, 71.0f, 77.0f, 3.0f, 3.0f, 5.0f}; // Planetdistance to the star.
    // Store the base colors  of the 6 spheres
    float planetColor[12][3] =
        {
            {1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 0.0f, 1.0f},
            {.8f, .4f, .1f}, // problematico
            {0.0f, .5f, .5f},
            {0.5f, .5f, .5f}, // problematico
            {0.7f, 0.7f, 0.7f},
            {0.7f, 0.7f, 0.7f},
            {0.7f, 0.7f, 0.7f}};

    struct Texture
    {
        GLuint id;
        int width, height;
    };

    Texture textures[9];

    // Points and faces of icosphere
    const float X = 0.525731112119133606f;
    const float Z = 0.850650808352039932f;
    const float N = 0.0f;
    const float Verts[12][3] =
        {
            {-X, N, Z}, {X, N, Z}, {-X, N, -Z}, {X, N, -Z}, {N, Z, X}, {N, Z, -X}, {N, -Z, X}, {N, -Z, -X}, {Z, X, N}, {-Z, X, N}, {Z, -X, N}, {-Z, -X, N}};
    const int Faces[20][3] =
        {
            {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1}, {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3}, {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6}, {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}};

}

char texts[9][18] = {
    "textos/sun",
    "textos/mercury",
    "textos/venus",
    "textos/earth",
    "textos/mars",
    "textos/jupiter",
    "textos/saturn",
    "textos/uranus",
    "textos/neptun",
};

int select_text;

char text[20000];

// Calulate the position of every sphere based on the angle and distance
void calcLocations()
{
    if (GLMAIN::paused)
        return;
    for (int i = 0; i < 9; i++)
    {
        GLMAIN::planetAngle[i] += GLMAIN::planetSpeed[i];
        while (GLMAIN::planetAngle[i] > 360.0)
            GLMAIN::planetAngle[i] -= 360.0;
        float tempAngle = (GLMAIN::planetAngle[i] / 180.0) * 3.14159;
        GLMAIN::planetlocations[i][0] = sin(tempAngle) * GLMAIN::planetDistance[i];
        GLMAIN::planetlocations[i][1] = cos(tempAngle) * GLMAIN::planetDistance[i];
    }
    int i = 9;
    GLMAIN::planetAngle[i] += GLMAIN::planetSpeed[i];
    while (GLMAIN::planetAngle[i] > 360.0)
        GLMAIN::planetAngle[i] -= 360.0;
    float tempAngle = (GLMAIN::planetAngle[i] / 180.0) * 3.14159;
    GLMAIN::planetlocations[i][0] = GLMAIN::planetlocations[3][0] + sin(tempAngle) * GLMAIN::planetDistance[i];
    GLMAIN::planetlocations[i][1] = GLMAIN::planetlocations[3][1] + cos(tempAngle) * GLMAIN::planetDistance[i];

    i = 10;

    GLMAIN::planetAngle[i] += GLMAIN::planetSpeed[i];
    while (GLMAIN::planetAngle[i] > 360.0)
        GLMAIN::planetAngle[i] -= 360.0;
    tempAngle = (GLMAIN::planetAngle[i] / 180.0) * 3.14159;
    GLMAIN::planetlocations[i][0] = GLMAIN::planetlocations[4][0] + sin(tempAngle) * GLMAIN::planetDistance[i];
    GLMAIN::planetlocations[i][1] = GLMAIN::planetlocations[4][1] + cos(tempAngle) * GLMAIN::planetDistance[i];

    i = 11;

    GLMAIN::planetAngle[i] += GLMAIN::planetSpeed[i];
    while (GLMAIN::planetAngle[i] > 360.0)
        GLMAIN::planetAngle[i] -= 360.0;
    tempAngle = (GLMAIN::planetAngle[i] / 180.0) * 3.14159;
    GLMAIN::planetlocations[i][0] = GLMAIN::planetlocations[4][0] + sin(tempAngle) * GLMAIN::planetDistance[i];
    GLMAIN::planetlocations[i][1] = GLMAIN::planetlocations[4][1] + cos(tempAngle) * GLMAIN::planetDistance[i];
    GLMAIN::planetlocations[i][2] = GLMAIN::planetlocations[4][2] + 1;
    
}

// Generate perspective projection matrix
void initPerspective(glm::mat4 &m)
{
    const float aspect = 1;
    const float zNear = 10.0f;
    const float zFar = -1.0f;
    const float zRange = zNear - zFar;
    const float tanHalfFOV = tanf(0.5);
    m[0][0] = 1.0f / (tanHalfFOV * aspect);
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f / tanHalfFOV;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = (zNear + zFar) / (zNear - zFar);
    m[2][3] = 2.0f * zFar * zNear / (zNear - zFar);
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = -1.0f;
    m[3][3] = 0.0f;
}
glm::mat4 mvp = glm::mat4(1.0f);
glm::mat4 projectionMatrix;

glm::mat4 modelMatrix = glm::mat4(3.0f);
glm::mat4 viewMatrix;
float theta = 0.0;
float radius = 60;
float x, y, z = 40;
glm::vec3 camPos = glm::vec3(x, y, z);
// Display method, draw six spheres.
void initVAO();

void display(void)
{

    initPerspective(projectionMatrix);

    initVAO();
    glEnable(GL_DEPTH_TEST);

    // Init view matrix
    // camera position
    x = radius * cos(theta);
    y = radius * sin(theta);
    camPos = glm::vec3(x, y, z);
    const glm::vec3 lookAt(0.0, 0.0, 0.0);
    const glm::vec3 camOffset = lookAt - camPos;
    const glm::vec3 camForward = camOffset /
                                 glm::length(camOffset);
    const glm::vec3 camUp0(.0f, 0.0f, -1.0f);
    const glm::vec3 camRight = glm::cross(camForward, camUp0);
    const glm::vec3 camUp = glm::cross(camRight, camForward);

    const glm::mat4 viewRotation(
        camRight.x, camUp.x, camForward.x, 0.f, // column 0
        camRight.y, camUp.y, camForward.y, 0.f, // column 1
        camRight.z, camUp.z, camForward.z, 0.f, // column 2
        0.f, 0.f, 0.f, 1.f);                    // column 3
    const glm::mat4 viewTranslation(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        camPos.x, camPos.y, camPos.z, 1);
    viewMatrix = viewRotation * viewTranslation;
    glm::mat4 modelViewMareix = viewMatrix * modelMatrix;

    // model -view - projection matrix
    mvp = projectionMatrix * viewMatrix * modelMatrix;

    // Set uniform  mvp
    float mvpFloat[16];
    for (int i = 0; i < 4; i++)
    {
        mvpFloat[i * 4] = mvp[i].x;
        mvpFloat[i * 4 + 1] = mvp[i].y;
        mvpFloat[i * 4 + 2] = mvp[i].z;
        mvpFloat[i * 4 + 3] = mvp[i].w;
    }
    if (GLMAIN::mvpLoc != -1)
        glUniformMatrix4fv(GLMAIN::mvpLoc, 1, false, mvpFloat);

    // Set uniform  mv
    float mvFloat[16];
    for (int i = 0; i < 4; i++)
    {
        mvFloat[i * 4] = modelViewMareix[i].x;
        mvFloat[i * 4 + 1] = modelViewMareix[i].y;
        mvFloat[i * 4 + 2] = modelViewMareix[i].z;
        mvFloat[i * 4 + 3] = modelViewMareix[i].w;
    }
    if (GLMAIN::mvLoc != -1)
        glUniformMatrix4fv(GLMAIN::mvLoc, 1, false, mvFloat);

    glEnable(GL_TEXTURE_2D);

    // Draw 6 spheres, the first one is the star
    for (int i = 0; i < 12; i++)
    {
        glBindVertexArray(GLMAIN::vao);
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        glLoadName(i);
        // set planet location
        if (GLMAIN::planetLocaionLoc != -1)
        {

            glUniform3fv(GLMAIN::planetLocaionLoc, 1, &GLMAIN::planetlocations[i][0]);
        }

        // set the raduis of current sphere
        if (GLMAIN::radiusLoc != -1)
            glUniform1f(GLMAIN::radiusLoc, GLMAIN::planerRadius[i]);
        // set color od current sphere
        if (GLMAIN::planetColorLoc != -1)
            glUniform3fv(GLMAIN::planetColorLoc, 1, GLMAIN::planetColor[i]);

        if (GLMAIN::lightAmbientLoc != -1 && GLMAIN::reflectAmbientLoc != -1)
        {
            if (i == 0) // It is the star
            {
                // The star has a bright ambient light.
                float la[3] = {1.0f, 1.0f, 1.0f};
                float ra[3] = {1.0f, 1.0f, 1.0f};
                glUniform3fv(GLMAIN::lightAmbientLoc, 1, la);
                glUniform3fv(GLMAIN::reflectAmbientLoc, 1, ra);
            }
            else if (GLMAIN::highlightSphere == i)
            {
                // The highlighted planet
                float la[3] = {1.0f, 1.0f, 1.0f};
                float ra[3] = {1.0f, 1.0f, 1.0f};
                glUniform3fv(GLMAIN::lightAmbientLoc, 1, la);
                glUniform3fv(GLMAIN::reflectAmbientLoc, 1, ra);
            }
            else
            {
                // The normal planets have normal ambient light.
                float la[3] = {0.8f, 0.8f, 0.8f};
                float ra[3] = {0.5f, 0.5f, 0.5f};
                glUniform3fv(GLMAIN::lightAmbientLoc, 1, la);
                glUniform3fv(GLMAIN::reflectAmbientLoc, 1, ra);
            }
        }
        if (GLMAIN::textureLoc != -1)
        {
            glUniform1i(GLMAIN::textureLoc, 0); // 0 corresponds to GL_TEXTURE0
        }

        // Set point light
        if (GLMAIN::lightPosLoc != -1)
        {
            float lp[3] = {0.0f, 0.0f, 0.0f}; // Only have one point light source located at the center of the star(the first sphere).
            glUniform3fv(GLMAIN::lightPosLoc, 1, lp);
        }
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GLMAIN::textures[i].id);

        // Draw your object

        glDrawElements(GL_PATCHES, sizeof(GLMAIN::Faces), GL_UNSIGNED_INT, (void *)0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void initVAO() // Init vao, vbo.
{

    glGenVertexArrays(1, &GLMAIN::vao);
    glBindVertexArray(GLMAIN::vao);

    GLuint positionBufferHandle;
    glGenBuffers(1, &positionBufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLMAIN::Verts), GLMAIN::Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &GLMAIN::elementBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLMAIN::elementBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLMAIN::Faces), GLMAIN::Faces, GL_STATIC_DRAW);
}
GLuint ProgramObject;
// Init shaders.
int setShaders()
{
    GLint vertCompiled, fragCompiled;
    GLint tcsCompiled, tesCompiled, gsCompiled;
    ProgramObject = glCreateProgram();
    GLint linked;
    char *vs = NULL, *fs = NULL;
    char *cs = NULL, *es = NULL, *gs = NULL;

    GLuint VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    GLuint tcsObject = glCreateShader(GL_TESS_CONTROL_SHADER);
    GLuint tesObject = glCreateShader(GL_TESS_EVALUATION_SHADER);
    GLuint gsObject = glCreateShader(GL_GEOMETRY_SHADER);

    vs = textFileRead((char *)"test.vert"); // vertex shader
    fs = textFileRead((char *)"test.frag"); // fragment shader
    cs = textFileRead((char *)"test.cont"); // TCS shader
    es = textFileRead((char *)"test.eval"); // TES shader
    gs = textFileRead((char *)"test.gs");   // Geometry shader

    glUseProgram(ProgramObject);

    // Load source code into shaders.
    glShaderSource(VertexShaderObject, 1, (const char **)&vs, NULL);
    glShaderSource(FragmentShaderObject, 1, (const char **)&fs, NULL);

    glShaderSource(tcsObject, 1, (const char **)&cs, NULL);
    glShaderSource(tesObject, 1, (const char **)&es, NULL);
    glShaderSource(gsObject, 1, (const char **)&gs, NULL);

    // Compile the  vertex shader.
    glCompileShader(VertexShaderObject);
    glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &vertCompiled);
    // Compile the fragment shader
    glCompileShader(FragmentShaderObject);
    glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &fragCompiled);
    // Compile the tessellation control shader
    glCompileShader(tcsObject);
    glGetShaderiv(tcsObject, GL_COMPILE_STATUS, &tcsCompiled);
    // Compile the tessellation evaluate shader
    glCompileShader(tesObject);
    glGetShaderiv(tesObject, GL_COMPILE_STATUS, &tesCompiled);
    // Compile the geometry shader
    glCompileShader(gsObject);
    glGetShaderiv(gsObject, GL_COMPILE_STATUS, &gsCompiled);

    if (!vertCompiled || !fragCompiled || !tcsCompiled || !tesCompiled || !gsCompiled)
    {
        printf("Shader compile failed, vertCompiled:%d, fragCompiled:%d, tcsCompiled:%d, tesCompiled:%d, gsCompiled:%d\n",
               vertCompiled, fragCompiled, tcsCompiled, tesCompiled, gsCompiled);
        return 0;
    }

    glAttachShader(ProgramObject, VertexShaderObject);
    glAttachShader(ProgramObject, FragmentShaderObject);
    glAttachShader(ProgramObject, tcsObject);
    glAttachShader(ProgramObject, tesObject);
    glAttachShader(ProgramObject, gsObject);

    glLinkProgram(ProgramObject);
    glGetProgramiv(ProgramObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        // Print logs if link shaders failed.
        GLsizei len;
        glGetProgramiv(ProgramObject, GL_INFO_LOG_LENGTH, &len);
        GLchar *log = new GLchar[len + 1];
        glGetProgramInfoLog(ProgramObject, len, &len, log);
        printf("Shader linking failed: %s\n", log);
        delete[] log;
        return 0;
    }

    glUseProgram(ProgramObject);

    GLMAIN::planetLocaionLoc = glGetUniformLocation(ProgramObject, "planetLocaion");
    GLMAIN::radiusLoc = glGetUniformLocation(ProgramObject, "radius");
    GLMAIN::planetColorLoc = glGetUniformLocation(ProgramObject, "planetColor");
    GLMAIN::mvpLoc = glGetUniformLocation(ProgramObject, "mvp");
    GLMAIN::mvLoc = glGetUniformLocation(ProgramObject, "mv");
    GLMAIN::lightAmbientLoc = glGetUniformLocation(ProgramObject, "La");   // //Ambient light intensity
    GLMAIN::reflectAmbientLoc = glGetUniformLocation(ProgramObject, "Ra"); // Ambient reflectivity
    GLMAIN::lightPosLoc = glGetUniformLocation(ProgramObject, "lightPos"); // Ambient reflectivity
    GLMAIN::textureLoc = glGetUniformLocation(ProgramObject, "ourTexture");

    /*glDeleteShader(VertexShaderObject);
    glDeleteShader(FragmentShaderObject);
    glDeleteShader(tcsObject);
    glDeleteShader(tesObject);
    glDeleteShader(gsObject);
    glDeleteProgram(ProgramObject);*/
    return 1;
}

// callback of key event
void loadtext()
{
    FILE *file = fopen(texts[select_text], "r"); // open the file at the given position
    strcpy(text, "");
    if (file != NULL)
    { // check if the file was successfully opened
        char line[256];
        while (fgets(line, sizeof(line), file))
        {                       // read each line of the file
            strcat(text, line); // add the line to the text variable
            strcat(text, "\n"); // add a newline character
        }
        fclose(file); // close the file
    }
    else
    {
        fprintf(stderr, "Error: Unable to open file at position %d\n", select_text);
    }
}

void keyfunc(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) // If space key is pressed, pause/resume animation
    {
        GLMAIN::paused = !GLMAIN::paused;
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        if (radius > 20)
        {
            radius -= 5;
            z -= 0.5;
        }
        else
        {
            z -= 1;
        }
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        if (z > 5 && radius < 100)
        {
            radius += 5;
            z += 0.5;
        }
        else
        {

            z += 1;
        }
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        theta += 0.1;
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        theta -= 0.1;
    }
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        select_text = 0;
        loadtext();
    }
    else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        select_text = 1;
        loadtext();
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        select_text = 2;
        loadtext();
    }
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        select_text = 3;
        loadtext();
    }
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        select_text = 4;
        loadtext();
    }
    else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        select_text = 5;
        loadtext();
    }
    else if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        select_text = 6;
        loadtext();
    }
    else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    {
        select_text = 7;
        loadtext();
    }
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    {
        select_text = 8;
        loadtext();
    }
}

void loadTexture(const char *fileName, int i)
{
    GLMAIN::Texture texture;
    int width, height, numComponents;
    unsigned char *imageData = stbi_load(fileName, &width, &height, &numComponents, STBI_rgb);

    if (imageData == NULL)
    {
        printf("Unable to load texture: %s\n", fileName);
        return;
    }

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData);

    texture.width = width;
    texture.height = height;

    GLMAIN::textures[i] = texture;
}

// If the frame buffer is resized by resing the window, update viewport and frameBufferWidth/frameBufferHeight
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{

    glViewport(0, 0, width, height);
    GLMAIN::frameBufferWidth = width;
    GLMAIN::frameBufferHeight = height;
}

// Entry point
int main(int argc, char *argv[])
{
    // init glfw
    if (!glfwInit())
        return -1;

    // create a windowed mode window and its OpenGL context
    GLMAIN::window = glfwCreateWindow(GLMAIN::frameBufferWidth, GLMAIN::frameBufferHeight, "Solar System", NULL, NULL);
    if (!GLMAIN::window)
    {
        glfwTerminate();
        return -1;
    }
    char textures[10][18] = {
        "textures/img0.jpg",
        "textures/img1.jpg",
        "textures/img2.jpg",
        "textures/img3.jpg",
        "textures/img4.jpg",
        "textures/img5.jpg",
        "textures/img6.jpg",
        "textures/img7.jpg",
        "textures/img8.jpg",
        "textures/img9.jpg",
    };
    glfwSwapInterval(1);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // make the window's context current
    glfwMakeContextCurrent(GLMAIN::window);
    glfwSetKeyCallback(GLMAIN::window, keyfunc);
    glfwSetFramebufferSizeCallback(GLMAIN::window, framebuffer_size_callback);

    // init glew
    glewExperimental = GL_TRUE;
    glewInit();

    setShaders();
    initVAO();

    static double limitFPS = 1.0 / 30.0; // limit to 30 frames per second
    double lastTime = glfwGetTime();
    double deltaTime = 0, nowTime = 0;

    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    for (int i = 0; i < 10; i++)
    {
        loadTexture(textures[i], i);
    }
    IMGUI_CHECKVERSION();
    const char *glsl_version = "#version 400";
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(GLMAIN::window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
    /* Loop until the user closes the window */
    select_text = 0;
    loadtext();

    while (!glfwWindowShouldClose(GLMAIN::window))
    {
        nowTime = glfwGetTime();
        deltaTime += (nowTime - lastTime) / limitFPS;
        lastTime = nowTime;

        if (deltaTime < 1.0)
            continue;
        // - Only update at 30 frames / second
        while (deltaTime >= 1.0)
        {
            deltaTime--;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(ProgramObject);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        calcLocations();

        display(); //  Render function
        ImGui::Begin("Info");
        ImGui::Text(text);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(GLMAIN::window);

        // looking for events
        glfwPollEvents();
        for (int i = 0; i < 12; i++)
        {
            glRotatef(GLMAIN::planetAngle[i], 0.0f, 0.0f, 1.0f);
        }
    }
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}

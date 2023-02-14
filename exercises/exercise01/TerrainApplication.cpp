#include "TerrainApplication.h"

#include <cmath>
#include <iostream>
#include <ituGL/geometry/VertexAttribute.h>
#include <vector>
#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

// Helper structures. Declared here only for this exercise
struct Vector2
{
    Vector2() : Vector2(0.f, 0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3
{
    Vector3() : Vector3(0.f,0.f,0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    Vector3 Normalize() const
    {
        float length = std::sqrt(1 + x * x + y * y);
        return Vector3(x / length, y / length, z / length);
    }
};

struct Vertex
{
    Vector3 position;
    Vector2 texturePos;
    Vector3 color;
    Vector3 normal;

    Vertex(Vector3 position, Vector2 texture, Vector3 color, Vector3 normal) : position(position), texturePos(texture), color(color), normal(normal) {}
};


TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(128), m_gridY(128), m_shaderProgram(0), vbo(), vao(), ebo(), vertexCount(0), wireframe(false)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    // Build shaders and store in m_shaderProgram
    BuildShaders();
    
    glEnable(GL_DEPTH_TEST);

    VertexAttribute positionAttrib = VertexAttribute(Data::Type::Float, 3, GL_FALSE);
    VertexAttribute textureAttrib = VertexAttribute(Data::Type::Float, 2, GL_FALSE);
    VertexAttribute colorAttrib = VertexAttribute(Data::Type::Float, 3, GL_FALSE);
    VertexAttribute normalAttrib = VertexAttribute(Data::Type::Float, 3, GL_FALSE);

    std::vector<Vertex> vertices;

    std::vector<int> indices;

    for (float x = 0; x < m_gridX+1; x += 1.0f)
    {
        for (float y = 0; y < m_gridY+1; y += 1.0f)
        {
            float calcX = (x / m_gridX) - 0.5f;
            float calcY = (y / m_gridY) - 0.5f;

            float calcZ = stb_perlin_fbm_noise3(calcX * 8.0f, calcY * 8.0f, 0.0f, 2.0f, 0.5f, 6);

            Vector3 pos = Vector3(calcX, calcY, calcZ * 0.1f);
            Vector2 tex = Vector2(x, y);
            Vector3 color;
            if (calcZ <= -0.3f)
            {
                color = Vector3(0.0f, 0.0f, 0.85f);
            } 
            else if (calcZ <= -0.05f)
            {
                color = Vector3(0.8f, 0.8f, 0.1f);
            }
            else if (calcZ <= 0.05f) 
            {
                color = Vector3(0.1f, 0.8f, 0.3f);
            }
            else if (calcZ <= 0.55f) 
            {
                color = Vector3(0.1f, 0.1f, 0.1f);
            }
            else
            {
                color = Vector3(0.95f, 0.95f, 0.98f);
            }

            Vector3 normal = Vector3();

            vertices.push_back(Vertex(pos, tex, color, normal));
        }
    }

    for (size_t x = 0; x < m_gridX + 1; x++)
    {
        for (size_t y = 0; y < m_gridY + 1; y++)
        {
            int selfI =     (x * (m_gridY + 1)) + y;
            int leftI =     ((x - 1) * (m_gridY + 1)) + y;
            int rightI =    ((x + 1) * (m_gridY + 1)) + y;
            int upI =       (x * (m_gridY + 1)) + (y + 1);
            int downI =     (x * (m_gridY + 1)) + (y - 1);
            Vector3 left, right, up, down;
            if (x == 0)
            {
                left = vertices[selfI].position;
            }
            else 
            {
                left = vertices[leftI].position;
            }
            if (x == m_gridX)
            {
                right = vertices[selfI].position;
            }
            else 
            {
                right = vertices[rightI].position;
            }
            if (y == 0)
            {
                down = vertices[selfI].position;
            }
            else 
            {
                down = vertices[downI].position;
            }
            if (y == m_gridY)
            {
                up = vertices[selfI].position;
            }
            else 
            {
                up = vertices[upI].position;
            }
            float deltaX = (right.z - left.z) / (right.x - left.x);
            float deltaY = (up.z - down.z) / (up.y - down.y);
            vertices[selfI].normal = Vector3(deltaX, deltaY, 1.0f).Normalize();
        }
    }

    for (size_t x = 1; x < m_gridX+1; x++)
    {
        for (size_t y = 1; y < m_gridY+1; y++)
        {
            int bot_left =  ((y - 1) * (m_gridX+1)) + (x - 1);
            int top_left =  (y * (m_gridX+1)) + (x - 1);
            int bot_right = ((y - 1) * (m_gridX+1)) + x;
            int top_right = (y * (m_gridX+1)) + x;
            indices.push_back(bot_left);
            indices.push_back(top_left);
            indices.push_back(bot_right);
            indices.push_back(bot_right);
            indices.push_back(top_left);
            indices.push_back(top_right);
        }
    }

    vertexCount = (m_gridX+1) * (m_gridY+1);

    vao.Bind();
    vbo.Bind();
    ebo.Bind();

    ebo.AllocateData(std::span(indices));
    vbo.AllocateData(std::span(vertices));

    vao.SetAttribute(0, positionAttrib, 0, sizeof(Vertex));
    vao.SetAttribute(1, textureAttrib, sizeof(Vector3), sizeof(Vertex));
    vao.SetAttribute(2, colorAttrib, sizeof(Vector3) + sizeof(Vector2), sizeof(Vertex));
    vao.SetAttribute(3, normalAttrib, sizeof(Vector3) + sizeof(Vector2) + sizeof(Vector3), sizeof(Vertex));

    vao.Bind();
    vbo.Bind();
    ebo.Bind();

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void TerrainApplication::Update()
{
    Application::Update();

    UpdateOutputMode();
}

void TerrainApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Set shader to be used
    glUseProgram(m_shaderProgram);

    glDrawElements(GL_TRIANGLES, 6 * vertexCount, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}

void TerrainApplication::BuildShaders()
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in vec3 aColor;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "uniform mat4 Matrix = mat4(1);\n"
        "out vec2 texCoord;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "void main()\n"
        "{\n"
        "   texCoord = aTexCoord;\n"
        "   color = aColor;\n"
        "   normal = aNormal;\n"
        "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "uniform uint Mode = 0u;\n"
        "in vec2 texCoord;\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   switch (Mode)\n"
        "   {\n"
        "   default:\n"
        "   case 0u:\n"
        "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "       break;\n"
        "   case 1u:\n"
        "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
        "       break;\n"
        "   case 2u:\n"
        "       FragColor = vec4(color, 1.0f);\n"
        "       break;\n"
        "   case 3u:\n"
        "       FragColor = vec4(normalize(normal), 1.0f);\n"
        "       break;\n"
        "   case 4u:\n"
        "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
        "       break;\n"
        "   }\n"
        "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    m_shaderProgram = shaderProgram;
}

void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}

#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <iostream>
#include <chrono>
#define M_PI           3.14159265358979323846  /* pi */

int buildShaderProgram();
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const unsigned int circleEdges = 1024*32;
const float rotationSpeed = 100.0f;
const double length = sqrt(2) / 2;

const bool wireframe = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    DeviceGL deviceGL;

    // glfw window creation
    // --------------------
    Window window(SCR_WIDTH, SCR_HEIGHT, "HelloWorld");
    if (!window.IsValid())
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    deviceGL.SetCurrentWindow(window);
    if (!deviceGL.IsReady())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    int shaderProgram = buildShaderProgram();

    std::vector<double> vertices;
    std::vector<int> triangleIndices;

    // Add origin
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (size_t i = 0; i < circleEdges; i++)
    {
        double angle = (360.0 / circleEdges) * i;
        vertices.push_back(std::sin(angle * (M_PI / 180)) * length);
        vertices.push_back(std::cos(angle * (M_PI / 180)) * length);
        vertices.push_back(0.0f);

        triangleIndices.push_back(0);
        triangleIndices.push_back(i+1);
        if (i == circleEdges-1)
            triangleIndices.push_back(1);
        else
            triangleIndices.push_back(i+2);
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //float vertices[] = {
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //     0.5f,  0.5f, 0.0f,  // top right
    //    -0.5f,  0.5f, 0.0f   // top left 
    //};
    //unsigned int indices[] = {  // note that we start from 0!
    //    0, 1, 2,   // first triangle
    //    2, 0, 3    // second triangle
    //};

    int triangleCount = triangleIndices.size();

    VertexBufferObject vbo = VertexBufferObject();
    VertexArrayObject vao = VertexArrayObject();
    ElementBufferObject ebo = ElementBufferObject();
   
    vao.Bind();
    vbo.Bind();
    ebo.Bind();

    ebo.AllocateData(std::span(triangleIndices));
    vbo.AllocateData(std::span(vertices));

    vao.SetAttribute(0, VertexAttribute(Data::Type::Double, 3, GL_FALSE), 0);

    vbo.Unbind();
    vao.Unbind();
    ebo.Unbind();

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //float time = 0.0f;

    // render loop
    // -----------
    while (!window.ShouldClose())
    {
        auto t_start = std::chrono::high_resolution_clock::now();
        // input
        // -----
        processInput(window.GetInternalWindow());

        // render
        // ------
        deviceGL.Clear(0.4f, 0.1f, 0.2f, 1.0f);

        // draw our first triangle
        glUseProgram(shaderProgram);
        vao.Bind();
        vbo.Bind();
        ebo.Bind();
        
        //for (size_t i = 0; i < circleEdges; i++)
        //{
        //    float t_angle = 45 + (i * 90);
        //    vertices[0 + (i * 3)] = sqrt(2) / 2 * std::sin((t_angle + (time * rotationSpeed)) * (M_PI / 180));
        //    vertices[1 + (i * 3)] = sqrt(2) / 2 * std::cos((t_angle + (time * rotationSpeed)) * (M_PI / 180));
        //}

        //vbo.UpdateData(std::span(vertices, floatCount));

        glDrawElements(GL_TRIANGLES, triangleCount, GL_UNSIGNED_INT, 0);

        //glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window.SwapBuffers();
        deviceGL.PollEvents();

        // a bit slower than 1 real life second = 1.0f added
        //time += std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - t_start).count();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    vao.~VertexArrayObject();
    vbo.~VertexBufferObject();
    ebo.~ElementBufferObject();
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // This is now done in the destructor of DeviceGL
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// build the shader program
// ------------------------
int buildShaderProgram() 
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.5f, 0.2f, 1.0f, 1.0f);\n"
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
    return shaderProgram;
}

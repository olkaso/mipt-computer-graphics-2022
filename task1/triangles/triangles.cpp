// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	window = glfwCreateWindow( 1024, 768, "Triangles", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID1 = LoadShaders( "SimpleTransform.vertexshader", "SingleColor.fragmentshader" );
    GLuint programID2 = LoadShaders( "SimpleTransform2.vertexshader", "SingleColor2.fragmentshader" );


	GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");

    GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");

    static const GLfloat g_vertex_buffer_data_1[] = {
            -0.3f, -0.3f, 0.0f,
            0.3f, -0.3f, 0.0f,
            0.0f,  0.3f, 0.0f,
    };
    static const GLfloat g_vertex_buffer_data_2[] = {
            -0.1f, -0.1f, 0.4f,
            0.4f, -0.1f, 0.3f,
            0.3f,  0.4f, 0.6f,
    };

    GLuint vertexbuffer1;
    glGenBuffers(1, &vertexbuffer1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_1), g_vertex_buffer_data_1, GL_STATIC_DRAW);

    GLuint vertexbuffer2;
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_2), g_vertex_buffer_data_2, GL_STATIC_DRAW);
    float i = 0;
	do{
	    i += 0.1;
	    if (i > 10000) i = 0;
	    Sleep(30);
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 7.0f/3.0f, 0.1f, 100.0f);
        glm::mat4 View       = glm::lookAt(
                glm::vec3(std::sin(i)/3, std::cos(i), 2),
                glm::vec3(std::cos(i), 0, std::sin(i)/3),
                glm::vec3(0,-1, 0)
        );

        glm::mat4 Model      = glm::mat4(1.0f);

        glm::mat4 MVP        = Projection * View * Model;

		glClear( GL_COLOR_BUFFER_BIT );

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(programID1);

		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);


        glUseProgram(programID2);
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
        glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );


        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

        glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteProgram(programID1);
    glDeleteBuffers(1, &vertexbuffer2);
    glDeleteProgram(programID2);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


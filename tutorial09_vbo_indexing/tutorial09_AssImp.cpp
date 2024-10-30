// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <iostream>

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 09 - Loading with AssImp", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	//GLuint vertexArrayID = glGetUniformLocation(programID, "JustGreen");
	
	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
	//GLuint vertexArrayID = glGetAttribLocation(programID, "JustGreen");

	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	bool res = loadAssImp("suzanne.obj", indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
	
	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

    	//-------------------------------------------------------------------------
    	//drawing square
    	float rectangleVertices[] =
            {
                    -2.5f, -2.5f, 0.0f,
                    -2.5f,  2.5f, 0.0f,
                    2.5f,	2.5f, 0.0f,
                    2.5f, -2.5f, 0.0f,
            };

    	float rectangleNormals[] =
            {
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
            };

    	unsigned int rectangleIndices[] =
            {
                0, 1, 3,
                1, 2, 3
            };

	GLuint JustGreen = glGetUniformLocation(programID, "JustGreen");

    	unsigned int squareVAO, squareVBO, squareEBO, squareNorm;
    	glGenVertexArrays(1,&squareVAO);
    	glGenBuffers(1,&squareVBO);
    	glGenBuffers(1,&squareEBO);
    	glGenBuffers(1,&squareNorm);

    	glBindVertexArray(squareVAO);
    	glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareNorm);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleNormals), rectangleNormals, GL_STATIC_DRAW);
    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), rectangleIndices, GL_STATIC_DRAW);

    	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    	glEnableVertexAttribArray(0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glBindVertexArray(0);

	do{
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        	// Compute the MVP matrix from keyboard and mouse input
        	computeMatricesFromInputs();
        	glm::mat4 ProjectionMatrix = getProjectionMatrix();
        	glm::mat4 ViewMatrix = getViewMatrix();
	
		// Use our shader
		glUseProgram(programID);
        	glm::vec3 lightPos = glm::vec3(4,4,4);
        	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"

        	glm::mat4 ModelMatrix1 = glm::mat4(1.0);
        	glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1;
	
		glUniform1i(JustGreen,1); //setting JustGreen equal to 1
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
        	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1[0][0]);
	
        	glBindVertexArray(squareVAO);
        	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glUniform1i(JustGreen,0);
		
		//glBindVertexArray(vertexArrayID);

		std::vector<glm::mat4> headTransforms;
		headTransforms.push_back(glm::translate(glm::mat4(1.0), glm::vec3(1.83f, 0.0f, 0.0f))); //right head
		headTransforms.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 1.83f, 0.0f))); //top head
		headTransforms.push_back(glm::translate(glm::mat4(1.0), glm::vec3(-1.83f, 0.0f, 0.0f))); //left head
		headTransforms.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -1.83f, 0.0f))); //bottom head

		for (int i = 0; i < 4; i++)
		{
			headTransforms[i] = glm::rotate(headTransforms[i], glm::radians(90.0f), glm::vec3(1,0,0));
		}

		headTransforms[0] = glm::rotate(headTransforms[0], glm::radians(90.0f), glm::vec3(0,1,0));
		headTransforms[1] = glm::rotate(headTransforms[1], glm::radians(180.0f), glm::vec3(0,1,0));
		headTransforms[2] = glm::rotate(headTransforms[2], glm::radians(270.0f), glm::vec3(0,1,0));


		//creating 4 instances centered around z=0
		for (int i = 0; i < 4; i++)
		{
			glm::mat4 ModelMatrix = headTransforms[i]; //this is applying the transformation //glm::mat4(1.0);
			//ModelMatrix = glm::translate(ModelMatrix, glm::vec3(i * 4.0f, j * 4.0f, 0.0));
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			//now we are sending transformation to the shader
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Bind our texture in Texture Unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture);
			// Set our "myTextureSampler" sampler to user Texture Unit 0
			glUniform1i(TextureID, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(vertexPosition_modelspaceID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				vertexPosition_modelspaceID, // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : UVs
			glEnableVertexAttribArray(vertexUVID);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			glVertexAttribPointer(
				vertexUVID,                       // attribute
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// 3rd attribute buffer : normals
			glEnableVertexAttribArray(vertexNormal_modelspaceID);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
			glVertexAttribPointer(
				vertexNormal_modelspaceID,        // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

			// Draw the triangles !
			glDrawElements(
				GL_TRIANGLES,      // mode
				indices.size(),    // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0           // element array buffer offset
			);

		}
		
	glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(vertexUVID);
        glDisableVertexAttribArray(vertexNormal_modelspaceID);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	//glDeleteBuffers(1, &greenbuffer);


	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


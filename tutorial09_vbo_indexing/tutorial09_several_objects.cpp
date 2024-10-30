/*
Author: Abby McCollam
Class: ECE4122 Section A
Last Date Modified: 10/28/23
Description:

Main file for implementing 4 suzanne objects and green rectangle

*/

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
	window = glfwCreateWindow( 1024, 768, "Tutorial 09 - Rendering several models", NULL, NULL);
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
	//glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint JustGreen = glGetUniformLocation(programID, "JustGreen");
	

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("suzanne.obj", vertices, uvs, normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

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

    //------------------------------------------------------------------------------
    //drawing square
    //RECTANGLE VERTICES
    float rectangleVertices[] =
            {
                    -2.5f, -2.5f, 0.0f,
                    -2.5f,  2.5f, 0.0f,
                    2.5f,	2.5f, 0.0f,
                    2.5f, -2.5f, 0.0f,
            };
    //RECTANGLE NORMALS
    float rectangleNormals[] =
            {
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
            };

    //RECTANGLE INDICES
    unsigned int rectangleIndices[] =
            {
                    0, 1, 3,
                    1, 2, 3
            };
  
    GLuint squareEBO; //stores ID for element buffer object
    glGenBuffers(1,&squareEBO); //generates 1 EBO
    glBindBuffer(GL_ARRAY_BUFFER, squareEBO); //binds EBO to target
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW); //copies data from rectangleIndices and binds

    //SAME IDEA FOR REST	
    GLuint squareNorm;
    glGenBuffers(1,&squareNorm);
    glBindBuffer(GL_ARRAY_BUFFER, squareNorm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleNormals), rectangleNormals, GL_STATIC_DRAW);

    GLuint squareVBO;
    glGenBuffers(1,&squareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    //------------------------------------------------------------------------------

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

        ////// Start of the rendering of the first object //////

		// Use our shader
		glUseProgram(programID);

        glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"

        //BEGINNING OF FIRST OBJECT
        glm::mat4 ModelMatrix1 = glm::mat4(1.0f);
        ModelMatrix1 = glm::translate(ModelMatrix1, glm::vec3(0.0f, -2.0f, 1.0f)); //translating first object
        ModelMatrix1 = glm::rotate(ModelMatrix1, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotating first object

        glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID, // The attribute we want to configure
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
			vertexUVID,                       // The attribute we want to configure
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,        // The attribute we want to configure
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

		////// End of rendering of the first object //////
		////// Start of the rendering of the second object //////

		// In our very specific case, the 2 objects use the same shader.
		// So it's useless to re-bind the "programID" shader, since it's already the current one.
		//glUseProgram(programID);

		// Similarly : don't re-set the light position and camera matrix in programID,
		// it's still valid !
		// *** You would have to do it if you used another shader ! ***
		//glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		//glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"

		// Again : this is already done, but this only works because we use the same shader.
		//// Bind our texture in Texture Unit 0
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Texture);
		//// Set our "myTextureSampler" sampler to user Texture Unit 0
		//glUniform1i(TextureID, 0);

        //BEGINNING OF SECOND OBJECT
		// BUT the Model matrix is different (and the MVP too)
		glm::mat4 ModelMatrix2 = glm::mat4(1.0);
		ModelMatrix2 = glm::translate(ModelMatrix2, glm::vec3(2.0f, 0.0f, 1.0f)); //translating to new position and rotating
        ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ModelMatrix2 = glm::rotate(ModelMatrix2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix2[0][0]);

		// The rest is exactly the same as the first object

		// 1rst attribute buffer : vertices
		//glEnableVertexAttribArray(vertexPosition_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		//glEnableVertexAttribArray(vertexUVID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : normals
		//glEnableVertexAttribArray(vertexNormal_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
		//END OF RENDERING SECOND OBJECT

        //BEGINNING OF THIRD OBJECT
        glm::mat4 ModelMatrix3 = glm::mat4(1.0);
        ModelMatrix3 = glm::translate(ModelMatrix3, glm::vec3(0.0f, 2.0f, 1.0f)); //translating to new position and rotating
        ModelMatrix3 = glm::rotate(ModelMatrix3, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ModelMatrix3= glm::rotate(ModelMatrix3, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix3[0][0]);

        // The rest is exactly the same as the first object

        // 1rst attribute buffer : vertices
        //glEnableVertexAttribArray(vertexPosition_modelspaceID); // Already enabled
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer : UVs
        //glEnableVertexAttribArray(vertexUVID); // Already enabled
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 3rd attribute buffer : normals
        //glEnableVertexAttribArray(vertexNormal_modelspaceID); // Already enabled
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Draw the triangles !
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        //END THIRD OBJECT

        //BEGINNING OF  FOURTH OBJECT
        glm::mat4 ModelMatrix4 = glm::mat4(1.0);
        ModelMatrix4 = glm::translate(ModelMatrix4, glm::vec3(-2.0f, 0.0f, 1.0f)); //translating to new position and rotating
        ModelMatrix4 = glm::rotate(ModelMatrix4, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ModelMatrix4= glm::rotate(ModelMatrix4, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * ModelMatrix4;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix4[0][0]);

        // The rest is exactly the same as the first object

        // 1rst attribute buffer : vertices
        //glEnableVertexAttribArray(vertexPosition_modelspaceID); // Already enabled
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer : UVs
        //glEnableVertexAttribArray(vertexUVID); // Already enabled
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 3rd attribute buffer : normals
        //glEnableVertexAttribArray(vertexNormal_modelspaceID); // Already enabled
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Draw the triangles !
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        //END RENDERING FOURTH OBJECT

        //BEGINNING OF RECTANGLE
        //Drawing rectangle
        GLuint JustGreen = glGetUniformLocation(programID, "JustGreen");
	    glUniform1i(JustGreen, 1); //setting JustGreen to 1 colors in rectangle green
	    glm::mat4 squareModelMatrix = glm::mat4(1.0f);
        squareModelMatrix = glm::translate(squareModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 squareMVP = ProjectionMatrix * ViewMatrix * squareModelMatrix;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &squareMVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &squareModelMatrix[0][0]);

        //repeat process for rectangle object
        glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
        glVertexAttribPointer(
                vertexPosition_modelspaceID, // The attribute we want to configure
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );

        glBindBuffer(GL_ARRAY_BUFFER, squareNorm);
        glVertexAttribPointer(
                vertexUVID, // The attribute we want to configure
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );

        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
                vertexNormal_modelspaceID,        // The attribute we want to configure
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);

        glDrawArrays(GL_QUADS, 0, 4);
        
        glUniform1i(JustGreen, 0); //need to set JustGreen to 0 now

	    //END OF  RENDERING RECTANGLE

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

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


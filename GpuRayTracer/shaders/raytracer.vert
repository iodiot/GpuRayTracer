#version 130

// In
in vec3 vertexPosition;
in vec3 vertexTexCoord;

// Out
out vec3 texCoord;
out vec3 cameraW, cameraU, cameraV;
out vec3 cameraRotatedPosition;

// Camera
uniform vec3 cameraPosition;
uniform vec3 cameraLookAt;
uniform vec3 cameraUp; 
uniform mat4 rotationMatrix;

void main()
{
	cameraRotatedPosition = (rotationMatrix * vec4(cameraPosition, 1.0)).xyz;

	// Compute camera basis
	cameraW = normalize(cameraRotatedPosition - cameraLookAt);
	cameraU = normalize(cross(cameraUp, cameraW));
	cameraV = cross(cameraW, cameraU);

	texCoord = vertexTexCoord;
	gl_Position = vec4(vertexPosition, 1.0);
}
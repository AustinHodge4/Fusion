#version 460
#extension GL_ARB_separate_shader_objects : enable

void main() {
    const vec3 positions[3] = vec3[3](
		vec3(1.0,1.0, 0.0),
		vec3(-1.0,1.0, 0.0),
		vec3(0.0,-1.0, 0.0)
	);

	//output the position of each vertex
	gl_Position = vec4(positions[gl_VertexIndex], 1.0);
}
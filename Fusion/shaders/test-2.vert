#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 fragColor;

void main() {
    const vec3 positions[3] = vec3[3](
		vec3(1.0,1.0, 0.0),
		vec3(-1.0,1.0, 0.0),
		vec3(0.0,-1.0, 0.0)
	);

	const vec3 colors[3] = vec3[3](
		vec3(1.0f, 0.0f, 0.0f), //red
		vec3(0.0f, 1.0f, 0.0f), //green
		vec3(00.f, 0.0f, 1.0f)  //blue
	);

	//output the position of each vertex
	gl_Position = vec4(positions[gl_VertexIndex], 1.0);
	fragColor = colors[gl_VertexIndex];
}
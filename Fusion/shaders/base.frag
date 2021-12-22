#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC -1, 1
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
    //outColor = vec4(fragTexCoord, 0.0, 1.0);
	//vec4 color = texture(texSampler, fragTexCoord);
	//if(color.a < 0.5)
		//discard;
	//color = vec4(vec3(gl_FragColor.z), 1.0);
    //outColor = color;

	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    outColor = vec4(vec3(depth), 1.0);

//	vec3 customColor = vec3(1.0, 1.0, 1.0);
//	vec3 lightDir = vec3(-0.5, -1, -0.5);
//
//    vec3 norm = normalize(vec3(1));
//	vec3 lightDirection = normalize(-lightDir);
//	vec3 viewDir = normalize(-fragPos); // The viewer is at (0,0,0) so viewDir is (0,0,0) - Position => -Position
//
//	float ambient = 0.1f;
//	float diffuse = max(dot(lightDirection, norm), 0.0f);
//	vec3 reflectDir = reflect(-lightDirection, norm);
//	float specular = pow(max(dot(viewDir, reflectDir),0.0f), 32.0f);
//  
//	float intensity = ambient + diffuse + specular;
//
//	outColor = vec4(customColor * intensity * texture(texSampler, fragTexCoord).rgb, 1.0f);
	//float influence = dot(vec3(0.0f, 1.0f, 0.0f), norm) * 0.5f + 0.5f;
	//color = vec4(mix(vec3(0.0f, 0.4f, 0.0f), vec3(0.6f, 0.6f, 1.0f), influence), 1.0f);
}
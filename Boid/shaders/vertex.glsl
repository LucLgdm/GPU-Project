#version 430 core

struct Boid {
	vec4 position;
	vec4 velocity;
};

layout(std430, binding = 0) readonly buffer BoidBuffer {
    Boid boids[];
};

layout(location = 1) in vec3 aPos;

void main() {
	Boid boid = boids[gl_InstanceID];
	
	gl_Position = boid.position + vec4(aPos, 0.0);
}

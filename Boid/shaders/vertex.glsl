#version 430 core

struct Boid {
	vec4 position;
	vec4 velocity;
};

layout(std430, binding = 0) readonly buffer BoidBuffer {
    Boid boids[];
};

layout(location = 1) in vec3 aPos;

uniform mat4 uMVP;

out vec3 vColor;

mat3 makeRotation(vec3 forward) {
	// if forward is parallel to vec3(0, 1, 0);
	vec3 up = abs(forward.y) > 0.999 ? vec3(0, 0, 1) : vec3(0, 1, 0);
	vec3 right = normalize(cross(forward, up));
	up = normalize(cross(right, forward));

	return mat3(right, forward, up);
}

void main() {
	Boid boid = boids[gl_InstanceID];

	vec3 forward;
	if (length(boid.velocity.xyz) < 0.0001)
		forward = vec3(0.0, 1.0, 0.0);
	else
		forward = normalize(boid.velocity.xyz);
	mat3 rotationMatrix = makeRotation(forward);

	vec3 rotatedPos = rotationMatrix * aPos;
	gl_Position = uMVP * (boid.position + vec4(rotatedPos, 0.0));

	// vColor = forward * 0.5 + 0.5;
	vColor = vec3(1.0f, 0.6f, 0.2f);
}

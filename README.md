# GPU Rendering and Simulation Projects

This repository contains three graphics and simulation projects developed to explore GPU programming, real-time rendering, and physically based rendering techniques.

## Projects

### Boid Simulation
A GPU-accelerated implementation of Craig Reynolds' Boids algorithm for simulating flocking behavior. The simulation demonstrates emergent collective motion using separation, alignment, and cohesion rules.

### Ray Caster
A real-time ray casting renderer running entirely on the GPU. This project explores ray generation, intersection tests, camera control, and interactive visualization.

### Path Tracer
A physically based GPU path tracer featuring:

- CUDA implementation
- Bounding Volume Hierarchy (BVH) acceleration structure
- Directional and spot lights
- Texture mapping
- Progressive accumulation
- Russian Roulette path termination
- Interactive scene editor with ImGui

## Technologies

- C++
- CUDA
- OpenGL
- GLSL
- ImGui
- GLFW
- GLM

## Goals

The purpose of these projects is to deepen understanding of:

- GPU programming
- Parallel computing
- Real-time graphics
- Ray tracing algorithms
- Physically based rendering
- Rendering acceleration structures

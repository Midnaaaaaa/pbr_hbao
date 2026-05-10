# Real-Time Physically Based Rendering Engine (OpenGL)

Real-time rendering project developed in OpenGL featuring physically based rendering materials, GGX microfacet shading, and screen-space ambient occlusion techniques.

---

## Preview

### Renderer Demo

[![Watch the video](https://img.youtube.com/vi/reN-rrXhxTQ/maxresdefault.jpg)](https://www.youtube.com/watch?v=reN-rrXhxTQ)

---

## Features

- Physically Based Rendering (PBR) workflow
- Metallic and roughness material system
- GGX microfacet BRDF implementation
- Cook-Torrance lighting model
- HDR rendering pipeline
- HBAO (Horizon-Based Ambient Occlusion) screen-space shaders
- Real-time dynamic lighting system
- Fully GLSL shader-based rendering architecture

---

## Rendering Pipeline

The renderer is built around a modern physically based pipeline:

- Energy-conserving lighting model
- Cook-Torrance BRDF for realistic light interaction
- GGX distribution for specular highlights
- HDR rendering for high dynamic range lighting
- Tone mapping for final display output

---

## Material System

The material system follows a standard PBR workflow:

- Albedo (base color)
- Metallic factor
- Roughness control
- Normal mapping support
- Environment-based reflections

This allows consistent and physically plausible shading across all assets.

---

## Screen-Space Effects

Advanced post-processing techniques are implemented in screen space:

- HBAO for improved depth-based ambient occlusion
- Screen-space lighting enhancements
- High-quality shadow approximation techniques
- Optimized for real-time performance

---

## Shading Architecture

The engine is fully shader-driven using GLSL:

- Modular shader design
- Separate passes for geometry, lighting, and post-processing
- Efficient GPU-based computation
- Real-time material evaluation per fragment

---

## Real-Time Lighting

The system supports dynamic lighting:

- Multiple light sources
- Real-time updates
- Physically accurate attenuation
- Specular and diffuse interaction

---

## Technologies

- C++
- OpenGL
- GLSL
- Physically Based Rendering (PBR)
- GGX Microfacet Model
- Screen-Space Effects (SSAO/HBAO)
- Real-Time Rendering Techniques

---

## Applications

This project is ideal for:

- Learning modern rendering pipelines
- Shader programming practice
- Graphics research and experimentation
- Real-time visualization systems
- Game engine rendering studies

---

## Future Work

- Image-based lighting (IBL) improvements
- Path tracing hybrid integration
- Temporal anti-aliasing (TAA)
- GPU-based particle systems
- Vulkan migration experiments

---

## License

MIT License

# OpenGL Project: 3D Scene Rendering with Camera Navigation and Effects

## 1. Subject Specification
This project involves developing a 3D scene using OpenGL, featuring full camera navigation, automated scene tours, lighting effects, texturing, object animations, and additional graphical enhancements. The objective is to create an interactive and visually impressive scene.

## 2. Scenario

### 2.1 Scene and Objects Description
The scene consists of multiple 3D objects (house, trees, plants, lake and lanterns), including a larger composite object (a windmill) made up of two smaller objects. The objects are properly textured and arranged to create a visually appealing environment that takes you into a forgotten forest paradise. The composite object also features a rotating windmill blades animation.

### 2.2 Functionalities
- **Camera Navigation:**
  - Full movement in the scene using keyboard and mouse.
  - Automated camera tour highlighting key points in the scene.
- **Lighting:**
  - One directional light source (toggleable via keyboard).
  - One point light source (toggleable via keyboard).
- **Rendering Modes:**
  - Wireframe view.
  - Point view using `GL_POINTS` and `GL_LINES`.
- **Animations:**
  - At least one animated object (e.g., rotating windmill blades).
- **Effects:**
  - Five different visual effects, including fog, transparency, fragment discarding, additional point lights, and rain.

## 3. Implementation Details

### 3.1 Lighting Model
The scene incorporates OpenGL’s lighting model with both directional and point light sources. Lighting calculations include ambient, diffuse, and specular components, implemented using the Phong shading model. Shadows and reflections are considered to enhance realism.

### 3.2 Camera Navigation
Camera control allows free movement across the scene using first-person and third-person perspectives. An automated camera tour is implemented using parametric equations, interpolating between key viewpoints to create a smooth transition.

### 3.3 Effects
Five visual effects are included to improve scene realism:
1. **Fog** – Simulated atmospheric conditions.
2. **Fragment Discarding** – Used to create transparent or irregularly shaped objects.
3. **Transparency** – Implemented using alpha blending.
4. **Additional Point Lights** – Enhancing lighting realism beyond the initial two sources.
5. **Rain Effect** – Simulating raindrops using particle systems.

## 4. Conclusions and Further Developments
This project successfully implements a 3D scene with interactive camera navigation, lighting, textures, animations, and advanced effects. Future improvements could include:
- Dynamic weather changes.
- Additional animation details.
- More complex shaders for realistic materials.

## 5. References
- OpenGL Programming Guide
- Various online tutorials and documentation
- Blender for 3D model creation

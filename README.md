# **3D Character Animation in OpenGL**
A real-time **3D character animation system** implemented in **C++ and OpenGL**, featuring **skeletal animation, keyframe interpolation, and shader-based rendering**. The project enables animation playback, supports character walking with user input, and uses **Assimp for FBX model loading**.

## **Table of Contents**
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Classes and Implementation](#classes-and-implementation)
- [Animation System](#animation-system)
- [Character Control](#character-control)
- [Rendering Pipeline](#rendering-pipeline)

---

## **Overview**
This project demonstrates **skeletal animation** in OpenGL using **bone transformations and keyframe interpolation**. The character’s movement is controlled via keyboard inputs, allowing the user to walk forward, turn, and stop based on the applied animation states.

### **Core Functionalities:**
✅ **Skeletal Animation** using bone transformations and keyframe interpolation  
✅ **Real-time Rendering** with OpenGL shaders  
✅ **Animation Import** from FBX files via **Assimp**  
✅ **User Input-Based Movement** controlling walking animations  
✅ **Phong Shading** for realistic lighting on the animated character  

---

## **Features**
🔹 **Bone-based Animation:** Uses a **hierarchical skeleton** with transformations applied per frame  
🔹 **Keyframe Interpolation:** Smooth animation transitions between keyframes  
🔹 **Model Loading:** Assimp loads 3D character models with bones and animations  
🔹 **Custom Shader Program:** Vertex and fragment shaders handle animation transformations  
🔹 **Interactive Character Movement:** Walk and rotate the character using user inputs  

---

## **Project Structure**
📂 **3DCharacterAnimation** *(root folder)*
```
│── src/                  # Source code files
│   ├── main.cpp          # Entry point, handles OpenGL setup and main loop
│   ├── Character.cpp     # Manages character animation logic
│   ├── Renderer.cpp      # Handles OpenGL rendering pipeline
│   ├── InputHandler.cpp  # Processes keyboard input for controlling the character
│   ├── Animation.cpp     # Core skeletal animation logic
│   ├── Shader.cpp        # Shader loading and management
│   ├── ModelLoader.cpp   # Uses Assimp to load 3D models
│── shaders/              # GLSL shader programs
│   ├── vertex_shader.glsl
│   ├── fragment_shader.glsl
│── assets/               # Contains 3D character models and animations
│── CMakeLists.txt        # Build configuration
│── README.md             # Documentation
```

---

## **Classes and Implementation**
### **1️⃣ Character Class (`Character.cpp`)**
- Represents the **animated 3D character**.
- Stores **bone hierarchy**, animation states, and **interpolates keyframes** for smooth transitions.
- **Key Methods:**
  - `update(float deltaTime)`: Updates animation based on time progression.
  - `setAnimation(std::string animationName)`: Switches between different animations.
  - `processInput(int key)`: Reacts to keyboard input for movement.

### **2️⃣ Animation Class (`Animation.cpp`)**
- Manages **skeletal animation**, applying transformations to bones per frame.
- Uses **keyframe interpolation** to compute bone positions, ensuring smooth animation.
- **Key Methods:**
  - `loadAnimation(std::string filePath)`: Loads animation data from an FBX file.
  - `calculateBoneTransform(float time)`: Computes **bone transformations** for a given frame.
  - `blendAnimations(Animation* a1, Animation* a2, float blendFactor)`: Supports **blended animation transitions**.

### **3️⃣ Renderer Class (`Renderer.cpp`)**
- Handles **rendering** of the character using OpenGL.
- Sends **bone transformation matrices** to the vertex shader.
- **Key Methods:**
  - `drawCharacter(Character& character)`: Renders the animated character.
  - `setLighting()`: Configures **Phong shading** for realistic lighting effects.

### **4️⃣ ModelLoader Class (`ModelLoader.cpp`)**
- Uses **Assimp** to import FBX character models with skeleton data.
- Loads **meshes, bones, and animation keyframes** from the file.
- **Key Methods:**
  - `loadModel(std::string filePath)`: Parses a 3D model file and extracts animation data.
  - `extractBoneData()`: Retrieves bone transformations for animation.

### **5️⃣ InputHandler Class (`InputHandler.cpp`)**
- Handles **user input for movement and animation switching**.
- Uses `GLFW` to detect **WASD** and other key presses.
- **Key Methods:**
  - `processKeyboardInput(GLFWwindow* window)`: Detects keypresses and updates character movement.

---

## **Animation System**
### **How Animation Works**
1. The **FBX model** contains **bones and keyframe animations**.
2. The **bone transformations** are stored and processed **per frame**.
3. The animation system **calculates bone positions using interpolation**.
4. **Each bone matrix** is sent to the vertex shader to apply **skeletal deformation**.

### **Keyframe Interpolation**
- Each bone's position, rotation, and scale are stored at different keyframes.
- The **current frame's transformations** are computed by **linearly interpolating** between the closest keyframes.
- This ensures **smooth motion** when transitioning between animation frames.

### **Vertex Shader for Skeletal Animation**
```glsl
uniform mat4 boneTransforms[MAX_BONES];

void main() {
    vec4 skinnedPosition = boneTransforms[boneIndex] * vec4(position, 1.0);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * skinnedPosition;
}
```
- The **bone transformation matrices** are applied to the character’s vertices.
- **Skinning weights** determine how much influence each bone has on the mesh.

---

## **Character Control**
### **Walking & Movement with Inputs**
- The character’s walking animation is **tied to keyboard input**.
- **Keybindings:**
  - **W** → Move Forward
  - **A/D** → Turn Left / Right
  - **S** → Stop Walking
  - **Space** → Jump (if implemented)

---

## **Rendering Pipeline**
1. **Character Mesh & Skeleton Data Loaded** → `ModelLoader`
2. **Animation Processed & Keyframes Interpolated** → `Animation`
3. **Bone Transformations Sent to GPU via Shader** → `Renderer`
4. **Final Animated Character Drawn on Screen** 🎮

---

## **Dependencies**
- **OpenGL** – Graphics rendering
- **GLFW** – Window and input handling
- **GLM** – Mathematics for 3D transformations
- **Assimp** – FBX model loading
- **GLEW** – OpenGL function loading

---

## **Conclusion**
This project demonstrates **real-time character animation** in OpenGL, using **skeletal animation, keyframe interpolation, and input-based movement**. 🚀

🎮 **Press `W` to walk, `S` to stop, `A/D` to turn!** 🚀

Here's your modified **README.md** that reflects all the recent updates in your project:

---

# **3D Character Animation in OpenGL**
A real-time **3D character animation system** implemented in **C++ and OpenGL**, featuring **skeletal animation, keyframe interpolation, shader-based rendering, and real-time character control**. The project utilizes **Assimp for FBX model loading**, **GLM for transformations**, and **OpenGL for rendering**.

## **Table of Contents**
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Core Components](#core-components)
- [Animation System](#animation-system)
- [Rendering Pipeline](#rendering-pipeline)
- [Character Control](#character-control)
- [Camera System](#camera-system)
- [Dependencies](#dependencies)
- [Conclusion](#conclusion)

---

## **Overview**
This project implements a **fully functional animation pipeline** in OpenGL with support for **skeletal animation, bone transformations, vertex skinning, and interpolation between animation states**. The character moves based on user input, transitioning between different animations **seamlessly**.

### **Core Functionalities:**
✅ **Skeletal Animation** with bone transformations and keyframe interpolation  
✅ **Real-time Rendering** using OpenGL shaders  
✅ **Animation Import** from FBX files via **Assimp**  
✅ **Character Movement** controlled by user input  
✅ **Phong Shading** for realistic lighting effects  
✅ **Bone Weight Processing** to apply proper transformations  

---

## **Features**
🔹 **Hierarchical Bone-Based Animation** - Uses **bone transformations** and **keyframe interpolation** for smooth animations  
🔹 **Model Loading with Assimp** - Supports **FBX**, **OBJ**, and other 3D model formats  
🔹 **Custom Shader Program** - Vertex and fragment shaders handle **bone transformations**, lighting, and textures  
🔹 **Blended Animation States** - Smooth transitions between animations using **linear interpolation**  
🔹 **User-Controlled Character Movement** - Walking, turning, and stopping via **keyboard inputs**  
🔹 **Quaternion-Based Camera Control** - Implements **smooth rotation and free movement**  

---

## **Project Structure**
📂 **3DCharacterAnimation** *(root folder)*
```
│── src/                  # Source code files
│   ├── main.cpp          # Entry point, handles OpenGL setup and main loop
│   ├── Model.h         # Loads character model and animations and handles keyframe interpolation and bone transformations
│   ├── Mesh.h          # Stores vertex data and handles rendering
│   ├── Shader.h        # Manages OpenGL shader compilation and linking
│   ├── FPSController.cpp # Processes player movement and input
│   ├── Camera.cpp        # Controls the view and movement
│── shaders/              # GLSL shader programs
│   ├── vertex_shader.glsl
│   ├── fragment_shader.glsl
│── assets/               # Contains 3D character models, textures, and animations
│── README.md             # Documentation
```

---

## **Core Components**
### **1️⃣ Model Class (`Model.h`)**
- Loads and stores **meshes, bones, and animations**.
- Updates **bone transformations per frame**.
- **Key Methods:**
  - `loadModel(path)`: Loads the **3D model** using Assimp.
  - `setActiveAnimation(name)`: Switches **active animation**.
  - `applyPose(timeStep)`: Updates the **current animation state**.

### **2️⃣ Mesh Class (`Mesh.h`)**
- Represents **character meshes**.
- Handles **vertex data, textures, and bone weights**.
- **Key Methods:**
  - `setupMesh()`: Configures **VAO, VBO, EBO**.
  - `Draw(shader)`: Renders the **character mesh**.

### **3️⃣ Inside MOdel.h**
- Stores **bone keyframes** and interpolates transformations.
- Supports **blending animations smoothly**.
- **Key Methods:**
  - `getPose(time)`: Computes the **bone transformations**.
  - `blendPose(prev, current, alpha)`: Blends between animations.

### **4️⃣ Shader Class (`Shader.cpp`)**
- Manages **OpenGL shader programs**.
- Sends **bone transformation matrices** to the GPU.
- **Key Methods:**
  - `setMat4(name, value)`: Sends a **4x4 matrix uniform**.
  - `setVec3(name, value)`: Sends a **3D vector uniform**.

### **5️⃣ FPS Controller (`FPSController.cpp`)**
- Processes **keyboard input** to move the player.
- Locks the character's direction when moving.
- **Key Methods:**
  - `Move(event, player, camera, deltaTime)`: Handles movement logic.

### **6️⃣ Camera Class (`Camera.cpp`)**
- Implements **first-person camera movement**.
- Uses **quaternions** for **smooth rotations**.
- **Key Methods:**
  - `updateCameraVectors()`: Updates **view direction**.
  - `GetViewMatrix()`: Returns the **view matrix**.

---

## **Animation System**
### **How Animation Works**
1. The **FBX model** contains **bones and keyframe animations**.
2. The **bone transformations** are stored and processed **per frame**.
3. The system **interpolates bone positions using keyframe data**.
4. Each **bone matrix** is sent to the vertex shader to apply **skeletal deformation**.

### **Bone Weight Processing**
- Each **vertex** is influenced by **up to 4 bones**.
- The system **normalizes weights** for proper **vertex blending**.
- **Example GLSL Vertex Shader for Skeletal Animation:**
```glsl
uniform mat4 boneTransforms[MAX_BONES];

void main() {
    vec4 skinnedPosition = boneTransforms[boneIndex] * vec4(position, 1.0);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * skinnedPosition;
}
```
- **Skinning weights** determine **how much influence each bone has** on the mesh.

---

## **Rendering Pipeline**
1. **Mesh and Bone Data Loaded** → `ModelLoader`
2. **Animation Processed & Keyframes Interpolated** → `Animation`
3. **Bone Matrices Sent to Shader** → `Shader`
4. **Final Animated Character Drawn** 

---

## **Character Control**
### **Walking & Movement with Inputs**
- The character’s walking animation is **tied to keyboard input**.
- **Keybindings:**
  - **W** → Move Forward
  - **A/D** → Turn Left / Right
  - **S** → Backward
---

## **Camera System**
- Uses **quaternion-based rotation** for **smooth movement**.
- Updates **camera orientation** with mouse movement.
- **First-Person View** tracks the player's position.
```cpp
glm::quat qYaw = glm::angleAxis(glm::radians(x), camera->WorldUp);
glm::quat qPitch = glm::angleAxis(glm::radians(y), camera->Right);
camera->Orientation = camera->Orientation * qYaw * qPitch;
```

---

## **Dependencies**
- **OpenGL** – Real-time rendering
- **GLM** – Mathematics for 3D transformations
- **Assimp** – 3D model loading (FBX, OBJ, etc.)
- **SDL2** – Input handling (keyboard & mouse)
- **STB_Image** – Texture loading

---

## **Conclusion**
This project implements **real-time character animation in OpenGL**, supporting **skeletal animation, keyframe interpolation, animation blending, and real-time character control**. 

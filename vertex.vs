#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 bones[100]; // Assuming you have a maximum of 100 bones

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    mat4 boneTransform = mat4(1.0); // Initialize with identity matrix instead of zero matrix

    // Check if the vertex is influenced by any bones
    if (aWeights[0] + aWeights[1] + aWeights[2] + aWeights[3] > 0.0) {
        // Accumulate weighted bone transformations only if there are actual weights
        boneTransform = bones[aBoneIDs[0]] * aWeights[0];
        boneTransform += bones[aBoneIDs[1]] * aWeights[1];
        boneTransform += bones[aBoneIDs[2]] * aWeights[2];
        boneTransform += bones[aBoneIDs[3]] * aWeights[3];
    }

    // Transform vertex position
    vec4 transformedPos = boneTransform * vec4(aPos, 1.0);
    gl_Position = projection * view * model * transformedPos;

    // Pass texture coordinates
    TexCoords = aTexCoords;

    // Transform normal for lighting calculations
    mat3 normalMatrix = mat3(transpose(inverse(model * boneTransform)));
    Normal = normalMatrix * aNormal;

    // Pass transformed fragment position
    FragPos = vec3(model * transformedPos);
}

#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include <SDL.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include "TextureUtility.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AnimationEnum.h"

#ifndef uint
typedef unsigned int uint;
#endif

using namespace std;

// Utility functions
inline glm::vec3 assimpToGlmVec3(const aiVector3D& vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat assimpToGlmQuat(const aiQuaternion& quat) {
    return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

inline void updateBoneTransformations(Shader& shader, const vector<glm::mat4>& boneTransforms) {
    for (unsigned int i = 0; i < boneTransforms.size(); i++) {
        std::string name = "bones[" + std::to_string(i) + "]";
        shader.setMat4(name.c_str(), boneTransforms[i]);
    }
}

inline glm::mat4 lerp(const glm::mat4& a, const glm::mat4& b, float alpha) {
    return a * (1.0f - alpha) + b * alpha;
}

 inline glm::vec3 lerp(const glm::vec3 & a, const glm::vec3 & b, float alpha) {
     return a * (1.0f - alpha) + b * alpha;
 }

struct BoneInfo {
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransformation;
};

struct Bone {
    int id = 0;
    string name = "";
    glm::mat4 offset = glm::mat4(1.0f);
    vector<Bone> children = {};
};

struct BoneTransformTrack {
    vector<float> positionTimestamps = {};
    vector<float> rotationTimestamps = {};
    vector<float> scaleTimestamps = {};

    vector<glm::vec3> positions = {};
    vector<glm::quat> rotations = {};
    vector<glm::vec3> scales = {};
};

struct Animation {
    float duration = 0.0f;
    float ticksPerSecond = 1.0f;
    bool active = true;  // Flag to determine if the animation should be updated
    unordered_map<string, BoneTransformTrack> boneTransforms = {};
};


class Model
{
public:

    vector<Mesh> meshes;

    Model(string const& path, bool isCharacter, bool gamma = false) : gammaCorrection(gamma), isCharacter(isCharacter)
    {
        loadModel(path, isCharacter);
    }

    void Draw(Shader& shader, float alpha) {

        if (isCharacter)
        {
            // Interpolate between previous and current bone transformations based on alpha
            std::vector<glm::mat4> interpolatedTransforms = interpolateTransforms(prevBoneTransforms, boneTransforms, alpha);

            // Now use interpolatedTransforms to update shader uniforms
            updateBoneTransformations(shader, interpolatedTransforms);
        }

        // Draw each mesh with updated transformations
        for (unsigned int i = 0; i < meshes.size(); i++) {
            meshes[i].Draw(shader);
        }
    }

    void applyPose(float timeStep) {
        static Animation* prevAnimation = nullptr;
        Animation* currentAnimation = getActiveAnimation();
        if (!currentAnimation) return;

        if (prevAnimation != currentAnimation) {
            blendPose(prevAnimation, currentAnimation, timeStep);
            prevAnimation = currentAnimation;
        }

        currentAnimationTime += timeStep;
        if (currentAnimationTime > currentAnimation->duration) {
            currentAnimationTime -= currentAnimation->duration;
        }

        float adjustedTime = currentAnimationTime;
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 inverseIdentityMatrix = glm::inverse(identityMatrix);
        getPose(*currentAnimation, skeleton, adjustedTime, boneTransforms, identityMatrix, inverseIdentityMatrix);
    }

    void setActiveAnimation(const string& name) {
        if (currentAnimationName != name) {
            currentAnimationName = name;
            auto it = animations.find(name);
            if (it != animations.end()) {
                std::for_each(animations.begin(), animations.end(), [&](auto& pair) { pair.second.active = false; });
                it->second.active = true;
                currentAnimationTime = 0.0f;  // Reset only when changing animations
            }
        }

    }

    void updatePrevTransforms() {
        prevBoneTransforms = boneTransforms;  // Store current transforms as previous
    }

private:

    vector<Texture> textures_loaded;

    string directory;
    bool gammaCorrection;
    bool isCharacter;

    map<string, aiNodeAnim*> boneAnimations;
    const aiScene* scene;
    Assimp::Importer importer;

    vector<glm::mat4> boneTransforms;
    vector<glm::mat4> prevBoneTransforms;
    unordered_map<string, BoneInfo> boneInfoMap;
    Bone skeleton;
    Animation animation;
    std::map<std::string, Animation> animations;
    float currentAnimationTime = 0.0f;
    string currentAnimationName = "";

    bool fileExists(const string& path)
    {
        ifstream file(path);
        return file.good();
    }

    void loadModel(string const& path, bool isCharacter)
    {
        if (!fileExists(path))
        {
            cout << "ERROR::ASSIMP:: File not found." << endl;
            cout << "PATH: " << path.c_str() << endl;
            return;
        }

        scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_GlobalScale);

        if (!scene)
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            cout << "PATH: " << path.c_str() << endl;
            return;
        }

        if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        {
            cout << "ERROR::ASSIMP:: Scene is incomplete." << endl;
            cout << "PATH: " << path.c_str() << endl;
            return;
        }

        if (!scene->mRootNode)
        {
            cout << "ERROR::ASSIMP:: Root node is null." << endl;
            cout << "PATH: " << path.c_str() << endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));

        if (isCharacter)
        {
            skeleton.name = scene->mRootNode->mName.C_Str();
            skeleton.id = getBoneID(skeleton.name);
            parseBoneHierarchy(scene->mRootNode, skeleton, scene);
        }

        glm::mat4 globalTransform = glm::mat4(1.0f);
        processNode(scene->mRootNode, scene, globalTransform);
        if (isCharacter)
        {
            processAnimations(scene);
            setActiveAnimation(getAnimationString(IDLE));
        }
    }

    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform) {
        glm::mat4 nodeTransform = assimpToGlmMat4(node->mTransformation);
        glm::mat4 globalTransform = parentTransform * nodeTransform;

        //cout << "Node: " << node->mName.C_Str() << "Meshes: " << node->mNumMeshes << endl;
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, globalTransform));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, globalTransform);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& globalTransform) {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            glm::vec4 transformedPosition = globalTransform * glm::vec4(vector, 1.0f);
            vertex.Position = glm::vec3(transformedPosition);

            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                glm::vec4 transformedNormal = globalTransform * glm::vec4(vector, 0.0f);  // Normal is a direction vector, so w should be 0
                vertex.Normal = glm::normalize(glm::vec3(transformedNormal));
            }

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = glm::normalize(glm::vec3(globalTransform * glm::vec4(vector, 0.0f)));

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = glm::normalize(glm::vec3(globalTransform * glm::vec4(vector, 0.0f)));
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        if (isCharacter)
        {
            processBones(mesh, vertices);
        }

        return Mesh(vertices, indices, textures);
    }

    void processBones(aiMesh* mesh, vector<Vertex>& vertices)
    {
        for (unsigned int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];
            int boneID = getBoneID(bone->mName.C_Str());

            if (boneInfoMap.find(bone->mName.C_Str()) == boneInfoMap.end()) {
                BoneInfo boneInfo;
                boneInfo.offsetMatrix = assimpToGlmMat4(bone->mOffsetMatrix);
                boneInfoMap[bone->mName.C_Str()] = boneInfo;
            }

            Bone* skeletonBone = findBone(skeleton, bone->mName.C_Str());
            if (skeletonBone) {
                skeletonBone->offset = assimpToGlmMat4(bone->mOffsetMatrix);
            }

            for (unsigned int j = 0; j < bone->mNumWeights; j++)
            {
                aiVertexWeight weight = bone->mWeights[j];
                int vertexID = weight.mVertexId;
                float weightValue = weight.mWeight;
                addBoneData(vertices[vertexID], boneID, weightValue);
            }
        }
    }

    void addBoneData(Vertex& vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if (vertex.Weights[i] == 0.0f)
            {
                vertex.BoneIDs[i] = boneID;
                vertex.Weights[i] = weight;
                return;
            }
        }
    }

    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    void processAnimations(const aiScene* scene) {
        for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
            aiAnimation* ai_anim = scene->mAnimations[i];
            Animation animation;
            animation.duration = ai_anim->mDuration / (ai_anim->mTicksPerSecond != 0 ? ai_anim->mTicksPerSecond : 25.0f);
            animation.ticksPerSecond = ai_anim->mTicksPerSecond != 0 ? ai_anim->mTicksPerSecond : 25.0f;
            if (scene->mAnimations[i]->mName.C_Str() == getAnimationString(WALKING))
            {
                animation.duration = 30.9f;
                animation.ticksPerSecond = 30.f;
            }
            cout << "Animation: " << scene->mAnimations[i]->mName.C_Str() << " Duration: " << ai_anim->mDuration << " ticks" << ai_anim->mTicksPerSecond << endl;
            for (unsigned int j = 0; j < ai_anim->mNumChannels; j++) {
                aiNodeAnim* channel = ai_anim->mChannels[j];
                BoneTransformTrack track;

                for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
                    aiVectorKey key = channel->mPositionKeys[k];
                    track.positionTimestamps.push_back(static_cast<float>(key.mTime));
                    track.positions.push_back(assimpToGlmVec3(key.mValue));
                }

                for (unsigned int k = 0; k < channel->mNumRotationKeys; k++) {
                    aiQuatKey key = channel->mRotationKeys[k];
                    track.rotationTimestamps.push_back(static_cast<float>(key.mTime));
                    track.rotations.push_back(assimpToGlmQuat(key.mValue));
                }

                for (unsigned int k = 0; k < channel->mNumScalingKeys; k++) {
                    aiVectorKey key = channel->mScalingKeys[k];
                    track.scaleTimestamps.push_back(static_cast<float>(key.mTime));
                    track.scales.push_back(assimpToGlmVec3(key.mValue));
                }

                animation.boneTransforms[channel->mNodeName.C_Str()] = track;
            }

            animations[ai_anim->mName.C_Str()] = animation;
        }
    }

    glm::vec3 assimpToGlmVec3(const aiVector3D& vec) {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    glm::quat assimpToGlmQuat(const aiQuaternion& quat) {
        return glm::quat(quat.w, quat.x, quat.y, quat.z);
    }

    void parseBoneHierarchy(aiNode* node, Bone& parentBone, const aiScene* scene)
    {
        Bone bone;
        bone.name = node->mName.C_Str();
        bone.id = getBoneID(bone.name);
        bone.offset = glm::mat4(1.0f);

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            parseBoneHierarchy(node->mChildren[i], bone, scene);
        }

        parentBone.children.push_back(bone);
    }

    void getPose(Animation& animation, Bone& skeleton, float dt, std::vector<glm::mat4>& output, glm::mat4& parentTransform, glm::mat4& globalInverseTransform) {

        if (animation.boneTransforms.find(skeleton.name) == animation.boneTransforms.end()) {
            // If no animation data for this bone, use parent transform for children
            for (Bone& child : skeleton.children) {
                getPose(animation, child, dt, output, parentTransform, globalInverseTransform);
            }
            return;
        }

        BoneTransformTrack& btt = animation.boneTransforms[skeleton.name];
        dt = fmod(dt, animation.duration);  // Wrap time around the duration

        // Interpolate position
        std::pair<uint, float> fp = getTimeFraction(btt.positionTimestamps, dt, 1);
        glm::vec3 position = lerp(btt.positions[fp.first - 1], btt.positions[fp.first], fp.second);

        // Interpolate rotation
        fp = getTimeFraction(btt.rotationTimestamps, dt, 1);
        glm::quat rotation = glm::slerp(btt.rotations[fp.first - 1], btt.rotations[fp.first], fp.second);

        // Interpolate scale
        fp = getTimeFraction(btt.scaleTimestamps, dt, 1);
        glm::vec3 scale = lerp(btt.scales[fp.first - 1], btt.scales[fp.first], fp.second);

        glm::mat4 positionMat = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotationMat = glm::mat4_cast(rotation);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

        glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
        glm::mat4 globalTransform = parentTransform * localTransform;

        output[skeleton.id] = globalInverseTransform * globalTransform * skeleton.offset;

        for (Bone& child : skeleton.children) {
            getPose(animation, child, dt, output, globalTransform, globalInverseTransform);
        }
    }

    std::pair<uint, float> getTimeFraction(std::vector<float>& times, float time, uint startFrame) {
        if (times.empty()) {
            return { 0, 0.0f };  // Return defaults if no keyframes are present
        }

        if (times.size() == 1) {
            return { 0, 0.0f };  // Only one keyframe, no interpolation needed
        }

        uint frame = startFrame;
        while (frame < times.size() && time >= times[frame]) {
            frame++;
        }
        if (frame == 0) {  // Handle case where startFrame is 0 and no increment occurs
            return { 0, 0.0f };
        }

        // Ensure frame does not exceed bounds and is at least 1 to prevent underflow in array access
        frame = std::min(frame, static_cast<uint>(times.size() - 1));
        frame = std::max(1u, frame);

        float lastTime = times[frame - 1];
        float nextTime = times[frame];
        float fraction = (time - lastTime) / (nextTime - lastTime);

        return { frame, fraction };
    }

    int getBoneID(const string& boneName)
    {
        static map<string, int> boneIDMap;
        static int nextBoneID = 0;

        if (boneIDMap.find(boneName) == boneIDMap.end())
        {
            boneIDMap[boneName] = nextBoneID++;
            boneTransforms.push_back(glm::mat4(1.0f));
        }
        return boneIDMap[boneName];
    }

    glm::mat4 assimpToGlmMat4(const aiMatrix4x4& from) {
        glm::mat4 to;
        // Transpose matrix due to column-major order in glm
        to[0][0] = from.a1; to[1][0] = from.a2;
        to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2;
        to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2;
        to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2;
        to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    Bone* findBone(Bone& bone, const string& name)
    {
        if (bone.name == name) return &bone;
        for (auto& child : bone.children)
        {
            Bone* found = findBone(child, name);
            if (found) return found;
        }
        return nullptr;
    }

    Animation* getActiveAnimation() {
        auto it = animations.find(currentAnimationName);
        if (it != animations.end()) return &it->second;
        return nullptr;
    }

    std::vector<glm::mat4> interpolateTransforms(const std::vector<glm::mat4>& prevTransforms,
        const std::vector<glm::mat4>& currentTransforms, float alpha) {
        std::vector<glm::mat4> interpolatedTransforms;
        for (size_t i = 0; i < currentTransforms.size(); ++i) {
            glm::mat4 prevTransform = prevTransforms[i];
            glm::mat4 currentTransform = currentTransforms[i];

            glm::mat4 interpolated = lerp(prevTransform, currentTransform, alpha);
            interpolatedTransforms.push_back(interpolated);
        }
        return interpolatedTransforms;
    }

    void blendPose(Animation* prevAnimation, Animation* currentAnimation, float blendFactor) {
        if (!prevAnimation || !currentAnimation) return;

        // Example blending - adjust according to actual bone indices and ensure both animations are compatible
        for (auto& bone : skeleton.children) {
            auto prevTrack = prevAnimation->boneTransforms.find(bone.name);
            auto currentTrack = currentAnimation->boneTransforms.find(bone.name);
            if (prevTrack != prevAnimation->boneTransforms.end() && currentTrack != currentAnimation->boneTransforms.end()) {
                // Interpolate each bone's transform from the last frame of prevAnimation and the first frame of currentAnimation
                glm::vec3 position = lerp(prevTrack->second.positions.back(), currentTrack->second.positions.front(), blendFactor);
                glm::quat rotation = glm::slerp(prevTrack->second.rotations.back(), currentTrack->second.rotations.front(), blendFactor);
                glm::vec3 scale = lerp(prevTrack->second.scales.back(), currentTrack->second.scales.front(), blendFactor);

                glm::mat4 posMat = glm::translate(glm::mat4(1.0f), position);
                glm::mat4 rotMat = glm::mat4_cast(rotation);
                glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
                glm::mat4 finalTransform = posMat * rotMat * scaleMat;

                // Here, instead of directly setting, you might want to adjust how these are applied based on your skeleton structure
                boneTransforms[bone.id] = finalTransform;
            }
        }
    }
};
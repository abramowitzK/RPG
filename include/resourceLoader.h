#pragma once
#include <texture.h>
#include <utils.h>
#include <shader.h>

constexpr int MaxTextures = 64;
constexpr int MaxShaders = 8;
typedef u32 ResourceId;
const std::string ResourcePath = "./Data/";
const std::string TexturePath = ResourcePath + "Textures/";
const std::string ModelPath = ResourcePath + "Models/";
const std::string FontPath = ResourcePath + "Fonts/";
const std::string AudioPath = ResourcePath + "Sounds/";
const std::string ScenePath = ResourcePath + "Scenes/";
const std::string ShaderPath = ResourcePath + "Shaders/";
const std::string ScriptPath = ResourcePath + "Scripts/";
const std::string LevelPath = ResourcePath + "Levels/";

struct ResourceManager
{
    Texture Textures[MaxTextures];
    ResourceId CurrentTexture;
    Shader Shaders[MaxShaders];
    ResourceId CurrentShader;
};

ResourceId LoadTexture(ResourceManager *manager, const char *name)
{
    manager->Textures[manager->CurrentTexture] = create_2d_texture((TexturePath + name).c_str());
    strcpy_s(manager->Textures[manager->CurrentTexture].Name, 64, name);
    return manager->CurrentTexture++;
}

ResourceId LoadShader(ResourceManager *manager, std::string const &name)
{
    ShaderInit(&manager->Shaders[manager->CurrentShader]);
    ShaderLoadVS(&manager->Shaders[manager->CurrentShader], (ShaderPath + name + ".vert").c_str());
    ShaderLoadFS(&manager->Shaders[manager->CurrentShader], (ShaderPath + name + ".frag").c_str());
    return manager->CurrentShader++;
}
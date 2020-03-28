#pragma once
#include <glew.h>
#include <structures.h>

enum class BlendState : bool
{
    On = true,
    Off = false
};

enum class BlendFunc : GLenum
{
    Zero = GL_ZERO,
    One = GL_ONE,
    SourceColor = GL_SRC_COLOR,
    OneMinusSourceColor = GL_ONE_MINUS_SRC_COLOR,
    DestinationColor = GL_DST_COLOR,
    OneMinusDestinationColor = GL_ONE_MINUS_DST_COLOR,
    SourceAlpha = GL_SRC_ALPHA,
    OneMinusSourceAlpha = GL_ONE_MINUS_SRC_ALPHA,
    DestinationAlpha = GL_DST_ALPHA,
    OneMinusDestinationAlpha = GL_ONE_MINUS_DST_ALPHA,
    ConstantColor = GL_CONSTANT_COLOR,
    OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
    ConstantAlpha = GL_CONSTANT_ALPHA,
    OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
    SourceAlphaSaturate = GL_SRC_ALPHA_SATURATE,
    Source1Color = GL_SRC1_COLOR,
    OneMinusSource1Color = GL_ONE_MINUS_SRC1_COLOR,
    Source1Alpha = GL_SRC1_ALPHA,
    OneMinusSource1Alpha = GL_ONE_MINUS_SRC1_ALPHA,
};
enum class CullState
{
    On = true,
    Off = false
};
enum class DepthState
{
    On = true,
    Off = false
};
enum class DepthFunc : GLenum
{
    Never = GL_NEVER,
    Equal = GL_EQUAL,
    NotEqual = GL_NOTEQUAL,
    Always = GL_ALWAYS,
    Greater = GL_GREATER,
    GreaterEqual = GL_GEQUAL,
    Less = GL_LESS,
    LessEqual = GL_LEQUAL,
};
enum class FrontFace : GLenum
{
    Ccw = GL_CCW,
    Cw = GL_CW
};

enum class CullFace : GLenum
{
    Front = GL_FRONT,
    Back = GL_BACK,
    FrontAndBack = GL_FRONT_AND_BACK,
};

enum class PolygonMode : GLenum
{
    Line = GL_LINE,
    Fill = GL_FILL,
};
struct RenderState
{
    CullState CullState;
    BlendState BlendState;
    DepthState DepthState;

    BlendFunc BlendFuncSource;
    BlendFunc BlendFuncDest;
    DepthFunc DepthFunc;

    Color ClearColor;
    FrontFace FrontFace;
    CullFace CullFace;
    PolygonMode PolyMode;
};
static const RenderState DefaultRenderState = {
    CullState::Off,
    BlendState::Off,
    DepthState::Off,

    BlendFunc::SourceAlpha,
    BlendFunc::OneMinusSourceAlpha,
    DepthFunc::Less,

    {0, 0, 0, 1},
    FrontFace::Ccw,
    CullFace::Back,
    PolygonMode::Fill};
static const RenderState DefaultTTFState = {
    CullState::Off,
    BlendState::On,
    DepthState::Off,

    BlendFunc::SourceAlpha,
    BlendFunc::OneMinusSourceAlpha,
    DepthFunc::Less,

    {0, 0, 0, 0},
    FrontFace::Ccw,
    CullFace::Back,
    PolygonMode::Fill};
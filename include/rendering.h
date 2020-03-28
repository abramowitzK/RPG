#pragma once
#include <glew.h>
#include <vector>
#include <cassert>
#include <renderState.h>
#include <shader.h>
#include <texture.h>
#include <map>
#include <memory>
#include <optional>
#include <stack>

struct Renderer
{
public:
	Renderer(int width, int height);
	Renderer(){};
	void clear_screen(bool depth, bool color);
	void handle_resize(int width, int height);
	void push_render_state(const RenderState &state);
	bool pop_render_state();

private:
	void apply_render_state(const RenderState &state);
	Shader *mDefaultShader;
	std::stack<RenderState> m_renderStates;
	int mWidth;
	int mHeight;
};

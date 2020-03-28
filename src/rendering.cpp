/*
* Created by Kyle on 12/11/2016.
* Copyright (c) 2016 Kyle All rights reserved.
*/
#include <rendering.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(int width, int height)
{
	m_renderStates.push(DefaultRenderState);

	mWidth = width;
	mHeight = height;
}

void Renderer::clear_screen(bool depth, bool color)
{
	if (depth)
	{
		if (color)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void Renderer::handle_resize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	glViewport(0, 0, mWidth, mHeight);
}

void Renderer::push_render_state(const RenderState &state)
{
	apply_render_state(state);
	m_renderStates.push(state);
}

bool Renderer::pop_render_state()
{
	if (!m_renderStates.empty())
	{
		m_renderStates.pop();
		apply_render_state(m_renderStates.top());
		return true;
	}
	else
	{
		return false;
	}
}

void Renderer::apply_render_state(const RenderState &state)
{
	state.CullState == CullState::On ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	state.DepthState == DepthState::On ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	state.BlendState == BlendState::On ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	glFrontFace((GLenum)state.FrontFace);
	glClearColor(state.ClearColor.r, state.ClearColor.g, state.ClearColor.b, state.ClearColor.a);
	glBlendFunc((GLenum)state.BlendFuncSource, (GLenum)state.BlendFuncDest);
	glDepthFunc((GLenum)state.DepthFunc);
	glCullFace((GLenum)state.CullFace);
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)state.PolyMode);
}
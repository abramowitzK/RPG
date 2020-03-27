#pragma once
#include <glew.h>
#include <vector>
#include <vectorMath.h>
#include <renderState.h>
#include <shader.h>
#include <structures.h>
#include <rendering.h>
#include <sprite.h>

struct RenderBatch
{
public:
	RenderBatch(GLuint offset, GLuint numVertices, Texture tex) : offset(offset), numVerts(numVertices),
																  texture(tex){};
	GLuint offset;
	GLuint numVerts;
	Texture texture;
};

struct Glyph
{
public:
	Glyph() {}

	Glyph(const Vector4 &destRect, const Vector4 &uvRect, float depth, Texture Texture, const Color &color)
	{
		texture = Texture;
		depth = depth;

		topLeft.color = color;
		topLeft.position = {destRect.x, destRect.y + destRect.w};
		topLeft.uvs = {uvRect.x, uvRect.y + uvRect.w};

		bottomLeft.color = color;
		bottomLeft.position = {destRect.x, destRect.y};
		bottomLeft.uvs = {uvRect.x, uvRect.y};

		bottomRight.color = color;
		bottomRight.position = {destRect.x + destRect.z, destRect.y};
		bottomRight.uvs = {uvRect.x + uvRect.z, uvRect.y};

		topRight.color = color;
		topRight.position = {destRect.x + destRect.z, destRect.y + destRect.w};
		topRight.uvs = {uvRect.x + uvRect.z, uvRect.y + uvRect.w};
	}

	Glyph::Glyph(const Vector4 &destRect, const Vector4 &uvRect, float depth, Texture Texture, const Color &color, float angle) : texture(Texture),
																																  depth(depth)
	{
		glm::vec2 halfDims(destRect.z / 2.0f, destRect.w / 2.0f);

		// Get points centered at origin
		glm::vec2 tl(-halfDims.x, halfDims.y);
		glm::vec2 bl(-halfDims.x, -halfDims.y);
		glm::vec2 br(halfDims.x, -halfDims.y);
		glm::vec2 tr(halfDims.x, halfDims.y);

		// Rotate the points
		tl = rotatePoint(tl, angle) + halfDims;
		bl = rotatePoint(bl, angle) + halfDims;
		br = rotatePoint(br, angle) + halfDims;
		tr = rotatePoint(tr, angle) + halfDims;

		topLeft.color = color;
		topLeft.position = {destRect.x + tl.x, destRect.y + tl.y};
		topLeft.uvs = {uvRect.x, uvRect.y + uvRect.w};

		bottomLeft.color = color;
		bottomLeft.position = {destRect.x + bl.x, destRect.y + bl.y};
		bottomLeft.uvs = {uvRect.x, uvRect.y};

		bottomRight.color = color;
		bottomRight.position = {destRect.x + br.x, destRect.y + br.y};
		bottomRight.uvs = {uvRect.x + uvRect.z, uvRect.y};

		topRight.color = color;
		topRight.position = {destRect.x + tr.x, destRect.y + tr.y};
		topRight.uvs = {uvRect.x + uvRect.z, uvRect.y + uvRect.w};
	}

	glm::vec2 rotatePoint(const glm::vec2 &pos, float angle)
	{
		glm::vec2 newv;
		newv.x = pos.x * cos(angle) - pos.y * sin(angle);
		newv.y = pos.x * sin(angle) + pos.y * cos(angle);
		return newv;
	}

	Texture texture;
	float depth;
	Vertex2D topLeft;
	Vertex2D bottomLeft;
	Vertex2D topRight;
	Vertex2D bottomRight;
};

struct SpriteBatch
{
public:
	SpriteBatch();

	~SpriteBatch();

	void init();

	void begin(GlyphSortType type = GlyphSortType::Texture);

	void end();

	void draw(const Vector4 &destRect, const Vector4 &uvRect, float depth, Texture texture, const Color &color);
	void draw(const Vector4 &destRect, const Vector4 &uvRect, float depth, Texture texture, const Color &color, float angle);
	void draw(Sprite s);

	void render_batches(Renderer *renderer);

	Shader *default_shader = nullptr;

private:
	void create_render_batches();

	void sort_glyphs();

	bool mIsInitialized = false;
	bool mIsBegun = false;
	GLuint mVbo = 0;
	GLuint mVao = 0;
	std::vector<Glyph> mGlyphData;
	std::vector<Glyph *> mGlyphs;
	std::vector<RenderBatch> mBatches;
	GlyphSortType mType;
	const RenderState sSpriteBatchState{
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
};

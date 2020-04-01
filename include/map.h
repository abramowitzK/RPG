#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tileSheet.h>
#include <comp/mapTile.h>
#include <vector>

class MapLayer final
{
public:
    MapLayer(const tmx::Map &map, std::size_t layerIdx, const std::vector<unsigned> &textures) : m_tilesetTextures(textures)
    {
        createSubsets(map, layerIdx);
    }

    ~MapLayer()
    {
        for (auto &ss : m_subsets)
        {
            if (ss.vbo)
            {
                glDeleteBuffers(1, &ss.vbo);
            }
            if (ss.lookup)
            {
                glDeleteTextures(1, &ss.lookup);
            }
            //don't delete the tileset textures as these are
            //shared and deleted elsewhere
        }
    }

    MapLayer(const MapLayer &) = delete;
    MapLayer &operator=(const MapLayer &) = delete;

    void draw()
    {

        for (const auto &ss : m_subsets)
        {
            glBindVertexArray(ss.vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ss.texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, ss.lookup);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        glBindVertexArray(0);
    }

private:
    const std::vector<unsigned> m_tilesetTextures;
    struct Subset final
    {
        unsigned vao = 0;
        unsigned vbo = 0;
        unsigned texture = 0;
        unsigned lookup = 0;
    };
    std::vector<Subset> m_subsets;

    void createSubsets(const tmx::Map &map, std::size_t layerIdx)
    {
        const auto &layers = map.getLayers();
        if (layerIdx >= layers.size() || (layers[layerIdx]->getType() != tmx::Layer::Type::Tile))
        {
            std::cout << "Invalid layer index or layer type, layer will be empty" << std::endl;
            return;
        }
        const auto layer = dynamic_cast<const tmx::TileLayer *>(layers[layerIdx].get());

        auto bounds = map.getBounds();
        float verts[] =
            {
                bounds.left, bounds.top, 0.f, 0.f, 0.f,
                bounds.left + bounds.width, bounds.top, 0.f, 1.f, 0.f,
                bounds.left, bounds.top + bounds.height, 0.f, 0.f, 1.f,
                bounds.left + bounds.width, bounds.top + bounds.height, 0.f, 1.f, 1.f};

        const auto &mapSize = map.getTileCount();
        const auto &tilesets = map.getTilesets();
        for (auto i = 0u; i < tilesets.size(); ++i)
        {
            //check each tile ID to see if it falls in the current tile set
            const auto &ts = tilesets[i];
            const auto &tileIDs = layer->getTiles();
            std::vector<std::uint16_t> pixelData;
            bool tsUsed = false;

            for (auto y = 0u; y < mapSize.y; ++y)
            {
                for (auto x = 0u; x < mapSize.x; ++x)
                {
                    auto idx = y * mapSize.x + x;
                    if (idx < tileIDs.size() && tileIDs[idx].ID >= ts.getFirstGID() && tileIDs[idx].ID < (ts.getFirstGID() + ts.getTileCount()))
                    {
                        pixelData.push_back(static_cast<std::uint16_t>((tileIDs[idx].ID - ts.getFirstGID()) + 1)); //red channel - making sure to index relative to the tileset
                        pixelData.push_back(static_cast<std::uint16_t>(tileIDs[idx].flipFlags));                   //green channel - tile flips are performed on the shader
                        tsUsed = true;
                    }
                    else
                    {
                        //pad with empty space
                        pixelData.push_back(0);
                        pixelData.push_back(0);
                    }
                }
            }

            //if we have some data for this tile set, create the resources
            if (tsUsed)
            {
                m_subsets.emplace_back();
                m_subsets.back().texture = m_tilesetTextures[i];
                glGenVertexArrays(1, &m_subsets.back().vao);
                glBindVertexArray(m_subsets.back().vao);
                glGenBuffers(1, &m_subsets.back().vbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_subsets.back().vbo);
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
                glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

                glGenTextures(1, &m_subsets.back().lookup);
                glBindTexture(GL_TEXTURE_2D, m_subsets.back().lookup);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, mapSize.x, mapSize.y, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, (void *)pixelData.data());

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
        }
    }
};

struct Map
{
    std::vector<std::unique_ptr<MapLayer>> Layers;
};

std::unique_ptr<Map> LoadMap(std::string const &path)
{
    tmx::Map map;
    map.load(path);
    auto gameMap = std::make_unique<Map>();
    const auto &tilesets = map.getTilesets();
    std::vector<Texture> textures;
    for (const auto &ts : tilesets)
    {
        textures.push_back(create_2d_texture(ts.getImagePath().c_str()));
    }

    std::vector<unsigned> textureIds;
    for (const auto &tex : textures)
    {
        textureIds.push_back(tex.Tex);
    }
    //create a drawable object for each tile layer
    const auto &layers = map.getLayers();
    for (auto i = 0u; i < layers.size(); ++i)
    {
        if (layers[i]->getType() == tmx::Layer::Type::Tile)
        {
            gameMap->Layers.emplace_back(std::make_unique<MapLayer>(map, i, textureIds));
        }
    }

    return gameMap;
}
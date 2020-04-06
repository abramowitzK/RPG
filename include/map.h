#pragma once
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tileSheet.h>
#include <comp/mapTile.h>
#include <vector>
#include <queue>

struct Path
{
    static const u32 MaxPath = 64;
    MapTile Tiles[MaxPath]; // Tiles contained in path
    u32 Index = 0;          // Num tiles -1
};

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
    std::vector<MapTile> m_tiles;

    bool FindPath(Vector2 Start, Vector2 End, Path *out)
    {
        MapTile StartTile;
        MapTile EndTile;
        if (!GetTileAtPoint(Start, &StartTile) || !GetTileAtPoint(End, &EndTile))
        {
            return false;
        }
        // if (EndTile.Flags != MapTileFlags::Walkable)
        // {
        //     return false;
        // }

        std::queue<MapTile> Queue;
        static std::vector<bool> Visited;
        static std::vector<i32> Pred;
        static std::vector<u32> Dist;
        Visited.resize(m_tiles.size());
        Pred.resize(m_tiles.size());
        Dist.resize(m_tiles.size());

        // TODO optimize this
        for (int i = 0; i < m_tiles.size(); i++)
        {
            Visited[i] = false;
            Dist[i] = INT_MAX;
            Pred[i] = -1;
        }

        //Push on the starting tile
        Queue.push({StartTile});
        Visited[StartTile.Index] = true;
        Dist[StartTile.Index] = 0;
        while (!Queue.empty())
        {
            auto element = Queue.front();
            auto index = element.Index;
            Queue.pop();

            // Get element off queue and check all child nodes
            if (index == EndTile.Index)
            {
                if (Dist[EndTile.Index] >= Path::MaxPath)
                {
                    return false;
                }
                // we found a path to the end!
                // vector path stores the shortest pat
                u32 Length = Dist[EndTile.Index];
                i32 crawl = EndTile.Index;
                i32 Index = Length;
                out->Tiles[Index] = EndTile;
                out->Index = Index;
                while (Pred[crawl] != -1)
                {
                    out->Tiles[--Index] = m_tiles[Pred[crawl]];
                    crawl = Pred[crawl];
                }

                return true;
            }

            // what to add to our index to get our neighbors in 2d space
            const i32 Neighbors[] = {
                -100, //Top
                100,  //Bottom
                -1,   //Left
                1,    //Right
                -99,  //TopRight
                99,   //BottomLeft
                -101, //TopLeft
                101,  //BottomRight
            };

            for (int i = 0; i < ARRAY_SIZE(Neighbors); ++i)
            {
                // Index of this neighbor
                i32 LocalIndex = index + Neighbors[i];
                // If this index doesn't exist in this layer, ignore it
                if (LocalIndex >= m_tiles.size() || LocalIndex < 0)
                {
                    continue;
                }

                //Don't visit again if already visited
                if (Visited[LocalIndex])
                {
                    continue;
                }

                if (m_tiles[LocalIndex].Flags != MapTileFlags::Walkable)
                {
                    Visited[LocalIndex] = true;
                    continue;
                }

                Pred[LocalIndex] = element.Index;
                Dist[LocalIndex] = Dist[element.Index] + 1;
                Visited[LocalIndex] = true;
                Queue.push(m_tiles[LocalIndex]);
            }
        }

        return false;
    }

    bool GetTileAtPoint(Vector2 Point, MapTile *out)
    {
        for (u32 i = 0u; i < m_tiles.size(); ++i)
        {
            if (IsPointInRect(m_tiles[i].Rect, Point))
            {
                *out = m_tiles[i];
                return true;
            }
        }

        return false;
    }

    void HighlightTile(i32 idx)
    {
        m_subsets.back().pixelData[idx] = 0;
        glBindTexture(GL_TEXTURE_2D, m_subsets.back().lookup);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, m_tileWidth, m_tileHeight, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, (void *)m_subsets.back().pixelData.data());
    }

    void RestoreOldPixelData()
    {
        m_subsets.back().pixelData = m_subsets.back().OldPixelData;
    }

private:
    const std::vector<unsigned> m_tilesetTextures;
    int m_width;
    int m_height;
    int m_tileWidth;
    int m_tileHeight;

    struct Subset final
    {
        unsigned vao = 0;
        unsigned vbo = 0;
        unsigned texture = 0;
        unsigned lookup = 0;
        std::vector<std::uint16_t> pixelData;
        std::vector<u16> OldPixelData;
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
        float verts[] = {
            bounds.left, bounds.top, 0.f, 0.f, 0.f,
            bounds.left + bounds.width, bounds.top, 0.f, 1.f, 0.f,
            bounds.left, bounds.top + bounds.height, 0.f, 0.f, 1.f,
            bounds.left + bounds.width, bounds.top + bounds.height, 0.f, 1.f, 1.f};

        const auto &mapSize = map.getTileCount();
        const auto &tilesets = map.getTilesets();
        m_tileWidth = mapSize.x;
        m_tileHeight = mapSize.y;
        m_width = m_tileWidth * map.getTileSize().x;
        m_height = m_tileHeight * map.getTileSize().y;
        for (auto i = 0u; i < tilesets.size(); ++i)
        {
            //check each tile ID to see if it falls in the current tile set
            const auto &ts = tilesets[i];
            const auto &tileIDs = layer->getTiles();
            std::vector<std::uint16_t> pixelData;
            bool tsUsed = false;
            const auto &tiles = ts.getTiles();
            for (auto y = 0u; y < mapSize.y; ++y)
            {
                for (auto x = 0u; x < mapSize.x; ++x)
                {
                    auto idx = y * mapSize.x + x;
                    auto id = tileIDs[idx].ID;
                    auto tile = ts.getTile(id);
                    MapTile mapTile;
                    if (tile)
                    {
                        auto const &props = tile->properties;
                        if (std::find_if(props.begin(), props.end(), [](const auto &property) {
                                return (property.getName() == "Walkable") && property.getBoolValue();
                            }) != props.end())
                        {
                            mapTile.Flags = MapTileFlags::Walkable;
                        }
                        else
                        {
                            mapTile.Flags = MapTileFlags::None;
                        }
                    }
                    else
                    {
                        mapTile.Flags = MapTileFlags::Walkable;
                    }
                    mapTile.Rect = {
                        (float)map.getTileSize().x,
                        (float)map.getTileSize().y,
                        (float)x * map.getTileSize().x,
                        (float)y * map.getTileSize().y};
                    mapTile.Index = idx;
                    m_tiles.push_back(mapTile);
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
                m_subsets.back().pixelData = pixelData;
                m_subsets.back().OldPixelData = pixelData;
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
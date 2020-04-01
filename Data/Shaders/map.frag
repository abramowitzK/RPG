    #version 130
    #define FLIP_HORIZONTAL 8u
    #define FLIP_VERTICAL 4u
    #define FLIP_DIAGONAL 2u
    in vec2 v_texCoord;
    uniform usampler2D u_lookupMap;
    uniform sampler2D u_tileMap;
    uniform vec2 u_tileSize = vec2(32.0);
    uniform vec2 u_tilesetCount = vec2(16, 20);
    uniform vec2 u_tilesetScale = vec2(2.0);
    uniform float u_opacity = 1.0;
    out vec4 colour;

    void main()
    {
        uvec2 values = texture(u_lookupMap, v_texCoord).rg;
        if(values.r > 0u)
        {
            float index = float(values.r) - 1.0;
            vec2 position = vec2(mod(index, u_tilesetCount.x), floor((index / u_tilesetCount.x))) / u_tilesetCount;
            
            vec2 texelSize = vec2(1.0) / textureSize(u_lookupMap, 0);
            vec2 offset = mod(v_texCoord, texelSize);
            vec2 ratio = offset / texelSize;
            offset = ratio * (1.0 / u_tileSize);
            offset *= u_tileSize / u_tilesetCount;
            colour = texture(u_tileMap, position + offset);
            colour.a = min(colour.a, u_opacity);
        }
        else
        {
             colour = vec4(0.0);
        }
    }
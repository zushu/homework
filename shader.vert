#version 120
//version 410

// input at each vertex
//layout(location = 0) in vec3 pos;
attribute vec3 pos;
// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;
uniform mat4 normal_view_mat;

// Texture-related data
uniform sampler2D rgbTexture;
//layout(binding = 1) uniform sampler2D height_texture;
uniform int widthTexture;
uniform int heightTexture;



// Output to Fragment Shader
varying vec2 textureCoordinate; // For texture-color
varying vec3 vertexNormal; // For Lighting computation
varying vec3 ToLightVector; // Vector from Vertex to Light;
varying vec3 ToCameraVector; // Vector from Vertex to Camera;

vec3 light_pos = vec3(widthTexture/2, 100, heightTexture/2);
vec3 intensity = vec3(1, 1, 1);


float calculate_y_coord(vec3 vertex);
vec3 calculate_normal(vec3 vertex);

void main()
{
    vec3 final_pos = pos;
    // + 1 because the tips of the tex image are (0, 0, 0), (w, 0, h), there are w + 1 vertices
    // 1 - because we triangularized the texture from bottom left to up rightm but the texture is indexed from up left to down right.
    float tex_i = 1 - (float(pos.x) / (widthTexture+1)); 
    float tex_j = 1 - (float(pos.z) / (heightTexture+1));
    textureCoordinate = vec2(tex_i, tex_j);
    // get texture value, compute height
    //texture — retrieves texels from a texture
    // gvec4 texture( 	gsampler2D sampler, vec2 P, [float bias]);
    // output: normalised RGBA
    vec4 tex_color = texture2D(rgbTexture, textureCoordinate); 
    float red = tex_color.x;
    final_pos.y = heightFactor * red;
    // compute normal vector using also the heights of neighbor vertices
    //vec3* neighbours = get_neighbours(final_pos);
    vec3 final_normal = calculate_normal(final_pos);

    // compute toLight vector vertex coordinate in VCS
    ToLightVector = normalize(vec3(MV*vec4(light_pos - final_pos, 0)));
    ToCameraVector = normalize(vec3(MV*(vec4(vec3(cameraPosition) - final_pos, 0))));
    vertexNormal = normalize(vec3(normal_view_mat*vec4(final_normal,0)));

    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP*vec4(final_pos,1); // this is a placeholder. It does not correctly set the position
    
}

float calculate_y_coord(vec3 vertex)
{
    vec2 tex_coord;
    tex_coord.x = abs(vertex.x-widthTexture)/widthTexture;
    tex_coord.y = abs(vertex.z-heightTexture)/heightTexture;
    vec4 tex_color = texture2D(rgbTexture, tex_coord);
    float y = heightFactor * tex_color.x;
    return y;
}

vec3 calculate_normal(vec3 vertex)
{
    vec3 neighbours[6];
    vec3 normal;
    // bottom left
    if (vertex.x == 0 && vertex.z == 0)
    {
        //east
        neighbours[0] = vec3(vertex.x + 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        // north
        neighbours[1] = vec3(vertex.x, 0, vertex.z + 1);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // north-east neighbour
        neighbours[2] = vec3(vertex.x + 1, 0, vertex.z + 1);
        neighbours[2].y = calculate_y_coord(neighbours[2]);

        vec3 v1 = neighbours[0] - vertex; // directed to right
        vec3 v2 = neighbours[1] - vertex; // directed up
        vec3 v3 = neighbours[2] - vertex; // directed north east

        vec3 normal1 = cross(v3, v2);
        vec3 normal2 = cross(v1, v3);

        normal = normalize(normal1 + normal2);

    }
    // bottom right
    else if (vertex.x == widthTexture - 1 && vertex.z == 0)
    {
        //west
        neighbours[0] = vec3(vertex.x - 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        // north
        neighbours[1] = vec3(vertex.x, 0, vertex.z + 1);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // north-west neighbour
        //neighbours[2] = vec3(vertex.x - 1, 0, vertex.z + 1);
        
        vec3 v1 = neighbours[0] - vertex; // directed to left
        vec3 v2 = neighbours[1] - vertex; // directed up
        //vec3 v3 = neighbours[2] - vertex; // directed north east

        normal = normalize(cross(v2, v1));
        //vec3 normal2 = cross(v1, v3);

    }
    // top left
    else if (vertex.x == 0 && vertex.z == heightTexture - 1)
    {
        //east
        neighbours[0] = vec3(vertex.x + 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        // south
        neighbours[1] = vec3(vertex.x, 0, vertex.z - 1);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // south-east neighbour
        //neighbours[2] = vec3(vertex.x + 1, 0, vertex.z - 1);
        vec3 v1 = neighbours[0] - vertex; // directed to right
        vec3 v2 = neighbours[1] - vertex; // directed down
        //vec3 v3 = neighbours[2] - vertex; // directed north east

        normal = normalize(cross(v2, v1));
        //vec3 normal2 = cross(v1, v3);

    }
    // top right
    else if (vertex.x == widthTexture - 1 && vertex.z == heightTexture - 1)
    {
        //west
        neighbours[0] = vec3(vertex.x - 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        // south
        neighbours[1] = vec3(vertex.x, 0, vertex.z - 1);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // south-west neighbour
        neighbours[2] = vec3(vertex.x - 1, 0, vertex.z - 1);
        neighbours[2].y = calculate_y_coord(neighbours[2]);

        vec3 v1 = neighbours[0] - vertex; // directed to left
        vec3 v2 = neighbours[1] - vertex; // directed down
        vec3 v3 = neighbours[2] - vertex; // directed south-west

        vec3 normal1 = cross(v3, v2);
        vec3 normal2 = cross(v1, v3);

        normal = normalize(normal1 + normal2);
    }
    // bottom row
    else if (vertex.z == 0)
    {
        //west
        neighbours[0] = vec3(vertex.x - 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        //east
        neighbours[1] = vec3(vertex.x + 1, 0, vertex.z);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // north
        neighbours[2] = vec3(vertex.x, 0, vertex.z + 1);
        neighbours[2].y = calculate_y_coord(neighbours[2]);
        // north-east neighbour
        neighbours[3] = vec3(vertex.x + 1, 0, vertex.z + 1);
        neighbours[3].y = calculate_y_coord(neighbours[3]);

        vec3 v1 = neighbours[0] - vertex; // directed to left
        vec3 v2 = neighbours[1] - vertex; // directed to right
        vec3 v3 = neighbours[2] - vertex; // directed up
        vec3 v4 = neighbours[3] - vertex; // directed north-east

        vec3 normal1 = cross(v3, v1);
        vec3 normal2 = cross(v4, v3);
        vec3 normal3 = cross(v2, v4);

        normal = normalize(normal1 + normal2 + normal3);
    }
    // top row 
    else if (vertex.z == heightTexture - 1)
    {
        //west
        neighbours[0] = vec3(vertex.x - 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        //east
        neighbours[1] = vec3(vertex.x + 1, 0, vertex.z);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // south
        neighbours[2] = vec3(vertex.x, 0, vertex.z - 1);
        neighbours[2].y = calculate_y_coord(neighbours[2]);
        // south-west neighbour
        neighbours[3] = vec3(vertex.x - 1, 0, vertex.z - 1);
        neighbours[3].y = calculate_y_coord(neighbours[3]);

        vec3 v1 = neighbours[0] - vertex; // directed to left
        vec3 v2 = neighbours[1] - vertex; // directed to right
        vec3 v3 = neighbours[2] - vertex; // directed down
        vec3 v4 = neighbours[3] - vertex; // directed south-west

        vec3 normal1 = cross(v1, v4);
        vec3 normal2 = cross(v4, v3);
        vec3 normal3 = cross(v3, v2);

        normal = normalize(normal1 + normal2 + normal3);
    }
    // left column
    else if (vertex.x == 0)
    {
        //east
        neighbours[0] = vec3(vertex.x + 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        //north
        neighbours[1] = vec3(vertex.x, 0, vertex.z + 1);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // south
        neighbours[2] = vec3(vertex.x, 0, vertex.z - 1);
        neighbours[2].y = calculate_y_coord(neighbours[2]);
        // north-east neighbour
        neighbours[3] = vec3(vertex.x + 1, 0, vertex.z + 1);
        neighbours[3].y = calculate_y_coord(neighbours[3]);

        vec3 v1 = neighbours[0] - vertex; // directed to right
        vec3 v2 = neighbours[1] - vertex; // directed up
        vec3 v3 = neighbours[2] - vertex; // directed down
        vec3 v4 = neighbours[3] - vertex; // directed north-east

        vec3 normal1 = cross(v4, v2);
        vec3 normal2 = cross(v1, v4);
        vec3 normal3 = cross(v3, v1);

        normal = normalize(normal1 + normal2 + normal3);

    }
    // right column
    else if (vertex.x == widthTexture - 1)
    {
        //west
        neighbours[0] = vec3(vertex.x - 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        //north
        neighbours[1] = vec3(vertex.x, 0, vertex.z + 1);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        // south
        neighbours[2] = vec3(vertex.x, 0, vertex.z - 1);
        neighbours[2].y = calculate_y_coord(neighbours[2]);
        // south-west neighbour
        neighbours[3] = vec3(vertex.x - 1, 0, vertex.z - 1);
        neighbours[3].y = calculate_y_coord(neighbours[3]);


        vec3 v1 = neighbours[0] - vertex; // directed to left
        vec3 v2 = neighbours[1] - vertex; // directed up
        vec3 v3 = neighbours[2] - vertex; // directed down
        vec3 v4 = neighbours[3] - vertex; // directed south-west

        vec3 normal1 = cross(v2, v1);
        vec3 normal2 = cross(v1, v4);
        vec3 normal3 = cross(v4, v3);

        normal = normalize(normal1 + normal2 + normal3);
    }
    // middle
    else
    {
        //west
        neighbours[0] = vec3(vertex.x - 1, 0, vertex.z);
        neighbours[0].y = calculate_y_coord(neighbours[0]);
        // east 
        neighbours[1] = vec3(vertex.x + 1, 0, vertex.z);
        neighbours[1].y = calculate_y_coord(neighbours[1]);
        //north
        neighbours[2] = vec3(vertex.x, 0, vertex.z + 1);
        neighbours[2].y = calculate_y_coord(neighbours[2]);
        // south
        neighbours[3] = vec3(vertex.x, 0, vertex.z - 1);
        neighbours[3].y = calculate_y_coord(neighbours[3]);
        // south-west neighbour
        neighbours[4] = vec3(vertex.x - 1, 0, vertex.z - 1);
        neighbours[4].y = calculate_y_coord(neighbours[4]);
        // north-east
        neighbours[5] = vec3(vertex.x + 1, 0, vertex.z + 1);
        neighbours[5].y = calculate_y_coord(neighbours[5]);

        vec3 v1 = neighbours[0] - vertex; // directed to left
        vec3 v2 = neighbours[1] - vertex; // directed right
        vec3 v3 = neighbours[2] - vertex; // directed up
        vec3 v4 = neighbours[3] - vertex; // directed down
        vec3 v5 = neighbours[4] - vertex; // directed south-west
        vec3 v6 = neighbours[5] - vertex; // directed north-east

        vec3 normal1 = cross(v1, v5);
        vec3 normal2 = cross(v5, v4);
        vec3 normal3 = cross(v4, v2);
        vec3 normal4 = cross(v2, v6);
        vec3 normal5 = cross(v6, v3);
        vec3 normal6 = cross(v3, v1);

        normal = normalize(normal1 + normal2 + normal3 + normal4 + normal5 + normal6);
    }

    return normal;
}

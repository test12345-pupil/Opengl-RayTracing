#version 420 core
layout (location = 0) in vec2 Pos;
layout (location = 1) in vec2 texCoords;
layout (binding = 0) uniform sampler2D imgTexture;
layout (binding = 1) uniform samplerBuffer BVHTree;
out vec3 Color;
uniform vec2 u_position;
uniform vec3 u_color;
uniform float u_width, u_height;
vec2 _pos;
void main(){
    Color = u_color;
    _pos = Pos + u_position;
    gl_Position = vec4(2.0*_pos.x/u_height - 1, 2.0*_pos.y/u_width - 1, 1.0, 1.0);
}
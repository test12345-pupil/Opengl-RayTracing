#version 420 core
layout (location = 0) in vec2 in_Pos;
out vec2 Pos;
uniform ivec2 u_position;
uniform int u_width, u_height;
vec2 _pos;
void main(){
    _pos = in_Pos + u_position;
    gl_Position = vec4(2.0*_pos.x/u_height - 1, 2.0*_pos.y/u_width - 1, 1.0, 1.0);
    Pos = gl_Position.xy;
}



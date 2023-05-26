#version 420 core
layout (location = 0) in vec2 in_Pos;
out vec2 Pos;
void main(){
    gl_Position = vec4(in_Pos, 1.0, 1.0);
    Pos = in_Pos;
}

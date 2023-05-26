#version 420 core

in vec2 Pos;
out vec4 fragColor;

layout (binding = 0) uniform sampler2D texPass0;
layout (binding = 1) uniform sampler2D texPass1;
layout (binding = 2) uniform sampler2D texPass2;
layout (binding = 3) uniform sampler2D texPass3;
layout (binding = 4) uniform sampler2D texPass4;
layout (binding = 5) uniform sampler2D texPass5;
layout (binding = 6) uniform sampler2D texPass6;

void main() {
    fragColor = texture(texPass4, (Pos * 0.5 + vec2(0.5))) * 30.0;
}
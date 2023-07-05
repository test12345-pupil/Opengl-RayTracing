#version 420 core

in vec2 Pos;
out vec4 fragColor;
uniform int u_width, u_height;

layout (binding = 0) uniform sampler2D texPass0;
layout (binding = 1) uniform sampler2D texPass1;
layout (binding = 2) uniform sampler2D texPass2;
layout (binding = 3) uniform sampler2D texPass3;
layout (binding = 4) uniform sampler2D texPass4;
layout (binding = 5) uniform sampler2D texPass5;
layout (binding = 6) uniform sampler2D texPass6;
const float exposure = 1;
const int D = 5;
void main() {
    vec4 sum = vec4(0);
    for(int i = -D; i <= D; ++i)
        for(int j = -D; j <= D; ++j)
            sum += texture(texPass5, vec2(Pos.x*0.5+0.5+2*i/u_height, Pos.y*0.5+0.5+2*j/u_width));
    
    sum /= (2*D+1)*(2*D+1);

    const float gamma = 5;

    vec3 mapped = vec3(1.0) - exp(-sum.xyz * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));

    fragColor = vec4(mapped, 1.0);
}
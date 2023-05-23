#version 420 core
in vec2 Pos;
out vec4 FragColor;
layout (binding = 0) uniform sampler2D imgTexture;
layout (binding = 1) uniform samplerBuffer BVHnodes;
uniform int u_size_BVHnode;
layout (binding = 2) uniform samplerBuffer Triangles;
uniform int u_size_Triangle;
uniform int u_width, u_height;
uniform int u_clearScreen;
uniform int u_nsamples;
uniform vec3 u_campos, u_camdir, u_camaxisX, u_camaxisY;
#define INF 1e18

struct Material{ 
    vec3 Color; // 表面颜色
    vec3 normal; // 法向量，应该始终normalized

    float reflectRate; // 反射光占比
    float reflectRough; // 粗糙程度
    float refractRate; // 折射光占比

    float refractAngle; // 折射率
    float refractRough; // 折射粗糙度
    bool isLighter; // 是否为光源, 0/1
};
struct Ray{
    vec3 start, direction;
};
struct AABB{
    vec3 d, u;
};
struct BVHnode{
    // 12 bytes
    int ls, rs, triangleID; // 若triangleID为-1则非叶子，否则为叶子
    // 24 bytes
    AABB AABBbox;// d, u
};
struct Triangle{
    Material material;
    vec3 a,b,c;
    vec2 ta,tb,tc;
};

BVHnode getIthNode(int index){
    index -= 1;
    vec3 data1 = texelFetch(BVHnodes, index * u_size_BVHnode).xyz;
    vec3 data2 = texelFetch(BVHnodes, index * u_size_BVHnode + 1).xyz;
    vec3 data3 = texelFetch(BVHnodes, index * u_size_BVHnode + 2).xyz;
    return BVHnode(
        floatBitsToInt(data1.x),
        floatBitsToInt(data1.y),
        floatBitsToInt(data1.z),
        AABB(data2, data3)
    );
}
Triangle getIthTriangle(int index){
    vec3 data0 = texelFetch(BVHnodes, index * u_size_BVHnode).xyz; // saves virtual table, dont use
    vec3 data1 = texelFetch(BVHnodes, index * u_size_BVHnode + 1).xyz;
    vec3 data2 = texelFetch(BVHnodes, index * u_size_BVHnode + 2).xyz;
    vec3 data3 = texelFetch(BVHnodes, index * u_size_BVHnode + 3).xyz;
    vec3 data4 = texelFetch(BVHnodes, index * u_size_BVHnode + 4).xyz;
    vec3 data5 = texelFetch(BVHnodes, index * u_size_BVHnode + 5).xyz;
    vec3 data6 = texelFetch(BVHnodes, index * u_size_BVHnode + 6).xyz;
    vec3 data7 = texelFetch(BVHnodes, index * u_size_BVHnode + 7).xyz;
    vec3 data8 = texelFetch(BVHnodes, index * u_size_BVHnode + 8).xyz;
    vec3 data9 = texelFetch(BVHnodes, index * u_size_BVHnode + 9).xyz;
    return Triangle(
        Material(
            data1,data2,
            data3.x,data3.y,data3.z,
            data4.x,data4.y,bool(floatBitsToInt(data4.z))),
        data5,data6,data7,
        data8.xy,
        vec2(data8.z, data9.x),
        data9.yz);
}

struct HitResult{
    float dist; // 与交点距离，若为nan则无效
    vec3 hitPoint; // 光线命中点
    Material material; // 命中点的材质
};

const HitResult InvalidHit = HitResult(INF, vec3(0), Material(vec3(0),vec3(0),0,0,0,0,0,bool(0)));


HitResult getHitResultRayTriangle(Ray r, Triangle t){
    vec3 D = r.direction, N = t.material.normal;
    if(dot(D,N) > 0.0f) N = -N;
    float coef = dot(t.a - r.start, N) / dot(D, N);
    if(coef < 1e-4) return InvalidHit;
    vec3 X = r.start + D * coef;
    vec3 c1 = cross(t.b-t.a, X-t.a);
    vec3 c2 = cross(t.c-t.b, X-t.b);
    vec3 c3 = cross(t.a-t.c, X-t.c);
    if(dot(c1, t.material.normal)<0 || dot(c2, t.material.normal)<0 || dot(c3, t.material.normal)<0) return InvalidHit;
    float d = distance(X, r.start);
    HitResult res = HitResult(d, X, t.material);
    res.material.normal = N;
    if(!isnan(t.ta.x)){
        res.material.Color = 0.75f * (res.hitPoint + vec3(1)) ;
    }
    return res;
}

void testInsectAABBRay(out float t0, out float t1, AABB box, Ray r) {
    vec3 idir = vec3(1.0 / r.direction.x, 1.0 / r.direction.y, 1.0 / r.direction.z);

    vec3 _in = (box.d - r.start) * idir;
    vec3 _out = (box.u - r.start) * idir;

    vec3 tmax = max(_in, _out);
    vec3 tmin = min(_in, _out);

    t1 = min(tmax.x, min(tmax.y, tmax.z));
    t0 = max(tmin.x, max(tmin.y, tmin.z));
}

int stack[256];
HitResult getBVHHitResult(int x, Ray r){
    int top = 0;
    stack[0] = x;
    HitResult ans = InvalidHit;
    while(top >= 0){
        BVHnode tx = getIthNode(stack[top]); --top;
        if(tx.triangleID >= 0){
            HitResult res = getHitResultRayTriangle(r, getIthTriangle(tx.triangleID));
            if(res.dist < ans.dist){
                ans = res;
            }
        }else{
            float ls_t0, ls_t1, rs_t0, rs_t1;
            testInsectAABBRay(ls_t0, ls_t1, getIthNode(stack[tx.ls]).AABBbox, r);
            testInsectAABBRay(rs_t0, rs_t1, getIthNode(stack[tx.rs]).AABBbox, r);
            if(ls_t0 < rs_t0){
                if(rs_t1 >= 0 && rs_t0 <= rs_t1 && rs_t0 <= ans.dist) stack[++top] = tx.rs;
                if(ls_t1 >= 0 && ls_t0 <= ls_t1 && ls_t0 <= ans.dist) stack[++top] = tx.ls;
            }else{
                if(ls_t1 >= 0 && ls_t0 <= ls_t1 && ls_t0 <= ans.dist) stack[++top] = tx.ls;
                if(rs_t1 >= 0 && rs_t0 <= rs_t1 && rs_t0 <= ans.dist) stack[++top] = tx.rs;
            }
        }
    }
    return ans;
}

// https://blog.demofox.org/2020/05/25/casual-shadertoy-path-tracing-1-basic-camera-diffuse-emissive/
uniform uint frameCounter;

uint seed = uint(
    uint((Pos.x * 0.5 + 0.5) * u_width)  * uint(1973) + 
    uint((Pos.y * 0.5 + 0.5) * u_height) * uint(9277) + 
    uint(frameCounter) * uint(26699)) | uint(1);

uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}
 
float randf() {
    return float(wang_hash(seed)) / 4294967296.0;
}
float randf(float l, float r) {
    return randf() * (r-l) + l;
}
vec3 randomDirection(vec3 norm){
    vec3 d;
    do{
        d = vec3(randf(-1,1),randf(-1,1),randf(-1,1));
    }while(dot(d,d) > 1.0);
    return normalize(norm + normalize(d));
}

vec3 RayTrace(Ray r){
    const float RAYTRACE_DIE_PROB = 0.2;
    vec3 Col = vec3(1);
    for(int depth = 0; depth < 10; ++depth){
        HitResult result = getBVHHitResult(1, r);
        if(result.dist == INF) return vec3(0);
        if(result.material.isLighter) return Col * result.material.Color;
        if(randf() < RAYTRACE_DIE_PROB) return vec3(0);
        float _r = randf(), C = abs(dot(-r.direction, result.material.normal)) / (1-RAYTRACE_DIE_PROB);
        vec3 dir = randomDirection(result.material.normal);
        if(_r < result.material.reflectRate){ // 镜面反射
            r = Ray(result.hitPoint, 
                mix(reflect(-1.0f * r.direction, result.material.normal) , dir, result.material.reflectRough));
            Col *= C;
        }else if(_r < result.material.reflectRate + result.material.refractRate){ // 折射
            r = Ray(result.hitPoint, 
                mix(refract(-1.0f * r.direction, result.material.normal, result.material.refractRate) , dir, result.material.reflectRough));
            Col *= C;
        }else{ // 漫反射
            r = Ray(result.hitPoint, dir);
            Col *= result.material.Color * C;
        }
    }
    return vec3(0);
}

void main(){
    // Pos.x, Pos.y \in [-1,1]
    Ray startRay = Ray(u_campos, normalize(u_camdir - u_camaxisY * Pos.x - u_camaxisX * Pos.y));
    FragColor = vec4(RayTrace(startRay), 1.0);
}
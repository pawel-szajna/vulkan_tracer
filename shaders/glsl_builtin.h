#pragma once

#include <io_types.hpp>
#include <constants.glsl>

struct vec2
{
    vec2(float x, float y);
    float x, y,
          xx, xy,
          yx, yy;
};

struct vec4 : vec3
{
    vec4(float x, float y, float z, float w);
    vec4(vec3 v, float w);
    float w, xyz;
};

vec2 operator+(vec2, vec2);
vec3 operator+(vec3, vec3);
vec4 operator+(vec4, vec4);

vec2 operator-(vec2, vec2);
vec3 operator-(vec3, vec3);
vec4 operator-(vec4, vec4);

vec2 operator*(vec2, vec2);
vec3 operator*(vec3, vec3);
vec4 operator*(vec4, vec4);

vec2 operator/(vec2, vec2);
vec3 operator/(vec3, vec3);
vec4 operator/(vec4, vec4);

vec2 operator*(float, vec2);
vec3 operator*(float, vec3);
vec4 operator*(float, vec4);

vec2 operator*(vec2, float);
vec3 operator*(vec3, float);
vec4 operator*(vec4, float);

vec2 operator-(vec2);
vec3 operator-(vec3);
vec4 operator-(vec4);

vec2 normalize(vec2);
vec3 normalize(vec3);
vec4 normalize(vec4);

float length(vec2);
float length(vec3);
float length(vec4);

float dot(vec2, vec2);
float dot(vec3, vec3);
float dot(vec4, vec4);
float exp(float);
float abs(float);
float sqrt(float);
float pow(float, float);
float min(float, float);
float max(float, float);
float log(float);

using uint = unsigned int;

InputData inputs;
struct OutputData { vec4* pixels; } results;

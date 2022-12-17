#version 450

layout(location = 0) in vec4 worldPos;
layout(location = 1) in vec4 worldPos2;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 glow;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 worldPosOut;

const float PI = 3.1415;

const vec3 sun = vec3(1.0, 0.7, 0.4);
const vec3 sun2 = vec3(1.0, 0.8, 0.5);

const vec3 moon = vec3(0.5, 0.5, 0.5);
const vec3 moon2 = vec3(0.5, 0.5, 0.5);

uniform layout (binding = 0, set = 0) SETT
{
    mat4 mCameraMatrix;
    vec4 mCameraEye;
    vec4 mSunMoonDir;
    vec4 mDaySkyColor;
    vec4 mNightSkyColor;
    vec4 mSun1Color;
    vec4 mSun2Color;
    vec4 mMoon1Color;
    vec4 mMoon2Color;
} settings;

// Noise function 
// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
    float fl = mod(p, 10.0);
    float fc = fract(p);
    return mix(rand(fl), rand(fl + 1.0), fc);
}

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

// end noise

vec3 sky(in vec3 lightDir, in vec3 rayDir)
{
    vec3 col = vec3(0.0);

    float t = settings.mCameraEye.w;
	
	float si = sin(t * PI);
	
	vec3 sky = mix(settings.mDaySkyColor.rgb, settings.mNightSkyColor.rgb, si);
	col = sky * (1.0 - 0.8 * rayDir.y) * 0.9;
			
	vec3 s = mix(settings.mSun1Color.rgb, settings.mMoon1Color.rgb, si);
	vec3 s2 = mix(settings.mSun2Color.rgb, settings.mMoon2Color.rgb, si);
	
	// Sun
	float sundot = clamp(dot(rayDir, lightDir), 0.0, 1.0);
	col += 0.25 * s * pow(sundot, 16.0);
	col += 0.75 * s2 * pow(sundot, 128.0);

    // Horizon/atmospheric perspective
	vec3 persp = mix(vec3(0.7, 0.75, 0.8), settings.mNightSkyColor.rgb, si);
	
    col = mix(col, persp, pow(1.0 - max(abs(rayDir.y), 0.0), 8.0));

    return col;
}

void main()
{
    vec3 skyColor = sky(normalize(settings.mSunMoonDir.xyz), normalize(settings.mCameraEye.xyz - worldPos2.xyz));

    skyColor += noise(worldPos.xyz * 1000.0) * 0.01;

    diffuse = vec4(skyColor, 1.0);
    glow = vec4(0.0);
    normal = vec4(inNormal, 1.0);
    worldPosOut = worldPos;
    worldPosOut.rgb *= 100.0;
}
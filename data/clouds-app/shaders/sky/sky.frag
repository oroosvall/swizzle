#version 450

layout(location = 0) in vec4 worldPos;
layout(location = 1) in vec4 worldPos2;

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 worldPosOut;

const float PI = 3.1415;

const vec3 daySky = vec3(0.3, 0.55, 0.8);
const vec3 nightSky = vec3(0.024422, 0.088654, 0.147314);

const vec3 sun = vec3(1.0, 0.7, 0.4);
const vec3 sun2 = vec3(1.0, 0.8, 0.5);

const vec3 moon = vec3(0.5, 0.5, 0.5);
const vec3 moon2 = vec3(0.5, 0.5, 0.5);

uniform layout (binding = 0, set = 0) CAM
{
    mat4 cam;
    vec4 eye;
} camera;

uniform layout (push_constant) PUSH
{
    vec4 eyeDir; // w is clamped time for the cycle period
    vec4 sunMoonDir;
    float totalTime; // total world time passed
} skyStuff;

const int MaxSteps = 32;

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
#define NUM_OCTAVES 2
float fbm(vec3 x) {
    float v = 0.0;
    float a = 0.5;
    vec3 shift = vec3(100);
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(x);
        x = x * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

// end noise

vec3 sky(in vec3 lightDir, in vec3 rayDir)
{
    vec3 col = vec3(0.0);

    float t = skyStuff.eyeDir.w;
	
	float si = sin(t * PI);
	
	vec3 sky = mix(daySky, nightSky, si);
	col = sky * (1.0 - 0.8 * rayDir.y) * 0.9;
			
	vec3 s = mix(sun, moon, si);
	vec3 s2 = mix(sun2, moon2, si);
	
	// Sun
	float sundot = clamp(dot(rayDir, lightDir), 0.0, 1.0);
	col += 0.25 * s * pow(sundot, 16.0);
	col += 0.75 * s2 * pow(sundot, 128.0);

    // Horizon/atmospheric perspective
	vec3 persp = mix(vec3(0.7, 0.75, 0.8), nightSky, si);
	
    col = mix(col, persp, pow(1.0 - max(abs(rayDir.y), 0.0), 8.0));

    return col;
}

float doCloud(vec3 eye, vec3 world, float time)
{
    float accumdist = 0;

    vec3 localcamvec = normalize( world - eye );

    vec3 camPos = world * 10.0;
    camPos.y += (camera.eye.w * 10.0);

    float StepSize = 1.0 / MaxSteps;
    if (world.y > 0.0)
    {
        for (int i = 0; i < MaxSteps; i++)
        {
            float cursample = smoothstep(0.3, 1.0, fbm( world.xyz / world.y * 2.0 + time * 0.05)) * 0.1;
            accumdist += cursample * StepSize;
            camPos += localcamvec * StepSize;
        }
        return accumdist * smoothstep(0.0, 0.3, clamp(0.0, 1.0, world.y));
    }

    return 0.0;
}

void main()
{
    const float cirrus = 1.0;
    const float cumulus = 0.6; 
    
    float time = skyStuff.eyeDir.w * 100.0;

    vec3 skyColor = sky(normalize(skyStuff.sunMoonDir.xyz), normalize(camera.eye.xyz - worldPos2.xyz));

    if (worldPos.y > 0.0)
    {
        float factor = doCloud(vec3(0.0), worldPos.xyz, skyStuff.totalTime);
        skyColor.rgb = mix(skyColor, vec3(1.0), factor);
    }

    skyColor += noise(worldPos.xyz * 1000.0) * 0.01;

    diffuse = vec4(skyColor, 1.0);
    normal = -normalize(worldPos2);
    worldPosOut = worldPos;
    worldPosOut.rgb *= 100.0;
}
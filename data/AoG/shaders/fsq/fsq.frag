#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

layout (constant_id = 0) const int SSAO_KERNEL_SIZE = 64;
layout (constant_id = 1) const float SSAO_RADIUS = 0.5;

layout(set=0, binding=0) uniform sampler2D gui;
layout(set=0, binding=1) uniform sampler2D scene;
layout(set=0, binding=2) uniform sampler2D depth;
layout(set=0, binding=3) uniform sampler2D glowTexture;
layout(set=0, binding=4) uniform sampler2D normalTex;
layout(set=0, binding=5) uniform sampler2D worldTex;

layout (set=0, binding=6) uniform sampler_kernel{
    vec4 krn_sample[64];
};

layout(set=0, binding=7) uniform sampler2D noiseTex;

const uint kernelSize = 64;

uniform layout( push_constant) Dof
{
    mat4 proj;
    vec4 settings;
    float dofEnabled;
    float glowEnabled;
    float ditherEnabled;
    float lensFlareEnabled;
    vec2 flarePos;
    vec2 screenSize;
} dof;

// Noise function 
// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

float rand(vec2 n) { 
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);
    
    float res = mix(
        mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
        mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
    return res*res;
}

float noise(float p)
{
    return noise(vec2(p));
}

// end noise

vec4 SRGBToLinear(vec4 c)
{
    return vec4(pow(c.rgb, vec3(2.4)), c.a);
}

float blurclamp = 0.5;  // max blur amount
float aspectratio = 1920.0 / 1080.0;
vec2 aspectcorrect = vec2(1.0,aspectratio) * 0.1;

const float kernel[41] = float[41](0.003848, 0.004894, 0.006148,
0.007629, 0.009351, 0.011321, 0.013537, 0.01599, 0.018655,
0.021497, 0.02447, 0.027511, 0.030552, 0.033513, 0.03631,
0.038858, 0.041076, 0.042888, 0.04423, 0.045056, 0.045335,
0.045056, 0.04423, 0.042888, 0.041076, 0.038858, 0.03631,
0.033513, 0.030552, 0.027511, 0.02447, 0.021497, 0.018655,
0.01599, 0.013537, 0.011321, 0.009351, 0.007629, 0.006148, 0.004894, 0.003848);

vec4 SampleTexture(vec2 dofblur)
{
    vec4 col = vec4(0.0);
    col += texture(scene, uv);
    col += texture(scene, uv + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur);
    col += texture(scene, uv + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur);
    col += texture(scene, uv + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur);
    col += texture(scene, uv + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur);
    col += texture(scene, uv + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur);
    col += texture(scene, uv + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur);
    col += texture(scene, uv + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur);	
    col += texture(scene, uv + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur);

    col += texture(scene, uv + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur*0.9);
    col += texture(scene, uv + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur*0.9);		
    col += texture(scene, uv + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur*0.9);		
    col += texture(scene, uv + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur*0.9);
    col += texture(scene, uv + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur*0.9);
    col += texture(scene, uv + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur*0.9);		
    col += texture(scene, uv + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur*0.9);	
    col += texture(scene, uv + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur*0.9);	

    col += texture(scene, uv + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.7);
    col += texture(scene, uv + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.7);	
    col += texture(scene, uv + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.7);	
    col += texture(scene, uv + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.7);	
    col += texture(scene, uv + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.7);
    col += texture(scene, uv + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.7);	
    col += texture(scene, uv + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.7);	
    col += texture(scene, uv + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.7);
            
    col += texture(scene, uv + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.4);
    col += texture(scene, uv + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.4);	
    col += texture(scene, uv + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.4);	
    col += texture(scene, uv + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.4);	
    col += texture(scene, uv + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.4);
    col += texture(scene, uv + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.4);	
    col += texture(scene, uv + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.4);	
    col += texture(scene, uv + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.4);	
        
    col /= 41;
    return col;
}

float toLinear(float depth, float near, float far) {
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

vec4 sampleGlow()
{
    vec4 glow = vec4(0.0);
    for(int x = -20; x < 21; x++ )
    {
        for(int y = -20; y < 21; y++ )
        {
            glow += texture(glowTexture, uv + vec2(dof.settings.z * x, dof.settings.w * y)) * kernel[x + 20] * kernel[y + 20];
        }
    }
    return glow;
}

vec3 lensFlare(vec2 uv, vec2 pos)
{
    vec2 main = uv-pos;
	vec2 uvd = uv*(length(uv));
	
	float ang = atan(main.x,main.y);
	float dist=length(main); dist = pow(dist,.1);
	float n = noise(vec2(ang*16.0,dist*32.0));
	
	float f0 = 1.0/(length(uv-pos)*16.0+1.0);
	
	f0 = f0 + f0*(sin(noise(sin(ang*2.+pos.x)*4.0 - cos(ang*3.+pos.y))*16.)*.1 + dist*.1 + .8);
	
	float f1 = max(0.01-pow(length(uv+1.2*pos),1.9),.0)*7.0;

	float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.25;
	float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.23;
	float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.21;
	
	vec2 uvx = mix(uv,uvd,-0.5);
	
	float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
	float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
	float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;
	
	uvx = mix(uv,uvd,-.4);
	
	float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
	float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
	float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;
	
	uvx = mix(uv,uvd,-0.5);
	
	float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
	float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
	float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;
	
	vec3 c = vec3(.0);
	
	c.r+=f2+f4+f5+f6; c.g+=f22+f42+f52+f62; c.b+=f23+f43+f53+f63;
	c = c*1.3 - vec3(length(uvd)*.05);
	c+=vec3(f0);
	
	return c;
}

vec4 ao()
{

    // Get G-Buffer values
    vec3 fragPos = texture(worldTex, uv).rgb;
    vec3 normal = normalize(texture(normalTex, uv).rgb * 2.0 - 1.0);

    // Get a random vector using a noise lookup
    ivec2 texDim = textureSize(worldTex, 0); 
    ivec2 noiseDim = textureSize(noiseTex, 0);
    const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * uv;  
    vec3 randomVec = texture(noiseTex, noiseUV).xyz * 2.0 - 1.0;

    // Create TBN matrix
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));

    // return vec4(tangent, 1.0);

    vec3 bitangent = cross(tangent, normal);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // Calculate occlusion value
    float occlusion = 0.0;
    // remove banding
    const float bias = 0.025;
    for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
    {		
        vec3 samplePos = TBN * krn_sample[i].xyz; 
        samplePos = fragPos + samplePos * SSAO_RADIUS; 
        
        // project
        vec4 offset = vec4(samplePos, 1.0);
        offset = dof.proj * offset; 
        offset.xyz /= offset.w; 
        offset.xyz = offset.xyz * 0.5 + 0.5; 
        
        float sampleDepth = -texture(worldTex, offset.xy).w;
        // float sampleDepth = -texture(depth, offset.xy ).x;
        // sampleDepth = toLinear(sampleDepth, 0.01, 100.0);

        float rangeCheck = smoothstep(0.0, 1.0, SSAO_RADIUS / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));

    return vec4(occlusion);

    // mat3 pinv = inverse(dof.proj);

    // vec3 fragPos   =  texture(worldTex, uv).xyz;
    // vec3 normal    = normalize(texture(normalTex, uv).rgb);
    // ivec2 texDim = textureSize(worldTex, 0); 
    // ivec2 noiseDim = textureSize(noiseTex, 0);
    // const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * uv;  
    // vec3 randomVec = (texture(noiseTex, noiseUV).xyz); 

    // vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    // vec3 bitangent = cross(tangent, normal);
    // mat3 TBN       = mat3(tangent, bitangent, normal);

    // // return vec4(tangent, 1.0);
    // float occlusion = 0.0;
    // float radius = 0.5;
    // float bias = 0.025;
    // for(int i = 0; i < kernelSize; ++i)
    // {
    //     // get sample position
    //     vec3 samplePos = TBN * krn_sample[i].xyz; // from tangent to view-space
    //     samplePos = fragPos + samplePos * radius; 
        
    //     vec4 offset = vec4(samplePos, 1.0);
    //     offset      = dof.proj * offset;    // from view to clip-space
    //     offset.xyz /= offset.w;               // perspective divide
    //     offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0 

    //     float sampleDepth = ( texture(worldTex, offset.xy)).z;
    //     occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0);

    //     float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
    //     occlusion       += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck; 
    // }

    // occlusion = 1.0 - (occlusion / kernelSize);

    // return vec4(occlusion);
}

void main()
{
	float depth1 = texture(depth, uv ).x;
    depth1 = toLinear(depth1, 0.01, 100.0);
	float factor = ( depth1 - dof.settings.x ) / dof.settings.y;
	 
	vec2 dofblur = vec2(factor); //vec2 (clamp( factor / dof.settings.y, -blurclamp, blurclamp ));
    float dither = 0.0;

    vec4 color = vec4(0.0);
    if (dof.dofEnabled > 1.0)
        color = SampleTexture(dofblur);
    else
        color = texture(scene, uv);

    color *= 0.8;
    vec3 lensFlareColor = vec3(0.0);
    if (dof.glowEnabled > 0.5)
    {
        vec4 glow = sampleGlow();
        color += (glow * 2.0);
    }
    if (dof.ditherEnabled > 0.5)
    {
        dither = noise(gl_FragCoord.xy * 2.0) * 2.5;
    }
    if (dof.lensFlareEnabled > 0.5)
    {
        lensFlareColor = lensFlare(uv, dof.flarePos);
    }

    vec4 finalColor = color + vec4(dither/255.0) + vec4(lensFlareColor, 0.0);

    // finalColor = ao();
    // finalColor = vec4(ao());
    // finalColor = clamp(texture(normalTex, uv), 0.0, 1.0);
    // finalColor.a = 1.0;
    vec4 col2 = SRGBToLinear(texture(gui, uv));
    fragColor = mix(finalColor, col2, col2.a);
}
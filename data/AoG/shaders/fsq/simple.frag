#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 fragColor;

layout(set=0, binding=0) uniform sampler2D gui;
layout(set=0, binding=1) uniform sampler2D scene;
layout(set=0, binding=2) uniform sampler2D depth;
layout(set=0, binding=3) uniform sampler2D glowTexture;


uniform layout( push_constant) Dof
{
    vec4 settings;
    float dofEnabled;
    float glowEnabled;
} dof;

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

void main()
{
	float depth1 = texture(depth, uv ).x;
    depth1 = toLinear(depth1, 0.01, 100.0);
	float factor = ( depth1 - dof.settings.x ) / dof.settings.y;
	 
	vec2 dofblur = vec2(factor); //vec2 (clamp( factor / dof.settings.y, -blurclamp, blurclamp ));

    vec4 color = vec4(0.0);
    if (dof.dofEnabled == 1.0)
        color = SampleTexture(dofblur);
    else
        color = texture(scene, uv);

    color *= 0.8;
    if (dof.glowEnabled == 1.0)
    {
        vec4 glow = sampleGlow();
        color += (glow * 2.0);
    }

    vec4 col2 = SRGBToLinear(texture(gui, uv));
    fragColor = mix(color, col2, col2.a);
}
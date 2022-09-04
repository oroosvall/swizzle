#version 450

layout(location = 0) in vec4 worldPos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec4 v_eye;

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 worldPosOut;

layout(set=0, binding=1) uniform Light
{
    vec4 lightPos; // w contains raidus
    vec4 lightColor; // w contains intensity
    vec4 specColor;
} lamp;

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

// end noise

void main()
{
    vec3 n = normalize(v_normal);
    float dist = distance(worldPos.xyz, lamp.lightPos.xyz);

    float dither = noise(gl_FragCoord.xy * 2.0) * 2.5;
	
	float d = lamp.lightPos.w;
	vec4 letThereBeLight = vec4(0);
	if(dist < d)
	{
		float attenuation = 1.0;
		if(dist != 0)
			attenuation = 1 - clamp((pow(dist,1.5) / d), 0, 1);
			attenuation = max(attenuation, 0);
		
		vec3 s = normalize(vec3(lamp.lightPos.xyz - worldPos.xyz));

		vec3 r = reflect(s, n.xyz);
	   
		letThereBeLight += 0.1 + vec4(lamp.lightColor.rgb * lamp.lightColor.w * attenuation * max(dot(n.xyz, s), 0), 1.0);
        dither *= mix(0.1, 1.0, attenuation);
	}

    diffuse = vec4((vertexColor * letThereBeLight).rgb + (dither/255.0), 1.0);
    normal = vec4(v_normal, 1.0);
    worldPosOut = vec4(worldPos.xyz, 1.0);
}
#version 450

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 worldPos;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec4 v_eye;

layout(location = 0) out vec4 fragColor;

layout(set=0, binding=0) uniform sampler2D diffuseTexture;

layout(set=0, binding=1) uniform Light
{
    vec4 lightColor;
} lamp;

const vec3 lightPos = vec3(1.0, 1.0 ,1.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 diffuseColor = vec3(0.7, 0.3 ,0.15);
const vec3 specColor = vec3(1.0, 1.0, 1.0);

void main()
{
    /*
    vec3 normal = normalize(v_normal); 
    vec3 lightDir = normalize(lightPos - worldPos.xyz);

    float lambertian = max(dot(lightDir,normal), 0.0);
    float specular = 0.0;

    if(lambertian > 0.0) {

        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(-worldPos.xyz);

        float specAngle = max(dot(reflectDir, viewDir), 0.0);
        specular = pow(specAngle, 4.0);
    }
    */

    vec3 normal = normalize(v_normal); 
    float dist = distance(worldPos.xyz, lightPos);
	
	float lampRadius = 300.0;
	
	vec3 letThereBeLight = vec3(0);
    float specular = 0.0;

	//if(dist < lampRadius)
	{
		float attenuation = 1.0;
		//if(dist != 0)
		attenuation = 1.0 - clamp((pow(dist,1.5) / lampRadius), 0.0, 1.0);
		attenuation = max(attenuation, 0.0);
		//attenuation = 1.0;

		vec3 s = normalize(vec3(lightPos - worldPos.xyz));

		vec3 r = reflect(s, normal);
	   
        vec3 viewDir = normalize(v_eye - worldPos).xyz;

        float specAngle = max(dot(r, -viewDir), 0.0);
        specular = attenuation * pow(specAngle, 4.0);

		letThereBeLight += lamp.lightColor.rgb * attenuation * max(dot(normal, s), 0.0);
	
	}

    //fragColor = vec4( lambertian*diffuseColor + specular*specColor, 1.0);

    //fragColor = vec4((diffuseColor * letThereBeLight).rgb + specular*specColor, 1.0);
    //fragColor = vec4(texture(diffuseTexture, uv).rgb, 1.0) * vec4((diffuseColor * letThereBeLight).rgb + specular*specColor, 1.0);
    vec2 newUv = vec2(uv.x, 1.0 - uv.y);
    fragColor = vec4(texture(diffuseTexture, newUv).rgb, 1.0);

    //fragColor = vec4(uv, 0.0,  1.0);
}
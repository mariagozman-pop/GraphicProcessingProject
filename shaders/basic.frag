#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

// Matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;


// Lighting
uniform vec3 lightDir;  // Direction of the directional light (sun)
uniform vec3 lightColor;  // Color of the directional light (sun)

uniform vec3 lightBulbPos;  // Position of the light bulb (point light)
uniform vec3 lightBulbColor;  // Color of the light bulb (point light)

uniform vec3 lightSource1Pos;  // Position of the light source 1 (point light)
uniform vec3 lightSource1Color; // Color of the light source 1 (point light)

uniform vec3 lightSource2Pos;  // Position of the light source 2 (point light)
uniform vec3 lightSource2Color; // Color of the light source 2 (point light)

uniform vec3 lightSource3Pos;  // Position of the light source 1 (point light)
uniform vec3 lightSource3Color; // Color of the light source 1 (point light)


// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;


// Lighting parameters
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;


uniform bool isLightBulb;  
uniform vec3 emissiveColor;


// Fog Parameters
uniform bool enableFog;      
uniform vec3 fogColor;        
uniform float fogDensity;     

// Transparent Objects
uniform bool isTransparent; 
uniform vec3 tintColor;

float computeFogFactor(float distance) {
    float fogFactor = exp(-pow(fogDensity * distance, 2.0f));
    return clamp(fogFactor, 0.0f, 1.0f); 
}

void computeDirLight() {
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    vec3 lightDirN = normalize(lightDir);

    vec3 viewDir = normalize(-fPosEye.xyz);

    ambient = ambientStrength * lightColor;

    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

void computePointLights() {
    vec3 lightPositions[4] = {lightBulbPos, lightSource1Pos, lightSource2Pos, lightSource3Pos};
    vec3 lightColors[4] = {lightBulbColor, lightSource1Color, lightSource2Color, lightSource3Color};
    float specStrengths[4] = {specularStrength, specularStrength, specularStrength, specularStrength};

    for (int i = 0; i < 4; ++i) {
        vec3 fPosWorld = vec3(model * vec4(fPosition, 1.0f));
        vec3 normalWorld = normalize(normalMatrix * fNormal);

	vec3 adjustedLightPos = lightPositions[i] + vec3(6.0f, 6.0f, 6.0f);
	vec3 lightDirN = normalize(adjustedLightPos - fPosWorld);
	float distance = length(adjustedLightPos - fPosWorld);

        float constant = 1.0f;
        float linear = 0.02f;
        float quadratic = 0.005f;

        float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));

        vec4 fPosEye = view * vec4(fPosWorld, 1.0f);
        vec3 viewDir = normalize(-fPosEye.xyz);

        vec3 ambientPoint = ambientStrength * lightColors[i];
        vec3 diffusePoint = max(dot(normalWorld, lightDirN), 0.0f) * lightColors[i];

        vec3 reflectDir = reflect(-lightDirN, normalWorld);
        float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
        vec3 specularPoint = specStrengths[i] * specCoeff * lightColors[i];

        ambientPoint *= attenuation;
        diffusePoint *= attenuation;
        specularPoint *= attenuation;

        if (i == 0 && !isLightBulb) {
            ambient += ambientPoint;
            diffuse += diffusePoint;
            specular += specularPoint;
        }
        else if (i != 0) {
            ambient += ambientPoint;
            diffuse += diffusePoint;
            specular += specularPoint;
	}
    }
}

void main() 
{
    if (isLightBulb) {
    	fColor = vec4(emissiveColor, 1.0f);    } 
    else {
	if(isTransparent) {
    	    vec4 diffuseColor = texture(diffuseTexture, fTexCoords);
    	    vec3 specularColor = texture(specularTexture, fTexCoords).rgb;

            float alpha = isTransparent ? mix(diffuseColor.a, 0.6, 0.5) : diffuseColor.a;

            ambient = ambientStrength * lightColor;

            vec3 baseColor = mix(tintColor, diffuseColor.rgb, 0.5);
            vec3 color = (ambient + diffuse) * baseColor + specular * specularColor;

            if (enableFog) {
        	float distance = length(view * vec4(fPosition, 1.0)); 
        	float fogFactor = computeFogFactor(distance);
        	color = mix(fogColor, color, fogFactor);
        	alpha *= fogFactor; // Attenuate alpha with fog
    	    }

      	    fColor = vec4(color * alpha, alpha);
	}
	else {

       	    computeDirLight();
    	    computePointLights();

    	    vec3 diffuseColor = texture(diffuseTexture, fTexCoords).rgb;
    	    vec3 specularColor = texture(specularTexture, fTexCoords).rgb;

    	    vec3 color = min((ambient + diffuse) * diffuseColor + specular * specularColor, 1.0f);

    	    if (texture(diffuseTexture, fTexCoords).a < 0.1) {
       	        discard;
    	    }

    	    if (enableFog) {
                float distance = length(view * vec4(fPosition, 1.0)); 

            	float fogFactor = computeFogFactor(distance);

            	color = mix(fogColor, color, fogFactor);
            }

            fColor = vec4(color, 1.0f); 
	}
    }
}

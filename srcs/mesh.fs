#version 330 core

in vec3 vColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 vNormal;

out vec4 FragColor;

uniform sampler2D scopTexture;
uniform sampler2D map_Kd;
uniform sampler2D map_Ka;
uniform sampler2D map_Ks;
uniform sampler2D map_Ns;
uniform sampler2D map_d;
uniform sampler2D bump;

uniform int isMap_Kd;
uniform int isMap_Ka;
uniform int isMap_Ks;
uniform int isMap_Ns;
uniform int isMap_d;
uniform int isBump;

uniform bool useTexture;
uniform bool hasMtl;
uniform vec3 Kd; //Kouleur diffuse
uniform vec3 Ka; //Kouleur ambiante
uniform vec3 Ks; //Kouleur speculaire
uniform float Ns; //shininess
uniform int illum;
uniform float d;

uniform float transition;

uniform vec3 lightDir; //direction de la lumiere
uniform vec3 viewPos;   //pos de la camera
uniform vec3 lightColor;

void main() {

    //Couleur diffuse (Kd)
    vec3 baseColor = (!hasMtl) ? vColor : Kd;
    if (isMap_Kd > 0)
        baseColor = texture(map_Kd, TexCoord).rgb;
    vec3 textureColor = useTexture ? texture(scopTexture, TexCoord).rgb : baseColor;
    vec3 outputColor = mix(baseColor, textureColor, transition);

    //Couleur Ambiante (Ka)
    vec3 ambientColor = Ka;
    if (isMap_Ka > 0)
        ambientColor = texture(map_Ka, TexCoord).rgb;

    //couleur speculaire (Ks)
    vec3 specColor = Ks;
    if (isMap_Ks > 0)
        specColor = texture(map_Ks, TexCoord).rgb;
    
    //Shininess (Ns);
    float shininess = Ns;
    if (isMap_Ns > 0)
        shininess = texture(map_Ns, TexCoord).r * 255.0;
    
    //Opacity
    float alpha = d;
    if (isMap_d > 0)
        alpha = texture(map_d, TexCoord).r;
    
    //Vecteur de light
    vec3 norm = normalize(vNormal);
    vec3 light = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - FragPos);

    //Ambiant (Ka)
    float ambientStrength = 0.8;   // augmente => ombres plus claires
    vec3 ambient = outputColor * ambientColor * ambientStrength;

    //Diffuse (Kd)
    float diff = max(dot(norm, light), 0.0);
    float diffuseStrength = 1.0;   // diminue => ombres plus douces
    vec3 diffuse = outputColor * diff * diffuseStrength;

    //Speculaire (Ks)
    vec3 reflectDir = reflect(-light, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specColor * spec * lightColor;

    vec3 finalColor = vec3(0.0, 0.0, 0.0);
    //Couleur
    switch(illum) {
        case 3:
            finalColor += specular;
        case 2:
            finalColor += diffuse;
        case 1:
            finalColor += ambient;
            break ;
        case 0:
            finalColor = outputColor;
            break;
        default:
            finalColor = outputColor;
    }
    FragColor = vec4(finalColor, d);
}
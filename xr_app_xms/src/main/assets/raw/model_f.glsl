#version 300 es

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexcoord0;

uniform vec3 eyePos;
uniform vec3 modelColor;

uniform sampler2D srcTex;

out highp vec4 outColor;

//--------------------------------------------------------------------------------------
vec4 GetSchlickFresnel(vec3 F0, vec3 Vector1, vec3 Vector2)
//--------------------------------------------------------------------------------------
{
    // See https://en.wikipedia.org/wiki/Schlick%27s_approximation
    // Also "Physics and Math of Shading", Hoffman, Slide 70
    // Parameters are "Vector1" and "Vector2" because for Environment map
    // we use normal and viewing angle but for Specular highlight we use
    // light and half vectors.
    float CosTheta = max(0.0, dot(Vector1, Vector2));
    float PowerTerm = pow(1.0 - CosTheta, 5.0);

    vec3 NewColor = F0 + (vec3(1.0, 1.0, 1.0) - F0) * PowerTerm;

    return vec4(NewColor.rgb, 1.0);
}

//--------------------------------------------------------------------------------------
void main()
//--------------------------------------------------------------------------------------
{
    // **********************
    // Texture Value
    // **********************
    vec4 srcColor = texture(srcTex, vTexcoord0);
    srcColor.xyz *= modelColor.xyz;

    // **********************
    // Normal Value
    // **********************
    vec3 FinalNorm = normalize(vWorldNormal);

    // **********************
    // View/Light Vectors
    // **********************
    vec3 LightPos = vec3(0.0, 0.0, 0.0);
    vec3 Light = normalize(LightPos.xyz - vWorldPos.xyz);

    vec3 View = normalize(eyePos.xyz - vWorldPos.xyz);
    vec3 Half = normalize(Light + View);

    // **********************
    // Diffuse Light
    // **********************
    float DiffuseAmount = dot(FinalNorm, Light.xyz);
    DiffuseAmount = clamp(DiffuseAmount, 0.1, 1.0);

    // **********************
    // Schlick Fresnel Values
    // **********************
    vec4 Schlick_LdotH = GetSchlickFresnel(vec3(0.04, 0.04, 0.04), Light, Half);

    // **********************
    // Gloss Value
    // **********************
    float AlphaP = 128.0;
    float MicroFacet = max(0.0, dot(FinalNorm, Half));
    MicroFacet = max(0.1, pow(MicroFacet, AlphaP));

    // **********************
    // Specular Color
    // Equation 10: "Crafting Physically Motivated Shading Models for Game Development", Naty Hoffman
    // **********************
    vec4 SpecColor = ((AlphaP + 2.0) / 8.0) * MicroFacet * Schlick_LdotH;


    // **********************
    // Output Color
    // **********************
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    // Object color + Spec color...
    vec4 FinalColor = vec4(srcColor.xyz, 1.0) + vec4(lightColor.xyz * SpecColor.xyz, 1.0);

    // ...multiplied by diffuse amount and light color
    FinalColor = DiffuseAmount * FinalColor * lightColor;

    outColor = FinalColor;
}


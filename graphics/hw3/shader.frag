#version 120
//#version 410

// Output Color
//out vec4 color;

uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;

// Texture-related data;
// from Week11 Texture Mapping Slide page 17
// rgbTexture: This variable represents the texture unit index. If its value is zero it will fetch from texture unit 0. Its value is given such as glUniform1i(mySamplerLoc, 0)
uniform sampler2D rgbTexture;
//layout(binding = 1) uniform sampler2D height_texture;
uniform int widthTexture;
uniform int heightTexture;

// Data from Vertex Shader
varying vec2 textureCoordinate;
varying vec3 vertexNormal; // For Lighting computation
varying vec3 ToLightVector; // Vector from Vertex to Light;
varying vec3 ToCameraVector; // Vector from Vertex to Camera;
//in vec2 textureCoordinate;
//in vec3 vertexNormal; // For Lighting computation
//in vec3 ToLightVector; // Vector from Vertex to Light;
//in vec3 ToCameraVector; // Vector from Vertex to Camera;

void main() {

  // Assignment Constants below
  // get the texture color
  vec4 textureColor = texture2D(rgbTexture, textureCoordinate);
  //vec4 textureColor = texture(rgbTexture, textureCoordinate);

  // apply Phong shading by using the following parameters
  vec4 ka = vec4(0.25,0.25,0.25,1.0); // reflectance coeff. for ambient
  vec4 Ia = vec4(0.3,0.3,0.3,1.0); // light color for ambient
  vec4 Id = vec4(1.0, 1.0, 1.0, 1.0); // light color for diffuse
  vec4 kd = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for diffuse
  vec4 Is = vec4(1.0, 1.0, 1.0, 1.0); // light color for specular
  vec4 ks = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for specular
  int specExp = 100; // specular exponent

  float cos_theta1 = dot(vertexNormal, ToLightVector);
  float cos_theta = 0;
  if (cos_theta1 > 0)
  {
    cos_theta = cos_theta1;
  }

  vec3 half_vec = normalize(ToLightVector + ToCameraVector);
  float cos_alpha1 = dot(vertexNormal, half_vec);
  float cos_alpha = 0;
  if (cos_alpha1 > 0)
  {
    cos_alpha = cos_alpha1;
  }

  // compute ambient component
  vec4 ambient = ka * Ia;
  // compute diffuse component
  vec4 diffuse = kd * Id * cos_theta;
  // compute specular component
  vec4 specular = ks * Is * pow(cos_alpha, specExp);

  // compute the color using the following equation
  //color = vec4(clamp( textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);
  gl_FragColor = vec4(clamp( textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);
}

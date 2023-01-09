#version 400


in vec3 VertexPosition;
out vec3 TexCoord;
void main()
{
  vec4 newPos = vec4(VertexPosition, 1.0);
  gl_Position =newPos;
  TexCoord = VertexPosition;

  

}


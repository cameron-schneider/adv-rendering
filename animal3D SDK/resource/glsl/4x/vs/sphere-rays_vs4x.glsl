

#version 450

// vertices for a triangle-strip at near plane
const vec4 kVertex[] = vec4[](
    vec4(-1.0, -1.0, -1.0, 1.0),
    vec4(+1.0, -1.0, -1.0, 1.0),
    vec4(-1.0, +1.0, -1.0, 1.0),
    vec4(+1.0, +1.0, -1.0, 1.0)
);

out vec4 vPosition_clip;

void main()
{
    gl_Position = vPosition_clip = kVertex[gl_VertexID];
}


#version 450

in vec4 vPosition_clip;

uniform mat4 uProjectionMat, uProjectionMatInv;
uniform int numBounces;

buffer uboSpheres
{
    int sphereCount;
    sSphere[];
};

struct sRay
{
    vec4 origin;    // w = 1
    vec4 direction; // w = 0
    vec4 rayColor;  // used to track what color our ray starts as, modified after each bounce to calculate the final pixel color
    float intensity;    // used to dampen the ray's lighting strength after each bounce
};

struct sSphere
{
    mat4 object2view; // model-view matrix; center is fourth column
    mat4 view2object; // model-view inverse matrix
    float radius, radiusSq, radiusInv, radiusSqInv;
};


struct sSphereHit
{
    vec4 hitPoint;
    vec4 hitNormal;
    vec4 hitTexCoord;
    vec4 hitColor;
};


const vec4 kOrigin = vec4(0.0, 0.0, 0.0, 1.0);

layout (location = 0) out vec4 cFragColor;
layout (location = 1) out vec4 cFragNormal;
layout (location = 2) out vec4 cFragPosition;
layout (location = 3) out vec4 cFragTexcoord;
//out float gl_FragDepth; // built-in variable for depth; leave commented

sSphereHit TraceRay(sRay in)
{
    sSphereHit lastHit;

    for(int i = 0; i <= numBounces; i++)
    {
        // 1. Trace forward in the direction of the ray, from it's starting origin and in it's starting direction
            /*
                Perform a mathematical sphere test against every sphere in uboSpheres.
                If we didn't hit a sphere, return immediately with no result.
            */
        // 2. If we hit a sphere:
            /*
                Modify the ray's color by multiplying it by the sphere's texture color at our hit location and our intensity, and dampen its intensity by some factor.
            /*    
        // 3. If we have more than 0 bounces:
            /*
                Change the ray's direction to be the result of any scattering randomosity factor multiplied by the rays direction times the normal at the hit point.
            */
        // 3.5 if we have 0 bounces:
            /*
                Fill "lastHit" with the correct information (normal, point, color, texcoord)
            */
        // 4. Repeat
    }

    // At the end of our bounce loop, our ray will now contain the correct color.
    // We can then return our last-hit sphere information;

    // NOTE: in this implementation, I don't actually care about bounces, so imagine 0 bounces, just fire and wait for a hit.

    return lastHit;
}

void main()
{
    sRay ray;
    ray.origin = kOrigin;
    ray.direction = uProjectionMatInv * vPosition_clip;
    ray.direction = ray.direction / ray.direction.w - ray.origin;

    sSphereHit hit = TraceRay(ray);

    // ****TO-DO: CORRECTLY SET THESE
    cFragColor = hit.hitColor;
    cFragNormal = hit.hitNormal;
    cFragPosition = hit.hitPoint;
    cFragTexcoord = hit.hitTexCoord;

    gl_FragDepth = 1.0;
}
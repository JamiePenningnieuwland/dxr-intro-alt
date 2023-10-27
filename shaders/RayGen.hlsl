/* Copyright (c) 2018-2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Common.hlsl"

// ---[ Ray Generation Shader ]---
float PseudoRandom(float2 uv)
{
    float value = frac(sin(dot(uv, float2(12.9898, 78.233)) * 43758.5453));
    return (2.0 * value) - 1.0;
}
// ---[ Ray Generation Shader ]---
float3 ShootRays(int numRays, uint2 pixel, float aspectRatio)
{
    float3 color = float3(0., 0., 0.);
    int ray = 0;
    for (ray = 0; ray < numRays; ray++)
    {
        // Generate random numbers for jittering
        float jitterX = PseudoRandom(float2(pixel.x, pixel.y)) * 0.5;
        float jitterY = PseudoRandom(pixel) * 0.5;

        // Jitter the pixel coordinates for antialiasing
        float2 p = float2(pixel.x, pixel.y); 
        p.x += (ray % 2) * 0.5 + jitterX;
        p.y += (ray / 2) * 0.5 + jitterY;
        float2 d = (((p.xy + 0.5f) / resolution.xy) * 2.f - 1.f);

        // Calculate the ray direction for this pixel
        // Use px and py to calculate viewOriginAndTanHalfFovY, aspectRatio, and other parameters
        RayDesc ray;
        ray.Origin = viewOriginAndTanHalfFovY.xyz;
        ray.Direction = normalize((d.x * view[0].xyz * viewOriginAndTanHalfFovY.w * aspectRatio) - (d.y * view[1].xyz * viewOriginAndTanHalfFovY.w) + view[2].xyz);
        ray.TMin = 0.1f;
        ray.TMax = 1000.f;

        // Trace the ray
        HitInfo payload;
        payload.ShadedColorAndHitT = float4(0.f, 0.f, 0.f, 0.f);

        TraceRay(
        SceneBVH,
        RAY_FLAG_NONE,
        0xFF,
        0,
        0,
        0,
        ray,
        payload);

    // Accumulate the results for this sample
        color += payload.ShadedColorAndHitT.rgb;
    }

    // Average the accumulated color over all samples
    color /= numRays;
    return color;
}
[shader("raygeneration")]
void RayGen()
{
	uint2 LaunchIndex = DispatchRaysIndex().xy;
	uint2 LaunchDimensions = DispatchRaysDimensions().xy;

	float2 d = (((LaunchIndex.xy + 0.5f) / resolution.xy) * 2.f - 1.f);
	float aspectRatio = (resolution.x / resolution.y);

    float3 color =  ShootRays(10, LaunchIndex, aspectRatio);
    RTOutput[LaunchIndex.xy] = float4(color.rgb, 1.f);
}

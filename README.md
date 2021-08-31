# OpenGLRenderer
Physically based deferred renderer in OpenGL with UI created using ImGui. Capabilities include viewing G-buffer and shader output contents, debug drawing of wireframes, camera frustums, bounding boxes of meshes and
and light volumes. I used point light volumes by rendering the bounding box of the region a point light affects. I implemented SSAO using a technique that is cache efficient, using separate render passes
for each jittered sample pattern. (https://developer.nvidia.com/sites/default/files/akamai/gameworks/samples/DeinterleavedTexturing.pdf) This is to build a foundation for other implementing other screen space effects
that are possibly even more costly than SSAO. Point lights can be configured to have omnidirectional shadow maps with percentage closer filtering. For now they remain static, but I plan to add
support for dynamic objects. The directional light uses cascaded shadow maps with percentage closer filtering as well. The shading models supported are the Blinn-Phong model and a specular
microfacet BRDF using the GGX normal distribution and the Smith masking function.

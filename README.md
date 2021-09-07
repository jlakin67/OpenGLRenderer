# OpenGLRenderer
Physically based deferred renderer in OpenGL with UI created using ImGui. Capabilities include viewing G-buffer and shader output contents, debug drawing of wireframes, camera frustums, bounding boxes of meshes and
and light volumes. I used point light volumes by rendering the bounding box of the region a point light affects. I implemented SSAO using a technique that is cache efficient, using separate render passes
for each jittered sample pattern. (https://developer.nvidia.com/sites/default/files/akamai/gameworks/samples/DeinterleavedTexturing.pdf) This is to build a foundation for other implementing other screen space effects
that are possibly even more costly than SSAO. Point lights can be configured to have omnidirectional shadow maps with percentage closer filtering. For now they remain static, but I plan to add
support for dynamic objects. The directional light uses cascaded shadow maps with percentage closer filtering as well. The shading models supported are the Blinn-Phong model and a specular
microfacet BRDF using the GGX normal distribution and the Smith masking function. For more efficient rendering of geometry I made a simple version of coherent hierarchical occlusion using hardware occlusion queries.
![Example1](sponza_shadows.png?raw=true "Example screenshot1")
![Example2](ssao.png?raw=true "Example screenshot2")
![Example3](tree_with_ui.png?raw=true "Example screenshot3")

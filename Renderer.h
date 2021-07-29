#pragma once

namespace Renderer {


	//displaying for debugging purposes
	enum RenderModes {RENDER_DEFAULT, RENDER_POSITION, RENDER_NORMAL, RENDER_ALBEDO, RENDER_DEPTH,
					  RENDER_SPECULARITY, RENDER_SHADOW, RENDER_CASCADE_DEPTHS, RENDER_WIREFRAME, NUM_RENDER_MODES};
	enum DisplaySkybox {SKYBOX_DEFAULT, SKYBOX_SHADOW_MAP, NUM_SKYBOXES};
	enum RenderFrustumOutline {NO_FRUSTUM_OUTLINE, VIEW_FRUSTUM_OUTLINE, CASCADE_FRUSTUM_OUTLINE, NUM_FRUSTUM_OUTLINES};
	extern int render_mode;
	extern int skybox_mode;
	extern int frustum_outline_mode;
}
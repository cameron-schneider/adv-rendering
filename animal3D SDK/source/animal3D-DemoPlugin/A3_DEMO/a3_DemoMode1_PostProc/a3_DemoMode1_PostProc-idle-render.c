/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein

	a3_DemoMode1_PostProc-idle-render.c
	Demo mode implementations: animation scene.

	********************************************
	*** RENDERING FOR ANIMATION SCENE MODE   ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_PostProc.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoRenderUtils.h"


// OpenGL
#ifdef _WIN32
#include <gl/glew.h>
#include <Windows.h>
#include <GL/GL.h>
#else	// !_WIN32
#include <OpenGL/gl3.h>
#endif	// _WIN32


//-----------------------------------------------------------------------------

// controls for pipelines mode
void a3postproc_render_controls(a3_DemoState const* demoState, a3_DemoMode1_PostProc const* demoMode,
	a3_TextRenderer const* text, a3vec4 const col,
	a3f32 const textAlign, a3f32 const textDepth, a3f32 const textOffsetDelta, a3f32 textOffset)
{
	// forward pipeline names
	a3byte const* renderModeName[postproc_renderMode_max] = {
		"Phong shading with multiple lights",
		"Phong shading with shadow mapping",
	};

	// render pass names
	a3byte const* renderPassName[postproc_renderPass_max] = {
		"Shadow map capture",
		"Scene",
		"Bright pass (half)",
		"Blur pass, 1st axis (half)",
		"Blur pass, 2nd axis (half)",
		"Bright pass (quarter)",
		"Blur pass, 1st axis (quarter)",
		"Blur pass, 2nd axis (quarter)",
		"Bright pass (eighth)",
		"Blur pass, 1st axis (eighth)",
		"Blur pass, 2nd axis (eighth)",
		"Final composite (bloom)",
	};

	// render target names
	a3byte const* renderTargetName_scene[postproc_renderTargetScene_max] = {
		"Color buffer",
		"Depth buffer",
	};
	a3byte const* renderTargetName_post[postproc_renderTargetPost_max] = {
		"Color buffer",
	};
	a3byte const* renderTargetName_shadow[postproc_renderTargetShadow_max] = {
		"Depth buffer",
	};
	a3byte const* const* renderTargetName[postproc_renderPass_max] = {
		renderTargetName_shadow,
		renderTargetName_scene,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
		renderTargetName_post,
	};

	// modes
	a3_DemoMode1_PostProc_RenderMode const renderMode = demoMode->renderMode;
	a3_DemoMode1_PostProc_RenderPass const renderPass = demoMode->renderPass;
	a3_DemoMode1_PostProc_RenderTarget const renderTarget = demoMode->renderTarget[renderPass],
		renderTargetCount = demoMode->renderTargetCount[renderPass];

	// lighting modes
	a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"    Render mode (%u / %u) ('j' | 'k'): %s", renderMode + 1, postproc_renderMode_max, renderModeName[renderMode]);
	a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"    Render pass (%u / %u) ('J' | 'K'): %s", renderPass + 1, postproc_renderPass_max, renderPassName[renderPass]);
	a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"        Render target (%u / %u) ('N' | 'M'): %s", renderTarget + 1, renderTargetCount, renderTargetName[renderPass][renderTarget]);
}


//-----------------------------------------------------------------------------

// sub-routine for rendering the demo state using the shading pipeline
void a3postproc_render(a3_DemoState const* demoState, a3_DemoMode1_PostProc const* demoMode, a3f64 const dt)
{
	// pointers
	const a3_DemoStateShaderProgram* currentDemoProgram;

	// framebuffers
	const a3_Framebuffer* currentWriteFBO = 0, * currentDisplayFBO = 0;

	// indices
	a3ui32 i = 0, j = 0;

	// RGB
	const a3vec4 rgba4[] = {
		{ 1.00f, 0.00f, 0.00f, 1.00f },	// red
		{ 1.00f, 0.25f, 0.00f, 1.00f },
		{ 1.00f, 0.50f, 0.00f, 1.00f },	// orange
		{ 1.00f, 0.75f, 0.00f, 1.00f },
		{ 1.00f, 1.00f, 0.00f, 1.00f },	// yellow
		{ 0.75f, 1.00f, 0.00f, 1.00f },
		{ 0.50f, 1.00f, 0.00f, 1.00f },	// lime
		{ 0.25f, 1.00f, 0.00f, 1.00f },
		{ 0.00f, 1.00f, 0.00f, 1.00f },	// green
		{ 0.00f, 1.00f, 0.25f, 1.00f },
		{ 0.00f, 1.00f, 0.50f, 1.00f },	// aqua
		{ 0.00f, 1.00f, 0.75f, 1.00f },
		{ 0.00f, 1.00f, 1.00f, 1.00f },	// cyan
		{ 0.00f, 0.75f, 1.00f, 1.00f },
		{ 0.00f, 0.50f, 1.00f, 1.00f },	// sky
		{ 0.00f, 0.25f, 1.00f, 1.00f },
		{ 0.00f, 0.00f, 1.00f, 1.00f },	// blue
		{ 0.25f, 0.00f, 1.00f, 1.00f },
		{ 0.50f, 0.00f, 1.00f, 1.00f },	// purple
		{ 0.75f, 0.00f, 1.00f, 1.00f },
		{ 1.00f, 0.00f, 1.00f, 1.00f },	// magenta
		{ 1.00f, 0.00f, 0.75f, 1.00f },
		{ 1.00f, 0.00f, 0.50f, 1.00f },	// rose
		{ 1.00f, 0.00f, 0.25f, 1.00f },
	};
	const a3vec4 grey4[] = {
		{ 0.5f, 0.5f, 0.5f, 1.0f },	// solid grey
		{ 0.5f, 0.5f, 0.5f, 0.5f },	// translucent grey
	};
	const a3ui32 hueCount = sizeof(rgba4) / sizeof(*rgba4),
		redIndex = 0, orangeIndex = 2, yellowIndex = 4, limeIndex = 6,
		greenIndex = 8, aquaIndex = 10, cyanIndex = 12, skyIndex = 14,
		blueIndex = 16, purpleIndex = 18, magentaIndex = 20, roseIndex = 22;
	const a3real
		* const red = rgba4[redIndex].v, * const orange = rgba4[orangeIndex].v, * const yellow = rgba4[yellowIndex].v, * const lime = rgba4[limeIndex].v,
		* const green = rgba4[greenIndex].v, * const aqua = rgba4[aquaIndex].v, * const cyan = rgba4[cyanIndex].v, * const sky = rgba4[skyIndex].v,
		* const blue = rgba4[blueIndex].v, * const purple = rgba4[purpleIndex].v, * const magenta = rgba4[magentaIndex].v, * const rose = rgba4[roseIndex].v,
		* const grey = grey4[0].v, * const grey_t = grey4[1].v;

	// camera used for drawing
	const a3_ProjectorComponent* activeCamera = demoMode->projector;
	const a3_SceneObjectComponent* activeCameraObject = activeCamera->sceneObjectPtr;
	const a3_SceneObjectComponent* currentSceneObject, * endSceneObject;

	// temp drawable pointers
	const a3_VertexDrawable* drawable[] = {
		0,								// root
		0,								// light
		0,								// camera
		demoState->draw_unit_box,		// skybox
		demoState->draw_unit_sphere,	// objects
		demoState->draw_unit_cylinder,
		demoState->draw_unit_capsule,
		demoState->draw_unit_torus,
		demoState->draw_unit_cone,
		demoState->draw_teapot,
		demoState->draw_unit_plane_z,
	};

	// temp texture pointers
	const a3_Texture* texture_dm[] = {
		0,								// root
		0,								// light
		0,								// camera
		demoState->tex_skybox_clouds,	// skybox
		demoState->tex_earth_dm,		// objects
		demoState->tex_mars_dm,
		demoState->tex_stone_dm,
		demoState->tex_sun_dm,
		demoState->tex_checker,
		demoState->tex_checker,
		demoState->tex_checker,
	};
	const a3_Texture* texture_sm[] = {
		0,								// root
		0,								// light
		0,								// camera
		0,								// skybox
		demoState->tex_earth_sm,		// objects
		demoState->tex_mars_sm,
		demoState->tex_stone_dm,
		demoState->tex_sun_dm,
		demoState->tex_checker,
		demoState->tex_checker,
		demoState->tex_checker,
	};

	// forward pipeline shader programs
	const a3_DemoStateShaderProgram* renderProgram[postproc_renderMode_max] = {
		demoState->prog_drawPhong_shadow,
		demoState->prog_drawPhong_shadow,
	};
	const a3ui32 renderModeLightCount[postproc_renderMode_max] = {
		postprocMaxCount_pointLight,
		1,
	};

	// ****TO-DO:
	//	-> uncomment FBO target array
	//	-> add pointer to target FBO for each pass
	//		(hint: choose the most relevant one for each; all are unique)
/*	// framebuffer target for each pass
	const a3_Framebuffer* writeFBO[postproc_renderPass_max] = {
		demoState->fbo_d32,
		//...
	};*/

	// target info
	a3_DemoMode1_PostProc_RenderMode const renderMode = demoMode->renderMode;
	a3_DemoMode1_PostProc_RenderPass const renderPass = demoMode->renderPass;
	a3_DemoMode1_PostProc_RenderTarget const renderTarget = demoMode->renderTarget[renderPass],
		renderTargetCount = demoMode->renderTargetCount[renderPass];

	// final model matrix and full matrix stack
	a3mat4 projectionMat = activeCamera->projectorMatrixStackPtr->projectionMat;
	a3mat4 projectionMatInv = activeCamera->projectorMatrixStackPtr->projectionMatInverse;
	a3mat4 viewProjectionMat = activeCamera->projectorMatrixStackPtr->viewProjectionMat;
	a3mat4 modelMat, modelViewMat, modelViewProjectionMat, atlasMat;

	// FSQ matrix
	const a3mat4 fsq = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// pixel size and effect axis
	//a3vec2 pixelSize = a3vec2_one;


	//-------------------------------------------------------------------------
	// 0) SHADOW PASS: render scene from light's perspective
	//	- activate shadow capture framebuffer
	//	- draw scene
	//		- clear depth buffer
	//		- render shapes using pass-thru shaders

	// ****TO-DO:
	//	-> uncomment shadow pass FBO binding
/*	// bind scene FBO
	currentWriteFBO = writeFBO[postproc_renderPassShadow]; //demoState->fbo_d32
	a3framebufferActivate(currentWriteFBO);*/
	glClear(GL_DEPTH_BUFFER_BIT);

	currentDemoProgram = demoState->prog_transform;
	a3shaderProgramActivate(currentDemoProgram->program);
	glDisable(GL_BLEND);

	// shadow capture on inverted geometry
	glCullFace(GL_FRONT);
	for (currentSceneObject = demoMode->obj_sphere, endSceneObject = demoMode->obj_ground;
		currentSceneObject <= endSceneObject; ++currentSceneObject)
	{
		j = currentSceneObject->sceneHierarchyIndex;

		// calculate and send MVP from light's perspective
		a3real4x4Product(modelViewProjectionMat.m,
			demoMode->proj_light_main->projectorMatrixStackPtr->viewProjectionMat.m,
			currentSceneObject->modelMatrixStackPtr->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uIndex, 1, &j);
		a3vertexDrawableActivateAndRender(drawable[j]);
	}
	glCullFace(GL_BACK);


	//-------------------------------------------------------------------------
	// 1) SCENE PASS: render scene with desired shader
	//	- activate scene framebuffer
	//	- draw scene
	//		- clear buffers
	//		- render shapes using appropriate shaders
	//		- capture color and depth

	// ****TO-DO:
	//	-> uncomment scene pass FBO binding
/*	// bind scene FBO
	currentWriteFBO = writeFBO[postproc_renderPassScene]; //demoState->fbo_c16x4_d24s8
	a3framebufferActivate(currentWriteFBO);*/

	// clear buffers
	if (demoState->displaySkybox)
	{
		// skybox clear: just draw skybox
		glDepthFunc(GL_ALWAYS);
		modelMat = demoMode->obj_skybox->modelMatrixStackPtr->modelMat;
		atlasMat = demoMode->obj_skybox->modelMatrixStackPtr->atlasMat;
		a3demo_drawModelTexturedColored_invertModel(
			modelViewProjectionMat.m, viewProjectionMat.m, modelMat.m, atlasMat.m,
			demoState->prog_drawTexture, demoState->draw_unit_box, demoState->tex_skybox_clouds, a3vec4_one.v);
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		// full clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// stencil test
	//if (demoState->stencilTest)
	//	a3demo_drawStencilTest(modelViewProjectionMat.m, viewProjectionMat.m, modelMat.m, demoState->prog_drawColorUnif, demoState->draw_unit_sphere);

	// select program based on settings
	currentDemoProgram = renderProgram[renderMode];
	a3shaderProgramActivate(currentDemoProgram->program);

	// send shared data: 
	//	- projection matrix
	//	- light data
	//	- activate shared textures including atlases if using
	//	- shared animation data
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP, 1, projectionMat.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP_inv, 1, projectionMatInv.mm);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
	a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor0, hueCount, rgba4->v);
	if (demoState->updateAnimation)
		a3shaderUniformSendDouble(a3unif_single, currentDemoProgram->uTime, 1, &demoState->timer_display->totalTime);

	// send lighting uniforms and bind blocks where appropriate
	a3shaderUniformBufferActivate(demoState->ubo_transform, demoProg_blockTransformStack);
	a3shaderUniformBufferActivate(demoState->ubo_light, demoProg_blockLight);

	// select pipeline algorithm
	glDisable(GL_BLEND);

	// forward shading algorithms
	for (currentSceneObject = demoMode->obj_sphere, endSceneObject = demoMode->obj_ground;
		currentSceneObject <= endSceneObject; ++currentSceneObject)
	{
		j = currentSceneObject->sceneHierarchyIndex;
		
		// activate texture maps
		a3textureActivate(texture_dm[j], a3tex_unit00);
		a3textureActivate(texture_sm[j], a3tex_unit01);
		// ****TO-DO:
		//	-> uncomment shadow map bind
	/*	a3framebufferBindDepthTexture(writeFBO[postproc_renderPassShadow], a3tex_unit06); //demoState->fbo_d32*/

		// send other data
		a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uIndex, 1, &j);
		a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uCount, 1, renderModeLightCount + renderMode);

		// draw
		a3vertexDrawableActivateAndRender(drawable[j]);
	}

	// stop using stencil
	if (demoState->stencilTest)
		glDisable(GL_STENCIL_TEST);


	//-------------------------------------------------------------------------
	// PREPARE FOR POST-PROCESSING
	//	- double buffer swap (if applicable)
	//	- ensure blending is disabled
	//	- re-activate FSQ drawable IF NEEDED (i.e. changed in previous step)
	glDisable(GL_BLEND);
	a3vertexDrawableActivate(demoState->draw_unit_plane_z);


	//-------------------------------------------------------------------------
	// POST-PROCESSING
	//	- activate target framebuffer
	//	- activate texture from previous framebuffer
	//	- draw FSQ with processing program active

	// optimized bloom algorithm: 
	//	-> render scene
	//	-> repeat a few times: 
	//		-> bright pass, half size
	//		-> blur in one direction (e.g. horizontal)
	//		-> blur in other direction (e.g. vertical)
	//	-> composite original scene result with final blur iteration results

	// ****TO-DO:
	//	-> uncomment first post-processing pass
	//	-> implement bloom pipeline following the above algorithm
	//		(hint: this is the entirety of the first bright pass)
/*	currentDemoProgram = demoState->prog_postBright;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3framebufferBindColorTexture(currentWriteFBO, a3tex_unit00, 0);
	currentWriteFBO = writeFBO[postproc_renderPassBright2];
	a3framebufferActivate(currentWriteFBO);
	a3vertexDrawableRenderActive();
	//...*/


	//-------------------------------------------------------------------------
	// DISPLAY: final pass, perform and present final composite
	//	- finally draw to back buffer
	//	- select display texture(s)
	//	- activate final pass program
	//	- draw final FSQ

	// revert to back buffer and disable depth testing
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable,
		-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);

	// ****TO-DO:
	//	-> uncomment display FBO selection
/*	// select framebuffer to display based on mode
	currentDisplayFBO = writeFBO[renderPass];

	// select output to display
	switch (renderPass)
	{
	case postproc_renderPassShadow:
		a3framebufferBindDepthTexture(currentDisplayFBO, a3tex_unit00);
		break;
	case postproc_renderPassScene:
		if (currentDisplayFBO->color && (!currentDisplayFBO->depthStencil || renderTarget < renderTargetCount - 1))
			a3framebufferBindColorTexture(currentDisplayFBO, a3tex_unit00, renderTarget);
		else
			a3framebufferBindDepthTexture(currentDisplayFBO, a3tex_unit00);
		break;
	case postproc_renderPassDisplay:
	case postproc_renderPassBright2:
	case postproc_renderPassBlurH2:
	case postproc_renderPassBlurV2:
	case postproc_renderPassBright4:
	case postproc_renderPassBlurH4:
	case postproc_renderPassBlurV4:
	case postproc_renderPassBright8:
	case postproc_renderPassBlurH8:
	case postproc_renderPassBlurV8:
		a3framebufferBindColorTexture(currentDisplayFBO, a3tex_unit00, renderTarget);
		break;
	}*/

	// final display: activate desired final program and draw FSQ
	if (currentDisplayFBO)
	{
		// determine if additional passes are required
		// set appropriate display program
		currentDemoProgram = demoState->prog_drawTexture;
		a3shaderProgramActivate(currentDemoProgram->program);

		// done
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);
		a3vertexDrawableRenderActive();
	}


	//-------------------------------------------------------------------------
	// OVERLAYS: done after FSQ so they appear over everything else
	//	- disable depth testing
	//	- draw overlays appropriately

	// enable alpha
	a3demo_enableCompositeBlending();

	// scene overlays
	if ((demoState->displayGrid || demoState->displayTangentBases || demoState->displayWireframe) &&
		renderPass != postproc_renderPassShadow)
	{
		// ****TO-DO:
		//	-> uncomment overlay FBO activation and configuration
	/*	// activate scene FBO and clear color; reuse depth
		currentWriteFBO = demoState->fbo_c16x4_d24s8;
		a3framebufferActivate(currentWriteFBO);
		glDisable(GL_STENCIL_TEST);
		glClear(GL_COLOR_BUFFER_BIT);*/

		// draw grid aligned to world
		if (demoState->displayGrid)
		{
			a3demo_drawModelSolidColor(modelViewProjectionMat.m, viewProjectionMat.m, a3mat4_identity.m, demoState->prog_drawColorUnif, demoState->draw_grid, blue);
		}

		if (demoState->displayTangentBases || demoState->displayWireframe)
		{
			const a3i32 flag[1] = { demoState->displayTangentBases * 3 + demoState->displayWireframe * 4 };
			const a3f32 size[1] = { 0.0625f };

			currentDemoProgram = demoState->prog_drawTangentBasis;
			a3shaderProgramActivate(currentDemoProgram->program);

			// projection matrix
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP, 1, projectionMat.mm);
			// wireframe color
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor0, hueCount, rgba4->v);
			// blend color
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);
			// tangent basis size
			a3shaderUniformSendFloat(a3unif_single, currentDemoProgram->uSize, 1, size);
			// overlay flag
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uFlag, 1, flag);

			for (currentSceneObject = demoMode->obj_sphere, endSceneObject = demoMode->obj_ground;
				currentSceneObject <= endSceneObject; ++currentSceneObject)
			{
				j = currentSceneObject->sceneHierarchyIndex;
				i = (j * 3) % hueCount;
				modelViewMat = currentSceneObject->modelMatrixStackPtr->modelViewMat;
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
				modelViewMat = currentSceneObject->modelMatrixStackPtr->modelViewMatInverseTranspose;
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV_nrm, 1, modelViewMat.mm);
				modelViewProjectionMat = currentSceneObject->modelMatrixStackPtr->modelViewProjectionMat;
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
				a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uIndex, 1, &i);
				a3vertexDrawableActivateAndRender(drawable[j]);
			}
		}

		// display color target with scene overlays
		a3framebufferDeactivateSetViewport(a3fbo_depthDisable,
			-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);
		currentDemoProgram = demoState->prog_drawTexture;
		a3vertexDrawableActivate(demoState->draw_unit_plane_z);
		a3shaderProgramActivate(currentDemoProgram->program);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, fsq.mm);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uAtlas, 1, a3mat4_identity.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, a3vec4_one.v);
		a3framebufferBindColorTexture(currentWriteFBO, a3tex_unit00, 0);
		a3vertexDrawableRenderActive();
	}

	// overlays with no depth
	glDisable(GL_DEPTH_TEST);

	// hidden volumes
	if (demoState->displayHiddenVolumes &&
		renderPass != postproc_renderPassShadow)
	{
		// lights
		currentDemoProgram = demoState->prog_drawColorUnif;
		a3shaderProgramActivate(currentDemoProgram->program);
		a3vertexDrawableActivate(demoState->draw_unit_sphere);
		a3real4x4SetScale(modelMat.m, 0.25f);
		for (i = 0; i < postprocMaxCount_pointLight; ++i)
		{
			modelMat.v3 = demoMode->pointLightData[i].worldPos;
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, demoMode->pointLightData[i].color.v);
			a3demo_drawModelSimple(modelViewProjectionMat.m, viewProjectionMat.m, modelMat.m, currentDemoProgram);
		}
	}

	// superimpose axes
	// draw coordinate axes in front of everything
	currentDemoProgram = demoState->prog_drawColorAttrib;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3vertexDrawableActivate(demoState->draw_axes);

	// center of world from current viewer
	// also draw other viewer/viewer-like object in scene
	if (demoState->displayWorldAxes &&
		renderPass != postproc_renderPassShadow)
	{
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, viewProjectionMat.mm);
		a3vertexDrawableRenderActive();
	}

	// individual objects (based on scene graph)
	if (demoState->displayObjectAxes &&
		renderPass != postproc_renderPassShadow)
	{
		for (currentSceneObject = demoMode->obj_sphere, endSceneObject = demoMode->obj_ground;
			currentSceneObject <= endSceneObject; ++currentSceneObject)
		{
			j = currentSceneObject->sceneHierarchyIndex;
			modelMat = currentSceneObject->modelMatrixStackPtr->modelMat;
			a3demo_drawModelSimple(modelViewProjectionMat.m, viewProjectionMat.m, modelMat.m, currentDemoProgram);
		}
	}
}


//-----------------------------------------------------------------------------

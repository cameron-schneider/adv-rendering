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

	a3_DemoMode0_Intro-idle-render.c
	Demo mode implementations: animation scene.

	********************************************
	*** RENDERING FOR ANIMATION SCENE MODE   ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode0_Intro.h"

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
void a3intro_render_controls(a3_DemoState const* demoState, a3_DemoMode0_Intro const* demoMode,
	a3_TextRenderer const* text, a3vec4 const col,
	a3f32 const textAlign, a3f32 const textDepth, a3f32 const textOffsetDelta, a3f32 textOffset)
{
	// forward pipeline names
	a3byte const* renderModeName[intro_renderMode_max] = {
		"Solid color",
		"Texture",
		"Lambert shading",
		"Phong shading",
	};

	// modes
	a3_DemoMode0_Intro_RenderMode const renderMode = demoMode->renderMode;

	// lighting modes
	a3textDraw(text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"    Rendering mode (%u / %u) ('j' | 'k'): %s", renderMode + 1, intro_renderMode_max, renderModeName[renderMode]);
}


//-----------------------------------------------------------------------------

// sub-routine for rendering the demo state using the shading pipeline
void a3intro_render(a3_DemoState const* demoState, a3_DemoMode0_Intro const* demoMode, a3f64 const dt)
{
	// pointers
	const a3_DemoStateShaderProgram* currentDemoProgram;

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
		0,								// camera
		demoState->tex_skybox_clouds,	// skybox
		demoState->tex_checker,			// objects
		demoState->tex_checker,
		demoState->tex_checker,
		demoState->tex_checker,
		demoState->tex_checker,
		demoState->tex_checker,
		demoState->tex_checker,
	};

	// forward pipeline shader programs
	const a3_DemoStateShaderProgram* renderProgram[intro_renderMode_max] = {
		demoState->prog_drawColorUnif,
		demoState->prog_drawTexture,
		demoState->prog_drawLambert,
		demoState->prog_drawPhong,
	};

	// target info
	a3_DemoMode0_Intro_RenderMode const renderMode = demoMode->renderMode;

	// final model matrix and full matrix stack
	a3mat4 projectionMat = activeCamera->projectorMatrixStackPtr->projectionMat;
	a3mat4 projectionMatInv = activeCamera->projectorMatrixStackPtr->projectionMatInverse;
	a3mat4 viewProjectionMat = activeCamera->projectorMatrixStackPtr->viewProjectionMat;
	a3mat4 modelMat, modelViewMat, modelViewProjectionMat, atlasMat;


	//-------------------------------------------------------------------------
	// 1) SCENE PASS: render scene with desired shader
	//	- activate scene framebuffer
	//	- draw scene
	//		- clear buffers
	//		- render shapes using appropriate shaders
	//		- capture color and depth

	// reset viewport
	a3framebufferDeactivateSetViewport(a3fbo_depth24_stencil8,
		-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);

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

	// ****DONE: 
	//	-> send lighting uniforms and bind blocks where appropriate


	// select pipeline algorithm
	glDisable(GL_BLEND);

	// forward shading algorithms
	for (currentSceneObject = demoMode->obj_sphere, endSceneObject = demoMode->obj_ground;
		currentSceneObject <= endSceneObject; ++currentSceneObject)
	{
		j = currentSceneObject->sceneHierarchyIndex;
		i = (j * 3 + hueCount / 2) % hueCount;
		switch (renderMode)
		{
		case intro_renderModePhong:
			// activate specular map, fall through to Lambert
			a3textureActivate(texture_dm[j], a3tex_unit01);
		case intro_renderModeLambert:
			// send lights and matrices, fall through to texturing
			modelViewMat = currentSceneObject->modelMatrixStackPtr->modelViewMat;
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
			modelViewMat = currentSceneObject->modelMatrixStackPtr->modelViewMatInverseTranspose;
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV_nrm, 1, modelViewMat.mm);
		case intro_renderModeTexture:
			// activate diffuse map, fall through to solid color
			a3textureActivate(texture_dm[j], a3tex_unit00);
		case intro_renderModeSolid:
			// send general matrix and color, end
			modelViewProjectionMat = currentSceneObject->modelMatrixStackPtr->modelViewProjectionMat;
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, rgba4[i].v);
			break;
		}
		a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uIndex, 1, &j);
		a3vertexDrawableActivateAndRender(drawable[j]);
	}

	// stop using stencil
	if (demoState->stencilTest)
		glDisable(GL_STENCIL_TEST);


	//-------------------------------------------------------------------------
	// OVERLAYS: done after FSQ so they appear over everything else
	//	- disable depth testing
	//	- draw overlays appropriately

	// enable alpha
	a3demo_enableCompositeBlending();

	// scene overlays
	if (demoState->displayGrid || demoState->displayTangentBases || demoState->displayWireframe)
	{
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
	}

	// overlays with no depth
	glDisable(GL_DEPTH_TEST);

	// hidden volumes
	if (demoState->displayHiddenVolumes)
	{

	}

	// superimpose axes
	// draw coordinate axes in front of everything
	currentDemoProgram = demoState->prog_drawColorAttrib;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3vertexDrawableActivate(demoState->draw_axes);

	// center of world from current viewer
	// also draw other viewer/viewer-like object in scene
	if (demoState->displayWorldAxes)
	{
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, viewProjectionMat.mm);
		a3vertexDrawableRenderActive();
	}

	// individual objects (based on scene graph)
	if (demoState->displayObjectAxes)
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

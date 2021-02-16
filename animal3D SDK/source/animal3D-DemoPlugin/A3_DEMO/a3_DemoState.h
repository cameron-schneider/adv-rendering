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
	
	a3_DemoState.h
	Demo state interface and programmer function declarations.

	********************************************
	*** THIS IS YOUR DEMO'S MAIN HEADER FILE ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOSTATE_H
#define __ANIMAL3D_DEMOSTATE_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"


//-----------------------------------------------------------------------------
// other demo includes

#include "_a3_demo_utilities/a3_DemoSceneObject.h"
#include "_a3_demo_utilities/a3_DemoShaderProgram.h"

#include "a3_DemoMode0_Intro.h"
#include "a3_DemoMode1_PostProc.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct	a3_DemoState					a3_DemoState;
typedef struct	a3_DemoModeCallbacks			a3_DemoModeCallbacks;
typedef enum	a3_DemoState_ModeName			a3_DemoState_ModeName;
typedef enum	a3_DemoState_TextDisplayName	a3_DemoState_TextDisplayName;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// demo mode names
enum a3_DemoState_ModeName
{
	demoState_modeIntro,			// starter scene
	demoState_modePostProc,			// post-processing scene

	demoState_mode_max
};


// text mode names
enum a3_DemoState_TextDisplayName
{
	demoState_textDisabled,			// no text overlay
	demoState_textControls,			// display controls
	demoState_textControls_gen,		// display general controls
	demoState_textData,				// display data

	demoState_text_max
};


// object maximum counts for easy array storage
// good idea to make these numbers greater than what you actually need 
//	and if you end up needing more just increase the count... there's 
//	more than enough memory to hold extra objects
enum a3_DemoState_ObjectMaxCount
{
	demoStateMaxCount_timer = 1,

	demoStateMaxCount_drawDataBuffer = 1,
	demoStateMaxCount_vertexArray = 4,
	demoStateMaxCount_drawable = 16,

	demoStateMaxCount_shaderProgram = 32,
	demoStateMaxCount_uniformBuffer = 8,

	demoStateMaxCount_texture = 32,

	demoStateMaxCount_framebuffer = 16,
};

	
//-----------------------------------------------------------------------------

// callback types
struct a3_DemoState;
typedef void (*a3_DemoMode_EventCallback)(a3_DemoState* demoState, void* demoMode, a3f64 const dt);
typedef void (*a3_DemoMode_InputCallback)(a3_DemoState* demoState, void* demoMode, a3i32 const button, a3i32 const state);

// callbacks pointers for each mode
struct a3_DemoModeCallbacks
{
	void* demoMode;
	a3_DemoMode_EventCallback handleInput, handleUpdate, handleRender;
	a3_DemoMode_InputCallback handleKeyPress, handleKeyHold;
};


// persistent demo state data structure
struct a3_DemoState
{
	//-------------------------------------------------------------------------
	// general variables pertinent to the state

	// terminate key pressed
	a3i32 exitFlag;

	// asset streaming between loads enabled (careful!)
	a3boolean streaming;

	// window and full-frame dimensions
	a3ui32 windowWidth, windowHeight;
	a3real windowWidthInv, windowHeightInv, windowAspect;
	a3ui32 frameWidth, frameHeight;
	a3real frameWidthInv, frameHeightInv, frameAspect;
	a3i32 frameBorder;


	//-------------------------------------------------------------------------
	// objects that have known or fixed instance count in the whole demo

	// text renderer
	a3boolean textInit;
	a3_DemoState_TextDisplayName textMode;
	a3_TextRenderer text[2];

	// input
	a3_MouseInput mouse[1];
	a3_KeyboardInput keyboard[1];
	a3_XboxControllerInput xcontrol[4];

	// pointer to fast trig table
	a3f32 trigTable[4096 * 4];

	// more accurate time tracking
	a3f64 t_timer, dt_timer, dt_timer_tot;
	a3i64 n_timer;


	//-------------------------------------------------------------------------
	// scene variables and objects

	// demo modes
	a3_DemoMode0_Intro demoMode0[1];
	a3_DemoMode1_PostProc demoMode1[1];
	a3_DemoState_ModeName demoMode;
	a3_DemoModeCallbacks demoModeCallbacks[demoState_mode_max];
	a3_DemoModeCallbacks const* demoModeCallbacksPtr;
	void* demoModePtr;

	// toggle grid in scene and axes superimposed, as well as other mods
	a3boolean displayGrid, displaySkybox, displayHiddenVolumes;
	a3boolean displayWorldAxes, displayObjectAxes;
	a3boolean displayTangentBases, displayWireframe;
	a3boolean updateAnimation;
	a3boolean stencilTest;
	a3boolean skipIntermediatePasses;


	//-------------------------------------------------------------------------
	// object arrays: organized as anonymous unions for two reasons: 
	//	1. easy to manage entire sets of the same type of object using the 
	//		array component
	//	2. at the same time, variables are named pointers


	// timers
	union {
		a3_Timer timer[demoStateMaxCount_timer];
		struct {
			a3_Timer
				timer_display[1];						// render FPS timer
		};
	};


	// draw data buffers
	union {
		a3_VertexBuffer drawDataBuffer[demoStateMaxCount_drawDataBuffer];
		struct {
			a3_VertexBuffer
				vbo_staticSceneObjectDrawBuffer[1];			// buffer to hold all data for static scene objects (e.g. grid)
		};
	};

	// vertex array objects
	union {
		a3_VertexArrayDescriptor vertexArray[demoStateMaxCount_vertexArray];
		struct {
			a3_VertexArrayDescriptor
				vao_tangentbasis_texcoord[1],				// VAO for vertex format with complete tangent basis, with texcoords
				vao_position_normal_texcoord[1];			// VAO for vertex format with complete tangent basis, with texcoords
			a3_VertexArrayDescriptor
				vao_position_color[1],						// VAO for vertex format with position and color
				vao_position[1];							// VAO for vertex format with only position
		};
	};

	// drawables
	union {
		a3_VertexDrawable drawable[demoStateMaxCount_drawable];
		struct {
			a3_VertexDrawable
				draw_grid[1],								// wireframe ground plane to emphasize scaling
				draw_axes[1];								// coordinate axes at the center of the world
			a3_VertexDrawable
				draw_unit_box[1],							// unit box (width = height = depth = 1)
				draw_unit_sphere[1],						// unit sphere (radius = 1)
				draw_unit_cylinder[1],						// unit cylinder (radius = length = 1)
				draw_unit_capsule[1],						// unit capsule (radius = length = 1)
				draw_unit_torus[1],							// unit torus (major radius = 1)
				draw_unit_cone[1],							// unit cone (radius = height = 1)
				draw_unit_plane_z[1];						// unit plane (width = height = 1) with Z normal
			a3_VertexDrawable
				draw_teapot[1];								// can't not have a Utah teapot
		};
	};


	// shader programs and uniforms
	union {
		a3_DemoStateShaderProgram shaderProgram[demoStateMaxCount_shaderProgram];
		struct {
			a3_DemoStateShaderProgram
				prog_transform_instanced[1],				// transform vertex only with instancing; no fragment shader
				prog_transform[1];							// transform vertex only; no fragment shader
			a3_DemoStateShaderProgram
				prog_drawColorAttrib_instanced[1],			// draw color attribute with instancing
				prog_drawColorUnif_instanced[1],			// draw uniform color with instancing
				prog_drawColorAttrib[1],					// draw color attribute
				prog_drawColorUnif[1];						// draw uniform color
			a3_DemoStateShaderProgram
				prog_drawPhong_instanced[1],				// draw Phong shading model with instancing
				prog_drawLambert_instanced[1],				// draw Lambert shading model with instancing
				prog_drawTexture_instanced[1],				// draw texture with instancing
				prog_drawPhong[1],							// draw Phong shading model
				prog_drawLambert[1],						// draw Lambert shading model
				prog_drawTexture[1];						// draw texture
			a3_DemoStateShaderProgram
				prog_drawTangentBasis_instanced[1],			// draw vertex/face tangent bases and wireframe with instancing
				prog_drawTangentBasis[1];					// draw vertex/face tangent bases and wireframe
			a3_DemoStateShaderProgram
				prog_postBright[1],							// post-processing bright pass (e.g. luminance)
				prog_postBlur[1],							// post-processing blur pass (e.g. Gaussian)
				prog_postBlend[1],							// post-processing blend pass (e.g. screen)
				prog_drawPhong_shadow_instanced[1],			// draw Phong shading model with shadow map, instanced
				prog_drawPhong_shadow[1];					// draw Phong shading model with shadow map
		};
	};

	// uniform buffers
	union {
		a3_UniformBuffer uniformBuffer[demoStateMaxCount_uniformBuffer];
		struct {
			a3_UniformBuffer
				ubo_light[1],								// uniform buffer for light data
				ubo_transform[1];							// uniform buffer for transformation data
		};
	};


	// textures
	union {
		a3_Texture texture[demoStateMaxCount_texture];
		struct {
			a3_Texture
				tex_earth_dm[1],
				tex_earth_sm[1],
				tex_earth_nm[1],
				tex_earth_hm[1],
				tex_mars_dm[1],
				tex_mars_sm[1],
				tex_mars_nm[1],
				tex_mars_hm[1],
				tex_stone_dm[1],
				tex_stone_nm[1],
				tex_stone_hm[1],
				tex_sun_dm[1],
				tex_skybox_clouds[1],
				tex_skybox_water[1],
				tex_ramp_dm[1],
				tex_ramp_sm[1],
				tex_testsprite[1],
				tex_checker[1];
		};
	};


	// ****TO-DO:
	//	-> uncomment framebuffers
/*	// framebuffers
	union {
		a3_Framebuffer framebuffer[demoStateMaxCount_framebuffer];
		struct {
			a3_Framebuffer
				fbo_c16x4[4],			// set of 16-bit color buffers (4 targets)
				fbo_c16_szHalf[3],		// set of half-screen-sized 16-bit color buffers
				fbo_c16_szQuarter[3],	// set of quarter-screen-sized 16-bit color buffers
				fbo_c16_szEighth[3],	// set of eighth-screen-sized 16-bit color buffers
				fbo_c32f[1],			// 32-bit float color buffer
				fbo_d32[1],				// 32-bit depth buffer
				fbo_c16x4_d24s8[1];		// 16-bit color buffer (4 targets) and depth-stencil buffer (24/8)
		};
	};*/


	// managed objects, no touchie
	a3_VertexDrawable dummyDrawable[1];


	//-------------------------------------------------------------------------
	// the end
};


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSTATE_H
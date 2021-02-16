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
	animal3D Math (A3DM) SDK
	By Daniel S. Buckstein

	animal3D-A3DM.c
	Compile A3DM module.
*/

// include configuration
#define A3_OPEN_SOURCE
#include "animal3D/a3/a3config.h"

// redefine inline tag as "extern inline" so it 
//	can be compiled and used inline simultaneously
#ifdef A3_INLINE
#undef A3_INLINE
#endif	// A3_INLINE
#define A3_INLINE extern inline

// also redefine global tag to be normal variable
#ifdef A3_GLOBAL
#undef A3_GLOBAL
#endif	// A3_GLOBAL
#define A3_GLOBAL

// include all headers and source
#include "animal3D-A3DM/animal3D-A3DM.h"
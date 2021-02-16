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
	
	a3_XboxControllerInput.c
	Basic Xbox controller input state tracking implementation.

	**DO NOT MODIFY THIS FILE**
*/


#if (defined _WINDOWS || defined _WIN32)


#include "animal3D/a3input/a3_XboxControllerInput.h"

#include <Windows.h>
#include <Xinput.h>


//-----------------------------------------------------------------------------

// utilities to help fix deadzoned controls
inline a3ubyte a3XboxControlInternalClampDeadzoneUChar(const a3ubyte value, const a3ubyte deadzone)
{
	return (value > deadzone ? value - deadzone : 0);
}

inline a3i16 a3XboxControlInternalClampDeadzoneShort(const a3i16 value, const a3i16 deadzone)
{
	return (value > deadzone ? value - deadzone : value < -deadzone ? value + deadzone + 1 : 0);
}


//-----------------------------------------------------------------------------

a3ret a3XboxControlSetRumble(a3_XboxControllerInput *control, const a3ui16 leftMotor, const a3ui16 rightMotor)
{
	if (control)
	{
		control->rumble[0] = leftMotor;
		control->rumble[1] = rightMotor;
		return (XInputSetState(control->id, (XINPUT_VIBRATION *)control->rumble) == ERROR_SUCCESS);
	}
	return a3input_invalid;
}


a3ret a3XboxControlUpdate(a3_XboxControllerInput *control)
{
	static const a3f64 invTriggerThreshold = 1.0 / (a3f64)(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	static const a3f64 invLThumbThreshold = 1.0 / (a3f64)(32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	static const a3f64 invRThumbThreshold = 1.0 / (a3f64)(32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

	if (control)
	{
		control->ctrl0 = control->ctrl;
		control->connected0 = control->connected;
		control->connected = XInputGetState(control->id, (XINPUT_STATE *)&control->ctrl) == ERROR_SUCCESS;
		if (control->connected)
		{
			// clamp to dead zones
			control->ctrl.lTrigger = a3XboxControlInternalClampDeadzoneUChar(control->ctrl.lTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			control->ctrl.rTrigger = a3XboxControlInternalClampDeadzoneUChar(control->ctrl.rTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			control->ctrl.lThumbX = a3XboxControlInternalClampDeadzoneShort(control->ctrl.lThumbX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			control->ctrl.lThumbY = a3XboxControlInternalClampDeadzoneShort(control->ctrl.lThumbY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			control->ctrl.rThumbX = a3XboxControlInternalClampDeadzoneShort(control->ctrl.rThumbX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			control->ctrl.rThumbY = a3XboxControlInternalClampDeadzoneShort(control->ctrl.rThumbY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

			// normalize
			control->ctrl.lTrigger_unit = control->ctrl.lTrigger * invTriggerThreshold;
			control->ctrl.rTrigger_unit = control->ctrl.rTrigger * invTriggerThreshold;
			control->ctrl.lThumbX_unit = control->ctrl.lThumbX * invLThumbThreshold;
			control->ctrl.lThumbY_unit = control->ctrl.lThumbY * invLThumbThreshold;
			control->ctrl.rThumbX_unit = control->ctrl.rThumbX * invRThumbThreshold;
			control->ctrl.rThumbY_unit = control->ctrl.rThumbY * invRThumbThreshold;

			// done
			return 1;
		}
		return 0;
	}
	return a3input_invalid;
}


//-----------------------------------------------------------------------------


#endif	// (defined _WINDOWS || defined _WIN32)
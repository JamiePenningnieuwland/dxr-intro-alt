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

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <functional>

namespace Window
{
	struct KeyEvent
	{
		std::function<void(int&)> keyPressed;
		std::function<void(int&)> KeyReleased;
	};
	static KeyEvent myEvent;
	HRESULT Create(LONG width, LONG height, HINSTANCE& instance, HWND &window, LPCWSTR title);
}
namespace Input
{
	enum eKey
	{	
	
		//Keyboard----------
		EGO_BACK = 0x08,
		EGO_TAB = 0x09,
		EGO_ENTER = 0x0D,
		EGO_SHIFT = 0x10,
		EGO_CONTROL = 0x11,
		EGO_ALT = 0x12,
		EGO_PAUSE = 0x13,
		EGO_CAPS = 0x14,
		EGO_ESC = 0x1B,
		EGO_SPACE = 0x20,
		EGO_PRIOR = 0x21,
		EGO_NEXT = 0x22,
		EGO_END = 0x23,
		EGO_HOME = 0x24,
		EGO_LEFT = 0x25,
		EGO_UP = 0x26,
		EGO_RIGHT = 0x27,
		EGO_DOWN = 0x28,
		EGO_SELECT = 0x29,
		EGO_PRINT = 0x2A,
		EGO_PRINTSCREEN = 0x2C,
		EGO_INSERT = 0x2D,
		EGO_DELETE = 0x2E,
		EGO_0 = 0x30,
		EGO_1 = 0x31,
		EGO_2 = 0x32,
		EGO_3 = 0x33,
		EGO_4 = 0x34,
		EGO_5 = 0x35,
		EGO_6 = 0x36,
		EGO_7 = 0x37,
		EGO_8 = 0x38,
		EGO_9 = 0x39,
		EGO_A = 0x41,
		EGO_B = 0x42,
		EGO_C = 0x43,
		EGO_D = 0x44,
		EGO_E = 0x45,
		EGO_F = 0x46,
		EGO_G = 0x47,
		EGO_H = 0x48,
		EGO_I = 0x49,
		EGO_J = 0x4A,
		EGO_K = 0x4B,
		EGO_L = 0x4C,
		EGO_M = 0x4D,
		EGO_N = 0x4E,
		EGO_O = 0x4F,
		EGO_P = 0x50,
		EGO_Q = 0x51,
		EGO_R = 0X52,
		EGO_S = 0x53,
		EGO_T = 0X54,
		EGO_U = 0x55,
		EGO_V = 0x56,
		EGO_W = 0x57,
		EGO_X = 0x58,
		EGO_Y = 0x59,
		EGO_Z = 0x5A,
		EGO_LSHIFT = 0xA0,
		EGO_RSHIFT = 0xA1,
		EGO_LCONTROL = 0xA2,
		EGO_RCONTORL = 0xA3,
		EGO_LALT = 0xA4,
		EGO_RALT = 0xA5
	};
	void Init();
	bool KeyPressed(eKey key);
}
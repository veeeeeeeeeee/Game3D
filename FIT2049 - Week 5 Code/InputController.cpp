/*	FIT2049 - Example Code
*	InputController.cpp
*	Created by Elliott Wilson - 2015 - Monash Univeristy
*	Implementation of InputController.h
*/

#include "InputController.h"

InputController::InputController(HWND windowHandle)
{
	m_windowHandle = windowHandle;						//In order to work out the client coords for the mouse we need a window handle

	memset(m_PrevKeys, 0, sizeof(m_PrevKeys));			//Clear out all of those arrays!
	memset(m_CurrentKeys, 0, sizeof(m_CurrentKeys));
	memset(m_prevMouse, 0, sizeof(m_prevMouse));
	memset(m_currentMouse, 0, sizeof(m_currentMouse));

	m_mouseX = 0;
	m_mouseY = 0;

	RECT screen;
	GetWindowRect (m_windowHandle, &screen);

	POINT temp_ori;
	temp_ori.x = (screen.right-screen.left)/2;
	temp_ori.y = (screen.bottom-screen.top)/2;
	SetCursorPos (temp_ori.x, temp_ori.y);

	//ScreenToClient (m_windowHandle, &temp_ori);
	m_oriMouseX = temp_ori.x;
	m_oriMouseY = temp_ori.y;

	ShowCursor (false);
}

void InputController::SetKeyDown(UINT keyCode)
{
	m_CurrentKeys[keyCode] = true;
}

void InputController::SetKeyUp(UINT keyCode)
{
	m_CurrentKeys[keyCode] = false;
}

void InputController::SetMouseDown(int mouseButton)
{
	m_currentMouse[mouseButton] = true;
}

void InputController::SetMouseUp(int mouseButton)
{
	m_currentMouse[mouseButton] = false;
}

void InputController::SetMouseDeltaX(long deltaX)
{
	m_mouseDeltaX = deltaX;
}
void InputController::SetMouseDeltaY(long deltaY)
{
	m_mouseDeltaY = deltaY;
}

bool InputController::GetKeyDown(UINT keyCode)
{
	return m_CurrentKeys[keyCode];
}

bool InputController::GetKeyUp(UINT keyCode)
{
	return m_PrevKeys[keyCode] && !m_CurrentKeys[keyCode];
}

bool InputController::GetMouseDown(int mouseButton)
{
	return m_currentMouse[mouseButton];
}

bool InputController::GetMouseUp(int mouseButton)
{
	return m_prevMouse[mouseButton] && !m_currentMouse[mouseButton];
}

int InputController::GetMouseX()
{
	return m_mouseX;
}

int InputController::GetMouseY()
{
	return m_mouseY;
}

int InputController::GetMouseXDelta()
{
	//return m_mouseX - m_prevMouseX;
	return m_mouseDeltaX;
}

int InputController::GetMouseYDelta()
{
	//return m_mouseY - m_prevMouseY;
	return m_mouseDeltaY;
}

int InputController::getOriMouseX() {
	return m_oriMouseX;
}

int InputController::getOriMouseY() {
	return m_oriMouseY;
}

void InputController::BeginUpdate()
{
	POINT pt;								//This POINT struct holds the x and y of the mouse cursor
	GetCursorPos(&pt);						//Populate the POINT struct
	ScreenToClient(m_windowHandle, &pt);	//The GetCursorPos gives you screen coords, client coords are probably better.
	m_mouseX = pt.x;
	m_mouseY = pt.y;
}

void InputController::EndUpdate()
{
	//When we finish an update we copy the values of the current arrays into the last arrays
	memcpy(m_PrevKeys, m_CurrentKeys, sizeof(m_CurrentKeys));
	memcpy(m_prevMouse, m_currentMouse, sizeof(m_currentMouse));
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;

	POINT resetMouse;
	resetMouse.x = m_oriMouseX;
	resetMouse.y = m_oriMouseY;
	ClientToScreen (m_windowHandle, &resetMouse);

	POINT currentMouse;
	currentMouse.x = m_mouseX;
	currentMouse.y = m_mouseY;
	ClientToScreen (m_windowHandle, &currentMouse);

	if (m_mouseX != m_oriMouseX)
		SetCursorPos (resetMouse.x, currentMouse.y);

	if (m_mouseY != m_oriMouseY)
		SetCursorPos (currentMouse.x, resetMouse.y);
}
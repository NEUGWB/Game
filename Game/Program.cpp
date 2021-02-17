#include <windows.h>        
#include <glad/glad.h>
#include <iostream>

#include "Game.h"
#include "BreakOutPostProcess.h"


HDC         hDC = NULL;
HGLRC       hRC = NULL;
HWND        hWnd = NULL;
HINSTANCE   hInstance;

bool    keys[256];
bool    active = TRUE;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height==0)
	{
		height = 1;
	}

	glViewport(0, 0, width, height);
}

int InitGL(GLvoid)
{
	return TRUE;
}

GLvoid KillGLWindow(GLvoid)
{
	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			printf("Release Of DC And RC Failed. ""SHUTDOWN ERROR\n");
		}

		if (!wglDeleteContext(hRC))
		{
			printf("Release Rendering Context Failed. ""SHUTDOWN ERROR\n");
		}
		hRC = NULL;
	}

	if (hDC && !ReleaseDC(hWnd, hDC))
	{
		printf("Release Device Context Failed. ""SHUTDOWN ERROR\n");
		hDC = NULL;
	}

	if (hWnd && !DestroyWindow(hWnd))
	{
		printf("Could Not Release hWnd. ""SHUTDOWN ERR\n");
		hWnd = NULL;
	}

	if (!UnregisterClassA("OpenGL", hInstance))
	{
		printf("Could Not Unregister Class. ""SHUTDOWN ERROR\n");
		hInstance = NULL;
	}
}

void *GetAnyGLFuncAddress(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);
	if (p == 0 ||
		(p == (void *)0x1) || (p == (void *)0x2) || (p == (void *)0x3) ||
		(p == (void *)-1))
	{
		HMODULE m = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(m, name);
	}

	return p;
}

void *myloader(const char *name)
{
	void *ret = GetAnyGLFuncAddress(name);
	if (!ret)
	{
		printf("Load %s fail, %d\n", name, GetLastError());
	}
	return ret;
}




BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint      PixelFormat;
	WNDCLASSA    wc;
	DWORD       dwExStyle;
	DWORD       dwStyle;
	RECT        WindowRect;
	WindowRect.left = (long)0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)height;

	hInstance = GetModuleHandle(NULL);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "OpenGL";

	if (!RegisterClassA(&wc))
	{
		printf("Failed To Register The Window Class. ""ERROR\n");
		return FALSE;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;
	dwStyle &= ~(WS_SIZEBOX);
	dwStyle &= ~(WS_MAXIMIZEBOX);

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);


	if (!(hWnd = CreateWindowExA(dwExStyle,
		"OpenGL",
		"abc",
		dwStyle |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN,
		0, 0,
		WindowRect.right-WindowRect.left,
		WindowRect.bottom-WindowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL)))
	{
		KillGLWindow();
		printf("Window Creation Error. ""ERROR\n");
		return FALSE;
	}

	static  PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	if (!(hDC = GetDC(hWnd)))
	{
		KillGLWindow();
		printf("GetDC\n");
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
	{
		KillGLWindow();
		printf("ChoosePixelFormat\n");
		return FALSE;
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))
	{
		KillGLWindow();
		printf("SetPixelFormat\n");
		return FALSE;
	}

	if (!(hRC = wglCreateContext(hDC)))
	{
		KillGLWindow();
		printf("wglCreateContext\n");
		return FALSE;
	}

	if (!wglMakeCurrent(hDC, hRC))
	{
		KillGLWindow();
		printf("wglMakeCurrent\n");
		return FALSE;
	}

	if (!gladLoadGLLoader((GLADloadproc)myloader))
	{
		return -1;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ReSizeGLScene(width, height);

	const GLubyte *b = glGetString(GL_VERSION);
	printf("gl version is %s\n", b);

	if (!InitGL())
	{
		KillGLWindow();
		printf("InitGL\n");
		return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    uMsg,
	WPARAM  wParam,
	LPARAM  lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
	{
		if (!HIWORD(wParam))
		{
			active = TRUE;
		} else
		{
			active = FALSE;
		}

		return 0;
	}

	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN:
	{
		keys[wParam] = TRUE;
		Game::GetInstance()->Keys[wParam] = true;
		return 0;
	}

	case WM_KEYUP:
	{
		keys[wParam] = FALSE;
		Game::GetInstance()->Keys[wParam] = false;
		return 0;
	}

	case WM_SIZE:
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	}


	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

unsigned __int64 update = 0, secUpdate = 0, frame = 0;
int main()
{
	MSG     msg;
	BOOL    done = FALSE;
	timeBeginPeriod(1);

	if (!CreateGLWindow((char *)"Game", 800, 600, 16, false))
	{
		return 0;
	}
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GEQUAL);
	GameInit();
	Game::GetInstance()->Init();
	secUpdate = update = timeGetTime();
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//DEBUG_PRINT("%d %d %d\n", msg.message, msg.wParam, msg.lParam);
			if (msg.message==WM_QUIT)
			{
				done = TRUE;
			} else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (keys[VK_ESCAPE])
		{
			done = TRUE;
		} else
		{
			frame++;
			ULONGLONG now = timeGetTime();
			float fdelta = (now - update) / 1000.0f;
			if (now - secUpdate >= 1000)
			{
				float ftime = (now - secUpdate) / 1000.f;
				Game::GetInstance()->SetFrameRate(frame / ftime, ftime);
				secUpdate = now;
				frame = 0;
			}
			update = now;

			Game::GetInstance()->ProcessInput(fdelta);
			Game::GetInstance()->Update(fdelta);
			Game::GetInstance()->Render();

			SwapBuffers(hDC);
			//Sleep(5);
		}
	}

	KillGLWindow();
	return (msg.wParam);
}

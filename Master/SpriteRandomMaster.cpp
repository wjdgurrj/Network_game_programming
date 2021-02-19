#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>

#include "ddutil.h"

#include <dsound.h>
#include "dsutil.h"


#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey ))

HWND MainHwnd;

LPDIRECTDRAW         DirectOBJ;
LPDIRECTDRAWSURFACE  RealScreen;
LPDIRECTDRAWSURFACE  BackScreen;
LPDIRECTDRAWSURFACE  SpriteImage;
LPDIRECTDRAWSURFACE  SpriteImage2;
LPDIRECTDRAWSURFACE  BackGround;
LPDIRECTDRAWSURFACE  END;
LPDIRECTDRAWSURFACE  Player1W;
LPDIRECTDRAWSURFACE  Player2W;
LPDIRECTDRAWSURFACE  BackGround1;


LPDIRECTDRAWCLIPPER   ClipScreen;

int gFullScreen = 0, Click = 0, Click1 = 0;
int gWidth = 640 * 2, gHeight = 480;
int MouseX = 100, MouseY = gHeight / 2;
int MouseX2 = 100;
int MouseY2 = gHeight / 2;
int direction = -1, direction2 = -1;
int Step = 5;
int stop = 0, stop2 = 0;
int life = 3, life2 = 3;
int gamemode = 0, gamemode2 = 0;

////////////////////

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

HSNDOBJ Sound[10];


BOOL _InitDirectSound(void)
{
	if (DirectSoundCreate(NULL, &SoundOBJ, NULL) == DS_OK)
	{
		if (SoundOBJ->SetCooperativeLevel(MainHwnd, DSSCL_PRIORITY) != DS_OK) return FALSE;

		memset(&DSB_desc, 0, sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc, &SoundDSB, NULL) != DS_OK) return FALSE;
		SoundDSB->SetVolume(DSBVOLUME_MAX); // DSBVOLUME_MIN
		SoundDSB->SetPan(DSBPAN_RIGHT);
		return TRUE;
	}
	return FALSE;
}

void _Play(int num)
{
	SndObjPlay(Sound[num], NULL);
}

void _Stop(int num) {
	SndObjStop(Sound[num]);
}
////////////////////////


BOOL Fail(HWND hwnd)
{
	ShowWindow(hwnd, SW_HIDE);
	MessageBox(hwnd, "DIRECT X 초기화에 실패했습니다.", "게임 디자인", MB_OK);
	DestroyWindow(hwnd);
	return FALSE;
}

void _ReleaseAll(void)
{
	if (DirectOBJ != NULL)
	{
		if (RealScreen != NULL)
		{
			RealScreen->Release();
			RealScreen = NULL;
		}
		if (SpriteImage != NULL)
		{
			SpriteImage->Release();
			SpriteImage = NULL;
		}
		if (SpriteImage2 != NULL)
		{
			SpriteImage2->Release();
			SpriteImage2 = NULL;
		}
		if (BackGround != NULL)
		{
			BackGround->Release();
			BackGround = NULL;
		}
		DirectOBJ->Release();
		DirectOBJ = NULL;
	}
}

long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	

	switch (message)
	{
		/*
			  case    WM_MOUSEMOVE    :   MouseX = LOWORD(lParam);
								   MouseY = HIWORD(lParam);
								   break;

			 case   WM_LBUTTONDOWN   :    Click=1;
							 _Play(3);
							 break;
							 */
	case   WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
		case VK_F12:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;

		case VK_RIGHT:
			return 0;

		case VK_DOWN:
			return 0;

		case VK_LEFT:
			return 0;

		case VK_UP:
			return 0;

		case VK_SPACE:
			_Play(7);
			Click = 1;
			break;
		}
		break;

	case    WM_DESTROY:  _ReleaseAll();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL _GameMode(HINSTANCE hInstance, int nCmdShow, int x, int y, int bpp)
{
	HRESULT result;
	WNDCLASS wc;
	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;
	LPDIRECTDRAW pdd;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "TEST";
	RegisterClass(&wc);


	if (gFullScreen) {
		if ((MainHwnd = CreateWindowEx(0, "TEST", NULL, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL)) == NULL)
			ExitProcess(1);
	}
	else {
		if ((MainHwnd = CreateWindow("TEST", "1Player", WS_OVERLAPPEDWINDOW, 0, 0, x,
			y, NULL, NULL, hInstance, NULL)) == NULL)
			ExitProcess(1);
		SetWindowPos(MainHwnd, NULL, 100, 100, x, y, SWP_NOZORDER);
	}

	SetFocus(MainHwnd);
	ShowWindow(MainHwnd, nCmdShow);
	UpdateWindow(MainHwnd);
	//    ShowCursor( FALSE );

	result = DirectDrawCreate(NULL, &pdd, NULL);
	if (result != DD_OK) return Fail(MainHwnd);

	result = pdd->QueryInterface(IID_IDirectDraw, (LPVOID *)&DirectOBJ);
	if (result != DD_OK) return Fail(MainHwnd);


	// 윈도우 핸들의 협력 단계를 설정한다.
	if (gFullScreen) {
		result = DirectOBJ->SetCooperativeLevel(MainHwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
		if (result != DD_OK) return Fail(MainHwnd);

		result = DirectOBJ->SetDisplayMode(x, y, bpp);
		if (result != DD_OK) return Fail(MainHwnd);

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;

		result = DirectOBJ->CreateSurface(&ddsd, &RealScreen, NULL);
		if (result != DD_OK) return Fail(MainHwnd);

		memset(&ddscaps, 0, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		result = RealScreen->GetAttachedSurface(&ddscaps, &BackScreen);
		if (result != DD_OK) return Fail(MainHwnd);
	}
	else {
		result = DirectOBJ->SetCooperativeLevel(MainHwnd, DDSCL_NORMAL);
		if (result != DD_OK) return Fail(MainHwnd);

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 0;

		result = DirectOBJ->CreateSurface(&ddsd, &RealScreen, NULL);
		if (result != DD_OK) return Fail(MainHwnd);

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = x;
		ddsd.dwHeight = y;
		result = DirectOBJ->CreateSurface(&ddsd, &BackScreen, NULL);
		if (result != DD_OK) return Fail(MainHwnd);

		result = DirectOBJ->CreateClipper(0, &ClipScreen, NULL);
		if (result != DD_OK) return Fail(MainHwnd);

		result = ClipScreen->SetHWnd(0, MainHwnd);
		if (result != DD_OK) return Fail(MainHwnd);

		result = RealScreen->SetClipper(ClipScreen);
		if (result != DD_OK) return Fail(MainHwnd);

		SetWindowPos(MainHwnd, NULL, 100, 100, x, y, SWP_NOZORDER | SWP_NOACTIVATE);
	}


	return TRUE;
}


extern void CommInit(int argc, char **argv);
extern void CommSend(char *sending);
extern void CommRecv(char *recvData);


void CALLBACK _GameProc(HWND hWnd, UINT message, UINT wParam, DWORD lParam)
{
	RECT BackRect = { 0, 0, 640, 480 };
	RECT DispRect = { 0, 0, gWidth, gHeight };
	RECT SpriteRect, dstRect, WinRect;
	char sendData[200];
	int Step = 3;
	int winner = 0;
	if (winner == 0) { //승패 결정 안났을 때 배경설정
		BackScreen->BltFast(0, 0, BackGround, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround1, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}
	// player 1 
	static int Frame = 0;
	static int Count = 0;
	SpriteRect.left = Frame * 100;
	SpriteRect.top = 210;
	SpriteRect.right = SpriteRect.left + 100;
	SpriteRect.bottom = 280;
	if (Click) {                        
		MouseX += Step;
		if (Count > 2) {
			if (Frame >= 5)
			{
				Frame = 0;
				Click = 0;

			}
			else
			{
				Frame++;
			}
			Count = 0;
		}
		else
		{
			Count++;
		}
	}
	if (MouseX <= 50) MouseX = 50;
	if (MouseY <= 35) MouseY = 35;
	if (MouseY > gHeight - 35) MouseY = gHeight - 35;

	if (MouseX > gWidth - 50 && winner == 0)
	{
		MouseX = gWidth - 50;
		BackGround1 = DDLoadBitmap(DirectOBJ, "Player1W.BMP", 0, 0); //player1이 이겼을 시 배경적용
		BackGround = DDLoadBitmap(DirectOBJ, "END.BMP", 0, 0);
		winner = 1;
		sprintf_s(sendData, "11");
		CommSend(sendData);

	}

	BackScreen->BltFast(MouseX - 50, MouseY + 130, SpriteImage, &SpriteRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	sprintf_s(sendData, "1,%d,%d,%d,%d,%d,%d,%d,%d", MouseX - 50, MouseY + 130, MouseX + 50, MouseY + 190, SpriteRect.left, SpriteRect.top, SpriteRect.right, SpriteRect.bottom);
	CommSend(sendData);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //player 2
	static int Frame2 = 0;
	static int Count2 = 0;
	SpriteRect.left = Frame2 * 100;
	SpriteRect.top = 210;
	SpriteRect.right = SpriteRect.left + 100;
	SpriteRect.bottom = 280;
	if (Click1) {
		MouseX2 += Step;

		if (Count2 > 2) {
			if (Frame2 >= 5)
			{

				Frame2 = 0;
				Click1 = 0;

			}
			else
			{
				Frame2++;
			}
			Count2 = 0;
		}
		else
		{
			Count2++;
		}
	}
	if (MouseX2 <= 50) MouseX2 = 50;
	if (MouseX2 > gWidth - 50 && winner == 0)
	{
		MouseX2 = gWidth - 50;
		BackGround1 = DDLoadBitmap(DirectOBJ, "Player2W.BMP", 0, 0);
		BackGround = DDLoadBitmap(DirectOBJ, "END.BMP", 0, 0); //player2가 이겼을 시 배경적용
		winner = 2;
		sprintf_s(sendData, "12");
		CommSend(sendData);
		
	}
	if (MouseY2 <= 35) MouseY2 = 35;
	if (MouseY2 > gHeight - 35) MouseY2 = gHeight - 35;

	BackScreen->BltFast(MouseX2 - 50, MouseY2 + 40, SpriteImage2, &SpriteRect, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	sprintf_s(sendData, "2,%d,%d,%d,%d,%d,%d,%d,%d", MouseX2 - 50, MouseY2 + 40, MouseX2 + 50, MouseY2 + 100, SpriteRect.left, SpriteRect.top, SpriteRect.right, SpriteRect.bottom);
	CommSend(sendData);

	if (gFullScreen)
		RealScreen->Flip(NULL, DDFLIP_WAIT);
	else {
		GetWindowRect(MainHwnd, &WinRect);
		RealScreen->Blt(&WinRect, BackScreen, &DispRect, DDBLT_WAIT, NULL);
	}
}
void _GameProcDraw(char *recvData) // 서로 통신을 하기 위하여 gameprocdraw 필요
{
	RECT BackRect = { 0, 0, 640, 480 };
	RECT DispRect = { 0, 0, gWidth, gHeight };
	RECT SpriteRect, dstRect, WinRect;
	char sendData[200];
	int type = 0;

	sscanf(recvData, "%d", &type); 
	if (type == 10) // slave에서 10 입력이 들어오면 실행
	{
		sprintf_s(sendData, "10");
		CommSend(sendData);
		Click1 = 1;  
	}
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!_GameMode(hInstance, nCmdShow, gWidth, gHeight, 32)) return FALSE;

	SpriteImage = DDLoadBitmap(DirectOBJ, "CUTEBABY.BMP", 0, 0);
	SpriteImage2 = DDLoadBitmap(DirectOBJ, "CUTEBABYY.BMP", 0, 0);
	BackGround1 = DDLoadBitmap(DirectOBJ, "BACKGROUND.BMP", 0, 0);
	BackGround = DDLoadBitmap(DirectOBJ, "BACKGROUND.BMP", 0, 0);
	END = DDLoadBitmap(DirectOBJ, "END.BMP", 0, 0);
	Player1W = DDLoadBitmap(DirectOBJ, "Player1W.BMP", 0, 0);
	Player2W = DDLoadBitmap(DirectOBJ, "Player2W.BMP", 0, 0);

	DDSetColorKey(END, RGB(0, 0, 0));
	DDSetColorKey(SpriteImage, RGB(0, 0, 0));
	DDSetColorKey(SpriteImage2, RGB(0, 0, 0));

	SetTimer(MainHwnd, 1, 30, _GameProc);

	CommInit(NULL, NULL);

	///////////////////

	if (_InitDirectSound())
	{
		Sound[0] = SndObjCreate(SoundOBJ, "BACKSOUND.WAV", 1);
		Sound[7] = SndObjCreate(SoundOBJ, "UPDOWN.WAV", 2);

		SndObjPlay(Sound[0], DSBPLAY_LOOPING);
	}
	//////////////////

	while (!_GetKeyState(VK_ESCAPE))
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0)) return msg.wParam;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//        else _GameProc();
	}
	DestroyWindow(MainHwnd);

	return TRUE;
}
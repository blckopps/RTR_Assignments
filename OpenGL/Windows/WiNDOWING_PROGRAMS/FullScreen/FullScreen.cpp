#include<Windows.h>


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

//GLOBAL VARIABLES
bool bFullScreen=false;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
HWND ghwnd=NULL;


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[]=TEXT("MYWINDOW ");
	wndclass.cbSize=sizeof(WNDCLASSEX);
	wndclass.style=CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.lpfnWndProc=WndProc;
	wndclass.hInstance=hInstance;
	wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName=szAppName;
	wndclass.lpszMenuName=NULL;
	wndclass.hIconSm=LoadIcon(NULL,IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	//create window

	hwnd=CreateWindow(szAppName,TEXT("My FullScreen_Window-SHUBHAM"),
									WS_OVERLAPPEDWINDOW,
									100,
									100,
									800,
									600,
									NULL,
									NULL,
									hInstance,
									NULL);
	ghwnd=hwnd;

	ShowWindow(hwnd,iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return((int)msg.wParam);


}
LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	//FUnction Declarations
	void toogle_screen(void);

	switch(iMsg)
	{	

		
	case WM_KEYDOWN:

		switch(wParam)
		{
		case 0x46:
		//	MessageBox(hwnd,TEXT("P BUTTON PRESSED!!"),TEXT("BUTTON P"),MB_OK);
			toogle_screen();
			
			
			
		

			break;
		}

		break;



	case WM_DESTROY:
		//MessageBox(hwnd,TEXT("This is WM_DESTROY!!"),TEXT("In Wm_DESTROY"),MB_OK);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}
//User Defined Functions

void toogle_screen(void)
{
	//MONITORINFO mi;
	

	if(bFullScreen==false)
	{
		dwstyle=GetWindowLong(ghwnd, GWL_STYLE);

		if(dwstyle & WS_OVERLAPPEDWINDOW)
		{
			MONITORINFO	mi= {sizeof(MONITORINFO)};
			
			if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE,dwstyle&~WS_OVERLAPPEDWINDOW);

				SetWindowPos(ghwnd, 
							 HWND_TOP,
							 mi.rcMonitor.left,
							 mi.rcMonitor.top,
							 mi.rcMonitor.right-mi.rcMonitor.left,
							 mi.rcMonitor.bottom-mi.rcMonitor.top,
							 SWP_NOZORDER | SWP_FRAMECHANGED );
					
			}	
		}
		ShowCursor(FALSE);
		bFullScreen=true;
	}
	else
	{
		SetWindowLong(ghwnd,
					  GWL_STYLE,
					  dwstyle | 
					  WS_OVERLAPPEDWINDOW);
			
		SetWindowPlacement(ghwnd,&wpPrev);

		SetWindowPos(ghwnd,
					HWND_TOP,
					0,0,0,0,
					SWP_NOZORDER |
					SWP_FRAMECHANGED |
					SWP_NOMOVE | 
					SWP_NOSIZE|
					SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
		bFullScreen=false;
	}
}
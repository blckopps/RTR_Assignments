#include<Windows.h>


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

bool isFullScreen=false;
DWORD dwstyle;

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
	

	DEVMODE dm,dm2;

		ZeroMemory(&dm, sizeof(dm));
		dm.dmSize=sizeof(dm);
		
		ZeroMemory(&dm2, sizeof(dm2));
		dm2.dmSize=sizeof(dm2);
		
		DWORD Current_Width;
		DWORD Current_Height;

		dwstyle=GetWindowLong(hwnd, GWL_STYLE);
		
		if(0 != EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS,&dm))
				{
					 Current_Width=dm.dmPelsWidth;
					 Current_Height=dm.dmPelsHeight;
					
				}
		/*		
		int def_w=GetSystemMetrics(SM_CXSCREEN);
		int def_h=GetSystemMetrics(SM_CYSCREEN);
		*/
	switch(iMsg)
	{	

		

	case WM_KEYDOWN:

		switch(wParam)
		{
		case 0x45:
			DestroyWindow(hwnd);
			break;


		case 0x46:
			
		
			if(isFullScreen == false)
			{
				dm2.dmPelsWidth=800;
				dm2.dmPelsHeight=600;
				//dm2.dmFields=
				
				


				//dm.dmPelsWidth=Current_Width;
				//dm.dmPelsHeight=Current_Height;
				
				long result=ChangeDisplaySettings(&dm2,CDS_FULLSCREEN | CDS_RESET);

				if(DISP_CHANGE_SUCCESSFUL != result)
				{

					MessageBox(hwnd,TEXT("ERROR"),TEXT("YEAH!!!"),MB_OK);
				}
				SetWindowLong(hwnd, GWL_STYLE,dwstyle&~WS_OVERLAPPEDWINDOW);
				ShowWindow(hwnd,SW_SHOWMAXIMIZED);
				isFullScreen=true;
			}
			else
			{
				
				
				ChangeDisplaySettings(&dm,CDS_FULLSCREEN | CDS_RESET);
				SetWindowLong(hwnd, GWL_STYLE,dwstyle | WS_OVERLAPPEDWINDOW);

				ShowWindow(hwnd,SW_SHOWNORMAL);
				/*
				dm.dmPelsWidth=Current_Width;
				dm.dmPelsHeight=Current_Height;
				ChangeDisplaySettings(&dm,0);
				*/
				isFullScreen=false;
			}
			
			
			//int def_w=GetSystemMetrics(SM_CXSCREEN);
			//int def_h=GetSystemMetrics(SM_CYSCREEN);
			
			/*
			rset and fullscreen
			TCHAR str[255];
			wsprintf(str,TEXT("DEFAULT %d BY %d"),def_w,def_h);
			MessageBox(hwnd,str,TEXT("df"),MB_OK);			
			
			TCHAR str1[255];
			wsprintf(str1,TEXT("CURRENT %d BY %d"),Current_Width,Current_Height);
			MessageBox(hwnd,str1,TEXT("df"),MB_OK);
			*/
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

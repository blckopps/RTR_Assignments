#include<Windows.h>


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

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

	hwnd=CreateWindow(szAppName,TEXT("My WINDOW APP-SHUBHAM"),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL);

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
	
	switch(iMsg)
	{	

	case WM_CREATE:
		MessageBox(hwnd,TEXT("This is WM_CREATE!!"),TEXT("In Wm_Create"),MB_OK);
		break;

		
	case WM_KEYDOWN:

		switch(wParam)
		{
		case 0x46:
			MessageBox(hwnd,TEXT("P BUTTON PRESSED!!"),TEXT("BUTTON P"),MB_OK);
			break;

		case VK_ESCAPE:
			MessageBox(hwnd,TEXT("ESCAPE Button Was Pressed!!"),TEXT("ESCAPE Button "),MB_OK);
			DestroyWindow(hwnd);
			break;

			
		}

		break;

	case WM_LBUTTONDOWN:
		MessageBox(hwnd,TEXT("Mouse L Button Was Pressed!!"),TEXT("Mouse L Button"),MB_OK);
		break;
	case WM_RBUTTONDOWN:
		MessageBox(hwnd,TEXT("Mouse R Button Was Pressed!!"),TEXT("Mouse R Button"),MB_OK);
		break;


	case WM_DESTROY:
		MessageBox(hwnd,TEXT("This is WM_DESTROY!!"),TEXT("In Wm_DESTROY"),MB_OK);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

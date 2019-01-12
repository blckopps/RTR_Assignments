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

	hwnd=CreateWindow(szAppName,TEXT("PlainDC-SHUBHAM"),
									WS_OVERLAPPEDWINDOW,
									100,
									100,
									600,
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
		HDC hdc;
		RECT rc;
		TCHAR str[255]=TEXT("HELLO WORLD!!!");

	switch(iMsg)
	{	
	
		
	case WM_RBUTTONDOWN:
			
			GetClientRect(hwnd,&rc);
			hdc=GetDC(hwnd);

			SetBkColor(hdc,RGB(0,0,0));
			SetTextColor(hdc,RGB(0,255,0));
			DrawText(hdc, str, -1, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		
			ReleaseDC(hwnd,hdc);
			break;
	
	case WM_DESTROY:
		//MessageBox(hwnd,TEXT("This is WM_DESTROY!!"),TEXT("In Wm_DESTROY"),MB_OK);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}


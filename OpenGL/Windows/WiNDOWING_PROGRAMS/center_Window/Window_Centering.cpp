#include<Windows.h>


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow)
{
	int def_h,def_w;
	//int const x,y;
	
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[]=TEXT("MYWINDOW");
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
	def_h=GetSystemMetrics(SM_CXSCREEN);
	def_w=GetSystemMetrics(SM_CYSCREEN);
	int const x=(def_h-800)/2;
	int const y=(def_w-600)/2;

	hwnd=CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
						szAppName,
						TEXT("CENTER_Window-SHUBHAM"),
									WS_OVERLAPPEDWINDOW,
									x,
									y,
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
	
	
	switch(iMsg)
	{	

		

			

		
	case WM_DESTROY:
		//MessageBox(hwnd,TEXT("This is WM_DESTROY!!"),TEXT("In Wm_DESTROY"),MB_OK);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

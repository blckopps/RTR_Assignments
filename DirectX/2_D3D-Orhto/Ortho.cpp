#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>

#include<d3d11.h>
#include<d3dcompiler.h> //shader compilation
#include<dxgi.h>
#include"XNAMath_204/xnamath.h"

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")
#pragma comment (lib,"DXGI.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Log(const char *);

//GLOBAL VARIABLES
FILE *gpFile = NULL;
char gszLogFileName[] = "Log.txt";

HWND ghwnd = NULL;
DWORD dwStyle;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

float gClearColor[4];

IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpId3DRenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

struct CBUFFER			//OpenGL's Uniforms
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gOrthographicProjectionMatrix;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//FUnction declaration
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);

	//variable decl

	bool bdone = false;

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("D3D11");

	if (fopen_s(&gpFile, gszLogFileName, "w") != 0)
	{
		MessageBox(NULL, TEXT("Can't create log file"), TEXT("ERROR!!!"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, "log file created successfully\n");
		fclose(gpFile);
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	//create window

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("D3D_Window-SHUBHAM"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPCHILDREN | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//call initialize D3D
	HRESULT hr;
	hr = initialize();
	if (FAILED(hr))
	{
		Log("Initialize faild!!!\n");
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{


		Log("Initialize Successfull\n");
	}

	//call in game loop UpdateWindow(hwnd);

	while (bdone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bdone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}
		}
		else
		{
			//call to display
			display();

			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bdone = true;
			}
		}

	}
	//clean up
	uninitialize();

	return((int)msg.wParam);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//FUnction Declarations
	HRESULT resize(int, int);

	void uninitialize(void);

	void toogle_screen(void);

	HRESULT hr;

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)	//if 0 then window is active
		{
			gbActiveWindow = true;
		}
		else
		{
			gbActiveWindow = false;
		}
		break;

	case WM_ERASEBKGND:
		return(0);
		break;

	case WM_KEYDOWN:

		switch (wParam)
		{
		case 0x46:	//F pressed

			toogle_screen();

			break;

		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;

			break;
		}

		break;
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_SIZE: //call resize

		if (gpID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));

			if (FAILED(hr))
			{
				Log("resize failed\n");
				return(hr);
			}
			else
			{
				Log("resize() successed\n");
			}
		}
		break;

	case WM_CLOSE:

		DestroyWindow(hwnd);
		break;


	case WM_DESTROY:
		//Log("Exit called");
		uninitialize();
		PostQuitMessage(0);
		break;

	default:

		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
//User Defined Functions

void toogle_screen(void)
{

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			MONITORINFO	mi = { sizeof(MONITORINFO) };

			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle&~WS_OVERLAPPEDWINDOW);

				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);

			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd,
			GWL_STYLE,
			dwStyle |
			WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowPos(ghwnd,
			HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER |
			SWP_FRAMECHANGED |
			SWP_NOMOVE |
			SWP_NOSIZE |
			SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

void Log(const char * log)
{
	if (fopen_s(&gpFile, gszLogFileName, "a+") != 0)
	{
		MessageBox(NULL, TEXT("Can't open log"), TEXT("ERROR!!!"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, log);
		
		fclose(gpFile);
	}
}


HRESULT initialize(void)
{

	//function declaration
	void uninitialize(void);
	HRESULT resize(int width, int height);

	//variable declarations
	HRESULT hr;

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_UNKNOWN,
	};

	/*
		D3D_DRIVER_TYPE_UNKNOWN => it is needed when we are selecting our own adapter
	*/

	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;

	UINT createDeviceFlag = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevel = 1;

	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;

	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferCount = 1;

	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	//Get NVIDIA
	IDXGIFactory *pIDXGIFactory = NULL;
	IDXGIAdapter *pAdapter = NULL;
	IDXGIOutput* pOutput = NULL;
	std::vector <IDXGIAdapter*> vAdapters;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);
	if (FAILED(hr))
	{
		Log("CreateDXGIFactory1 failed\n");
	}

	// Collect all Adapters
	for (unsigned int i = 0; pIDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		vAdapters.push_back(pAdapter);
	}

	int obtainedAdapterIndex = 0;// Set 0 for default behavior
	size_t bestVideoMemory = 0;

	// Iterate over all adapter for desired qualities
	for (unsigned int iIndexAdapter = 0; iIndexAdapter < vAdapters.size(); iIndexAdapter++)
	{
		DXGI_ADAPTER_DESC desc;

		vAdapters[iIndexAdapter]->GetDesc(&desc);

		// Selection as per name
		/*if (wcsncmp(desc.Description, L"NVIDIA", 6) == 0)
		{
			obtainedAdapterIndex = iIndexAdapter;
			break;
		}*/
		if (fopen_s(&gpFile, gszLogFileName, "a+") != 0)
		{
			MessageBox(NULL, TEXT("Can't open log"), TEXT("ERROR!!!"), MB_OK);
			exit(0);
		}
		else
		{
			fprintf_s(gpFile, "\t%d. Adapter obtained  is : %S\n", iIndexAdapter, desc.Description);
			fclose(gpFile);
		}

		// Selection as per heighest video memory
		if ((desc.DedicatedVideoMemory != 0) && (desc.DedicatedVideoMemory > bestVideoMemory))
		{
			bestVideoMemory = desc.DedicatedVideoMemory;
			obtainedAdapterIndex = iIndexAdapter;
		}

	}

	pAdapter = vAdapters[obtainedAdapterIndex];

	//print adapter
	DXGI_ADAPTER_DESC desc;
	pAdapter->GetDesc(&desc);

	if (fopen_s(&gpFile, gszLogFileName, "a+") != 0)
	{
		MessageBox(NULL, TEXT("Can't open log"), TEXT("ERROR!!!"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, "Best Adapter  is : %S\n", desc.Description);
		fclose(gpFile);
	}

	//Log("Adapter type is : %s",  desc.AdapterLuid);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];

		hr = D3D11CreateDeviceAndSwapChain(
			pAdapter,
			d3dDriverType,
			NULL,
			createDeviceFlag,
			&d3dFeatureLevel_required,
			numFeatureLevel,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain,
			&gpID3D11Device,
			&d3dFeatureLevel_acquired,
			&gpID3D11DeviceContext
		);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}



	if (FAILED(hr))
	{
		Log("D3D11CreateDeviceAndSwapChain: failed hr\n");
		return(hr);
	}
	else
	{
		//
		Log("D3D11CreateDeviceAndSwapChain : successfull\n");
		switch (d3dDriverType)
		{
		case D3D_DRIVER_TYPE_UNKNOWN:
			Log("Driver type is : unknown\n");
			break;
		case D3D_DRIVER_TYPE_HARDWARE:
			Log("Driver type is : Hardware\n");
			break;
		case D3D_DRIVER_TYPE_REFERENCE:
			Log("Driver type is : REFERENCE\n");
			break;
		case D3D_DRIVER_TYPE_NULL:
			Log("Driver type is NULL\n");
			break;
		case D3D_DRIVER_TYPE_SOFTWARE:
			Log("Driver type is software\n");
			break;
		case D3D_DRIVER_TYPE_WARP:
			Log("Driver type is : WARP\n");
			break;
		default:
			break;
		}


		//supported version
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			Log("Version : D3D_FEATURE_LEVEL_11_0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			Log("Version : D3D_FEATURE_LEVEL_10_1\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			Log("Version : D3D_FEATURE_LEVEL_10_0\n");
		}
		else
		{
			Log("Version : Unknown\n");
		}
	}

	////////////////########**********shader code*********#########////////////////////////////////

	/////////*******Vertex Shader***********//////////////////
	const char *vertexShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldViewProjectionMatrix;" \
		"}" \
		"float4 main(float4 pos : POSITION) : SV_POSITION" \
		"{" \
		"float4 position = mul(worldViewProjectionMatrix, pos);" \
		"return (position);" \
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);
	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			if (fopen_s(&gpFile, gszLogFileName, "a+") != 0)
			{
				MessageBox(NULL, TEXT("Can't open log"), TEXT("ERROR!!!"), MB_OK);
				exit(0);
			}
			else
			{
				fprintf_s(gpFile, "D3DCompile fail for VS : Error : %s\n", (char *)pID3DBlob_Error->GetBufferPointer());

				fclose(gpFile);
			}
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		Log("D3DCompile successfull for VS\n");
	}

	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader);

	if (FAILED(hr))
	{
		Log("CreateVertexShader falid for VS \n");
	}
	else
	{
		Log("CreateVertexShader successfull for VS \n");
	}

	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);

	//////////****Pixel Shader***///////////////
	const char *pixelShaderSourceCode =

		"float4 main(void) : SV_TARGET" \
		"{" \
		"return (float4(1.0f, 1.0f, 1.0f, 1.0f));" \
		"}";

	ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode,
				lstrlenA(pixelShaderSourceCode) + 1,
				"PS",
				NULL,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main",
				"ps_5_0",
				0,
				0,
				&pID3DBlob_PixelShaderCode,
				&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			if (fopen_s(&gpFile, gszLogFileName, "a+") != 0)
			{
				MessageBox(NULL, TEXT("Can't open log"), TEXT("ERROR!!!"), MB_OK);
				exit(0);
			}
			else
			{
				fprintf_s(gpFile, "D3DCompile fail for PS : Error : %s\n", (char *)pID3DBlob_Error->GetBufferPointer());

				fclose(gpFile);
			}
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		Log("D3DCompile successfull for PS\n");
	}

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader);

	if (FAILED(hr))
	{
		Log("CreateVertexShader falid for PS \n");
	}
	else
	{
		Log("CreateVertexShader successfull for PS \n");
	}

	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);

	///////////////////Shader End///////////
	float vertices[] =
	{
		0.0f, 50.0f, 0.0f,
		50.0f, -50.0f, 0.0f,
		-50.0f, -50.0f, 0.0f,
	};

	//create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float)*ARRAYSIZE(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer);
	if (FAILED(hr))
	{
		Log("CreateBuffer : failed for vertex buffer\n");
	}

	//copy vertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer,
								0,
								D3D11_MAP_WRITE_DISCARD,
								0,
								&mappedSubresource);
	memcpy(mappedSubresource.pData, vertices, sizeof(vertices));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer, 0);


	//create and set input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc;

	inputElementDesc.SemanticName = "POSITION";
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc.InputSlot = 0;
	inputElementDesc.AlignedByteOffset = 0;
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc.InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(&inputElementDesc,
											1,
											pID3DBlob_VertexShaderCode->GetBufferPointer(),
											pID3DBlob_VertexShaderCode->GetBufferSize(),
											&gpID3D11InputLayout);
	if (FAILED(hr))
	{
		Log("CreateInputLayout(): POSITION : failed\n");
	}
	else
	{
		Log("CreateInputLayout(): POSITION : Successfull\n");
	}
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_PixelShaderCode->Release();

	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_PixelShaderCode = NULL;

	//define and set constant buffer
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D10_BUFFER_DESC));

	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, 0, &gpID3D11Buffer_ConstantBuffer);

	if (FAILED(hr))
	{
		Log("CreateBuffer: gpID3D11Buffer_ConstantBuffer: failed\n");
	}
	else
	{
		Log("CreateBuffer: gpID3D11Buffer_ConstantBuffer: successfull\n");
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer); // 0: index in shader
																						// 1: How many to set
	//clear color
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 1.0f;
	gClearColor[3] = 1.0f;

	gOrthographicProjectionMatrix = XMMatrixIdentity();

	//call resize()
	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		Log("resize failed\n");
		return(hr);
	}
	else
	{
		Log("resize succesfull\n");
	}

	return(S_OK);
}


HRESULT resize(int width, int height)
{
	HRESULT hr = S_OK;

	//free size independent resource
	if (gpId3DRenderTargetView)
	{
		gpId3DRenderTargetView->Release();
		gpId3DRenderTargetView = NULL;
	}

	//resize swap chain buffer accordingly.. because height and width is going to change.
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	//get back buffer form swap chain
	//it is going to change because height and width is going to change.
	ID3D11Texture2D *pID3D11Texture2d_BackBuffer;

	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2d_BackBuffer);

	//again get render target view
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2d_BackBuffer, NULL, &gpId3DRenderTargetView);
	if (FAILED(hr))
	{
		Log("resize():CreateRenderTargetView: faild hr\n");
	}

	pID3D11Texture2d_BackBuffer->Release();
	pID3D11Texture2d_BackBuffer = NULL;

	//set render target view
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpId3DRenderTargetView, NULL);

	//set view port
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = float(width);
	d3dViewPort.Height = float(height);
	d3dViewPort.MaxDepth = 1.0f;
	d3dViewPort.MinDepth = 0.0f;

	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	//set projection matrix
	if (width <= height)
	{
		gOrthographicProjectionMatrix = XMMatrixOrthographicOffCenterLH
		(
			-100.0f,
			100.0f,
			-100.0f*((float)height/(float)width),
			100.0f * ((float)height/(float)width),
			-100.0f,
			100.0f
		);
	}
	else
	{
		gOrthographicProjectionMatrix = XMMatrixOrthographicOffCenterLH
		(
			-100.0f * ((float)width / (float)height),
			100.0f * ((float)width/(float)height),
			-100.0f,
			100.0f,
			-100.0f,
			100.0f
		);
	}
	return(hr);
}

void display(void)
{
	//clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpId3DRenderTargetView, gClearColor);

	//select vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer, &stride, &offset);

	//select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//matrix calculations
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gOrthographicProjectionMatrix;

	//load data in constant buffer
	CBUFFER constantBuffer;
	
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	//draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(3, 0);

	//buffer swapping
	gpIDXGISwapChain->Present(0, 0);
}

void uninitialize(void)
{
	//code
	if (gpId3DRenderTargetView)
	{
		gpId3DRenderTargetView->Release();
		gpId3DRenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

}
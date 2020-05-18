#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>

#include<d3d11.h>
#include<d3dcompiler.h> //shader compilation
#include<dxgi.h>
#include<xnamath.h>
#include "WICTextureLoader.h"	//

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")
#pragma comment (lib,"DXGI.lib")
#pragma comment (lib,"DirectXTK.lib")

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

bool gbActiveWindow_SB = false;
bool gbEscapeKeyIsPressed_SB = false;
bool gbFullScreen_SB = false;

float gClearColor[4];

IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpId3DRenderTargetView = NULL;

//shader
ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;

//buffers
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_cube_SB = NULL;
ID3D11Buffer *gpID3D11Buffer_TextureBuffer_cube_SB = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_pyramid_SB = NULL;
ID3D11Buffer *gpID3D11Buffer_TextureBuffer_pyramid_SB = NULL;

ID3D11InputLayout *gpID3D11InputLayout_SB = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_SB = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState_SB = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView_SB = NULL;

//Pyramid
ID3D11ShaderResourceView *gpID3D11ShaderResouceView_Texture_Pyramid = NULL;
ID3D11SamplerState *gpID3D11SamplerState_Pyramid = NULL;

//Cube
ID3D11ShaderResourceView *gpID3D11ShaderResouceView_Texture_Cube = NULL;
ID3D11SamplerState *gpID3D11SamplerState_Cube = NULL;

struct CBUFFER			//OpenGL's Uniforms
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix_SB;

float angle_SB = 0.0f;

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
		TEXT("D3D TEXTURE SMILEY-SHUBHAM"),
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

			if (gbActiveWindow_SB == true)
			{
				if (gbEscapeKeyIsPressed_SB == true)
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
			gbActiveWindow_SB = true;
		}
		else
		{
			gbActiveWindow_SB = false;
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
			if (gbEscapeKeyIsPressed_SB == false)
				gbEscapeKeyIsPressed_SB = true;

			break;
		}

		break;
	case WM_SETFOCUS:
		gbActiveWindow_SB = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow_SB = false;
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

	if (gbFullScreen_SB == false)
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
		gbFullScreen_SB = true;
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
		gbFullScreen_SB = false;
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
	HRESULT LoadD3DTexture(const wchar_t *textureFileName, ID3D11ShaderResourceView **ppID3D11ShaderResourceView);

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
		"struct vertex_output" \
		"{" \
		"float4 pos: SV_POSITION;" \
		"float2 texcoord: TEXCOORD;" \
		"};" \
		"vertex_output main(float4 pos : POSITION , float2 texcoord : TEXCOORD) " \
		"{" \
		"vertex_output output;" \
		"output.pos=mul(worldViewProjectionMatrix, pos);" \
		"output.texcoord=texcoord;" \
		"return output;" \
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
		"Texture2D myTexture2D;" \
		"SamplerState mySamplerState;" \
		"float4 main(float4 pos : SV_POSITION, float2 texcoord:TEXCOORD):SV_TARGET" \
		"{" \
		"float4 color = myTexture2D.Sample(mySamplerState, texcoord);" \
		"return (color);" \
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
	//triangle array
	float vertices_smiley[] =
	{
		
		////FRONT
		//-1.0f, 1.0f, -1.0f,
		//1.0f, 1.0f, -1.0f,
		//-1.0f, -1.0f, -1.0f,
		////2nd triangle
		//-1.0f, -1.0f, -1.0f,
		//1.0f, 1.0f, -1.0f,
		//1.0f, -1.0f, -1.0f,

		//
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f


	};

	float texcoords[] =
	{
		
		0.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,

		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0

	};

	//
	//create vertex buffer
	D3D11_BUFFER_DESC bufferDesc_vertex;
	ZeroMemory(&bufferDesc_vertex, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_vertex.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_vertex.ByteWidth = sizeof(float)*ARRAYSIZE(vertices_smiley);
	bufferDesc_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_vertex, NULL, &gpID3D11Buffer_VertexBuffer_cube_SB);
	if (FAILED(hr))
	{
		Log("CreateBuffer : failed for vertex buffer\n");
	}

	//copy vertices_smiley into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource_vertex;
	ZeroMemory(&mappedSubresource_vertex, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_cube_SB,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource_vertex);

	memcpy(mappedSubresource_vertex.pData, vertices_smiley, sizeof(vertices_smiley));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_cube_SB, 0);

	//create texture buffer
	D3D11_BUFFER_DESC bufferDesc_texture;
	ZeroMemory(&bufferDesc_texture, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_texture.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_texture.ByteWidth = sizeof(float)*ARRAYSIZE(texcoords);
	bufferDesc_texture.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_texture.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_texture, NULL, &gpID3D11Buffer_TextureBuffer_cube_SB);
	if (FAILED(hr))
	{
		Log("CreateBuffer : failed for vertex buffer for cube\n");
	}

	//copy color buffer into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource_texture;

	ZeroMemory(&mappedSubresource_texture, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(gpID3D11Buffer_TextureBuffer_cube_SB,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource_texture);

	memcpy(mappedSubresource_texture.pData, texcoords, sizeof(texcoords));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_TextureBuffer_cube_SB, 0);

	//////////*******create and set input layout******///////////
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];

	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	//glPixelStorei
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout_SB);

	if (FAILED(hr))
	{
		Log("CreateInputLayout():  : failed\n");
	}
	else
	{
		Log("CreateInputLayout():  : Successfull\n");
	}
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout_SB);

	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_PixelShaderCode->Release();

	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_PixelShaderCode = NULL;

	/////////////////Set Rasterizer state//////////////////////////////////
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;		//FRNOT AND BACK ARE OTHER OPTIONS
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState_SB);
	if (FAILED(hr))
	{
		Log("CreateRasterizerState fail!!!");
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState_SB);


	////////////////////define and set constant buffer//////////////////////////////

	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D10_BUFFER_DESC));

	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, 0, &gpID3D11Buffer_ConstantBuffer_SB);

	if (FAILED(hr))
	{
		Log("CreateBuffer: gpID3D11Buffer_ConstantBuffer_SB: failed\n");
	}
	else
	{
		Log("CreateBuffer: gpID3D11Buffer_ConstantBuffer_SB: successfull\n");
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_SB); // 0: index in shader
																						// 1: How many to set

	//Load texture
	hr = LoadD3DTexture(L"Smiley.bmp", &gpID3D11ShaderResouceView_Texture_Cube);
	if (FAILED(hr))
	{
		Log("LoadD3DTexture: failed for cueb\n");
	}
	else
	{
		Log("LoadD3DTexture Successfull for cube\n");
	}

	//create sample state
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(D3D11_SAMPLER_DESC));

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = gpID3D11Device->CreateSamplerState(&sampleDesc, &gpID3D11SamplerState_Cube);
	if (FAILED(hr))
	{
		Log("CreateSamplerState failed \n");
	}
	else
	{
		Log("CreateSamplerState success\n");
	}

	//clear color
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	gPerspectiveProjectionMatrix_SB = XMMatrixIdentity();

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

	//create depth stencile view buffer
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;

	gpID3D11Device->CreateTexture2D(&textureDesc, NULL, &pID3D11Texture2D_DepthBuffer);

	//create depth stencil view from above DSV buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer,
		&depthStencilViewDesc, &gpID3D11DepthStencilView_SB);

	if (FAILED(hr))
	{
		Log("CreateDepthStencilView : Fail!!!!\n");
	}

	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;

	//set render target view
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpId3DRenderTargetView, gpID3D11DepthStencilView_SB);

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
	gPerspectiveProjectionMatrix_SB = XMMatrixPerspectiveFovLH
	(
		XMConvertToRadians(45.0f),
		(float)width / (float)height,
		0.1f,
		100.0f
	);
	return(hr);
}

void display(void)
{
	//clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpId3DRenderTargetView, gClearColor);

	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView_SB, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//select vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_cube_SB, &stride, &offset);

	stride = sizeof(float) * 2;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_TextureBuffer_cube_SB, &stride, &offset);

	//bind texture and sampler as PS
	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResouceView_Texture_Cube);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Cube);

	//select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//matrix calculations
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	XMMATRIX transaltionMatrix = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
	
	worldMatrix =  transaltionMatrix;

	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix_SB;

	//load data in constant buffer
	CBUFFER constantBuffer;

	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_SB, 0, NULL, &constantBuffer, 0, 0);

	//draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(6, 0);



	//buffer swapping
	gpIDXGISwapChain->Present(0, 0);

}

void uninitialize(void)
{
	//code

	if (gpID3D11Buffer_ConstantBuffer_SB)
	{
		gpID3D11Buffer_ConstantBuffer_SB->Release();
		gpID3D11Buffer_ConstantBuffer_SB = NULL;
	}

	if (gpID3D11InputLayout_SB)
	{
		gpID3D11InputLayout_SB->Release();
		gpID3D11InputLayout_SB = NULL;
	}

	//triangle
	if (gpID3D11Buffer_VertexBuffer_cube_SB)
	{
		gpID3D11Buffer_VertexBuffer_cube_SB->Release();
		gpID3D11Buffer_VertexBuffer_cube_SB = NULL;
	}

	if (gpID3D11Buffer_TextureBuffer_cube_SB)
	{
		gpID3D11Buffer_TextureBuffer_cube_SB->Release();
		gpID3D11Buffer_TextureBuffer_cube_SB = NULL;
	}


	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}
	//common
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

//Load Texture from File
HRESULT LoadD3DTexture(const wchar_t *textureFileName, ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
	HRESULT hr;

	//create texture
	hr = DirectX::CreateWICTextureFromFile(gpID3D11Device, gpID3D11DeviceContext, textureFileName, NULL, ppID3D11ShaderResourceView);
	if (FAILED(hr))
	{
		Log("CreateWICTextureFromFile Failed\n");
	}
	return hr;
}

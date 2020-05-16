#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>

#include<d3d11.h>
#include<d3dcompiler.h> //shader compilation
#include<dxgi.h>

#pragma warning(disable:438)
#include<xnamath.h>
#include"Sphere.h"

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")
#pragma comment (lib,"DXGI.lib")
#pragma comment (lib,"Sphere.lib")


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
bool gIsLighting = false;

bool PerVertex = true;
bool PerFragment = false;

float gClearColor[4];

IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpId3DRenderTargetView = NULL;



///////////////////////********Per Vertex********/////////////////////////
/////////////buffers///////////////////////////////////////////

//shader
ID3D11VertexShader *gpID3D11VertexShader_PV = NULL;
ID3D11PixelShader *gpID3D11PixelShader_PV = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_sphere_SB_PV = NULL;
ID3D11Buffer *gpID3D11Buffer_NormalBuffer_sphere_SB_PV = NULL;

//Input layout
ID3D11InputLayout *gpID3D11InputLayout_SB_PV = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_SB_PV = NULL;

//////////////////////////***************//////////////////////////

///////////*******Per Fragment ******//////////////////
//shader
ID3D11VertexShader *gpID3D11VertexShader_PF = NULL;
ID3D11PixelShader *gpID3D11PixelShader_PF = NULL;

/////////////buffers///////////////////////////////////////////
//ID3D11Buffer *gpID3D11Buffer_VertexBuffer_sphere_SB = NULL;
//ID3D11Buffer *gpID3D11Buffer_NormalBuffer_sphere_SB = NULL;

//Input layout
ID3D11InputLayout *gpID3D11InputLayout_SB_PF = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_SB_PF = NULL;


///////////////******Per Frag ENd***********///////////////
//Rastarizer state
ID3D11RasterizerState *gpID3D11RasterizerState_SB = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView_SB = NULL;


struct CBUFFER			//OpenGL's Uniforms
{
	//Matrices
	XMMATRIX projectionMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX worldMatrix;

	XMVECTOR La_Red;
	XMVECTOR La_Green;
	XMVECTOR La_Blue;

	XMVECTOR Ld_Red;
	XMVECTOR Ld_Green;
	XMVECTOR Ld_Blue;

	XMVECTOR Ls_Red;
	XMVECTOR Ls_Green;
	XMVECTOR Ls_Blue;

	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;

	XMVECTOR lightPosition_Red;
	XMVECTOR lightPosition_Green;
	XMVECTOR lightPosition_Blue;

	float MaterialShininess;
	UINT  keyPressed;
};

XMMATRIX gPerspectiveProjectionMatrix_SB;

//SPHERE variables
ID3D11Buffer *gpID3D11Buffer_IndexBuffer_SB = NULL;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_texture[764];

unsigned short sphere_elements[2280];

unsigned int gNumberElements;
unsigned int gNumberVertices;

//////Light and Material Arrays
//light array Red
float lightAmbient_Red[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightSpecular_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
//light array Green
//
float lightAmbient_Green[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float lightSpecular_Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
//light array Blue
float lightAmbient_Blue[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
float lightSpecular_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

//material array
float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float materialDifuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float lightPosition_Red[4];
float lightPosition_Green[4];
float lightPosition_Blue[4];

float materialShininess = 128.0f;
float angle_X = 0.0f;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//FUnction declaration
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);
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
		TEXT("D3D Sphere Three Lights-SHUBHAM"),
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

				update();

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
		case 'q':	//q pressed
		case 'Q':

			if (gbEscapeKeyIsPressed_SB == false)
				gbEscapeKeyIsPressed_SB = true;


			break;

		case 'l':
		case 'L':
			gIsLighting = !gIsLighting;
			break;

		case 'v':
		case 'V':
			PerVertex = true;
			PerFragment = false;
			break;

		case 'f':
		case 'F':
			PerFragment = true;
			PerVertex = false;
			break;
		case VK_ESCAPE:
			toogle_screen();

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

	/////////*******Vertex Shader Per vertex ***********//////////////////
	const char *vertexShaderSourceCode_PV =
		"cbuffer ConstantBuffer" \
		"{" \

		"float4x4 projectionMatrix;" \
		"float4x4 worldMatrix;" \
		"float4x4 viewMatrix;" \



		"float4 la_Red;" \
		"float4 la_Green;" \
		"float4 la_Blue;" \

		"float4 ld_Red;" \
		"float4 ld_Green;" \
		"float4 ld_Blue;" \

		"float4 ls_Red;" \
		"float4 ls_Green;" \
		"float4 ls_Blue;" \

		"float4 ka;" \

		"float4 kd;" \

		"float4 ks;" \


		"float4 lightPosition_Red;" \
		"float4 lightPosition_Green;" \
		"float4 lightPosition_Blue;" \

		"float materialShininess;" \


		"uint keyPressed ;" \
		"};" \
		"struct vertex_output" \
		"{" \
			"float4 position: SV_POSITION;" \
			"float4 phong_ads_color: COLOR;" \
		"};" \
		"vertex_output main(float4 pos : POSITION , float4  normal: NORMAL) " \
		"{" \
		"float4x4 worldViewMatrices;" \

		"worldViewMatrices = mul(worldMatrix, viewMatrix);" \


		"vertex_output output;" \

		"if(keyPressed == 1)" \
		"{" \
			" float4 eyeCoordinates = mul(worldMatrix, pos);" \
			"eyeCoordinates = mul(viewMatrix, eyeCoordinates);"\

			"float3 tnorms=normalize((mul((float3x3)mul(viewMatrix,worldMatrix),normal)));" \

			" float3 light_direction_Red = normalize((float3)lightPosition_Red-eyeCoordinates);" \
			" float3 light_direction_Green = normalize((float3)lightPosition_Green-eyeCoordinates);" \
			" float3 light_direction_Blue = normalize((float3)lightPosition_Blue-eyeCoordinates);" \

			"float tn_dot_ld_Red = max(dot(tnorms, light_direction_Red), 0.0);" \
			"float tn_dot_ld_Green = max(dot(tnorms, light_direction_Green), 0.0);" \
			"float tn_dot_ld_Blue = max(dot(tnorms, light_direction_Blue), 0.0);" \

			"float3 ambient_Red = la_Red * ka;"  \
			"float3 ambient_Green = la_Green * ka;"  \
			"float3 ambient_Blue = la_Blue * ka;"  \

			"float3 difuse_Red  = ld_Red * kd * tn_dot_ld_Red;"  \
			"float3 difuse_Green = ld_Green * kd * tn_dot_ld_Green;"  \
			"float3 difuse_Blue  = ld_Blue * kd * tn_dot_ld_Blue;"  \

			"float3 reflectionVector_Red = reflect(-light_direction_Red, tnorms);" \
			"float3 reflectionVector_Green = reflect(-light_direction_Green, tnorms);" \
			"float3 reflectionVector_Blue = reflect(-light_direction_Blue, tnorms);" \

			"float3 viewerVector = normalize((float3)(-eyeCoordinates.xyz));"  \

			"float3 specular_Red = ls_Red * ks * pow(max(dot(reflectionVector_Red, viewerVector), 0.0), materialShininess);" \
			"float3 specular_Green = ls_Green * ks * pow(max(dot(reflectionVector_Green, viewerVector), 0.0), materialShininess);" \
			"float3 specular_Blue = ls_Blue * ks * pow(max(dot(reflectionVector_Blue, viewerVector), 0.0), materialShininess);" \

			"float3 redLight = ambient_Red + difuse_Red + specular_Red;" \
			"float3 greenLight = ambient_Green + difuse_Green + specular_Green;" \
			"float3 blueLight = ambient_Blue + difuse_Blue + specular_Blue;" \

			"output.phong_ads_color = float4(redLight + greenLight + blueLight, 1.0);" \

		"}" \
		"else" \
		"{" \
			"output.phong_ads_color = float4(1.0, 1.0, 1.0, 1.0);" \
		"}"\

		"output.position=mul(projectionMatrix, mul(worldViewMatrices, pos));" \

		"return output;" \
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode_PV = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode_PV,
		lstrlenA(vertexShaderSourceCode_PV) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode_PV,
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

	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode_PV->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PV->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_PV);

	if (FAILED(hr))
	{
		Log("CreateVertexShader falid for VS \n");
	}
	else
	{
		Log("CreateVertexShader successfull for VS \n");
	}

	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PV, 0, 0);

	//////////****Pixel Shader  Per vertex***///////////////
	const char *pixelShaderSourceCode_PV =
		"float4 main(float4 pos : SV_POSITION, float4 ads_color:COLOR):SV_TARGET" \
		"{" \
			"float4 color = ads_color;" \
			"return (color);" \
		"}";

	ID3DBlob *pID3DBlob_PixelShaderCode_PV = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode_PV,
		lstrlenA(pixelShaderSourceCode_PV) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode_PV,
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

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode_PV->GetBufferPointer(),
		pID3DBlob_PixelShaderCode_PV->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader_PV);

	if (FAILED(hr))
	{
		Log("CreateVertexShader falid for PS \n");
	}
	else
	{
		Log("CreateVertexShader successfull for PS \n");
	}

	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PV, 0, 0);

	///////////////////Per vertex Shader End///////////
	/////////////////////////////////////////////////////////**************////////////////////////////////////
	////////////////////Per Fragment Start////////////////////////

	/////////*******Vertex Shader***********//////////////////
	const char *vertexShaderSourceCode_PF =
		"cbuffer ConstantBuffer" \
		"{" \
			"float4x4 projectionMatrix;" \
			"float4x4 worldMatrix;" \
			"float4x4 viewMatrix;" \


			"float4 la_Red;" \
			"float4 la_Green;" \
			"float4 la_Blue;" \

			"float4 ld_Red;" \
			"float4 ld_Green;" \
			"float4 ld_Blue;" \

			"float4 ls_Red;" \
			"float4 ls_Green;" \
			"float4 ls_Blue;" \

			"float4 ka;" \

			"float4 kd;" \

			"float4 ks;" \


			"float4 lightPosition_Red;" \
			"float4 lightPosition_Green;" \
			"float4 lightPosition_Blue;" \

			"float materialShininess;" \


			"uint keyPressed ;" \
		"};" \
		"struct vertex_output" \
		"{" \
			"float4 position: SV_POSITION;" \
			"float3 tnorms_vs:NORMAL0;" \

			"float3 light_direction_vs_RED:POSITION1;" \
			"float3 light_direction_vs_GREEN:POSITION2;" \
			"float3 light_direction_vs_BLUE:POSITION3;" \

			"float3 viewer_vector_vs:POSITION4;" \

		"};" \
		"vertex_output main(float4 pos : POSITION , float4  normal: NORMAL) " \
		"{" \
		"float4x4 worldViewMatrices;" \

		"worldViewMatrices = mul(worldMatrix, viewMatrix);" \

		"vertex_output output;" \

		" float4 eyeCoordinates = mul(worldMatrix, pos);" \

		"eyeCoordinates = mul(viewMatrix, eyeCoordinates);"\

		"output.tnorms_vs = mul((float3x3)mul(viewMatrix,worldMatrix),normal);" \

		" output.light_direction_vs_RED = (float3)lightPosition_Red-eyeCoordinates;" \
		" output.light_direction_vs_GREEN = (float3)lightPosition_Green-eyeCoordinates;" \
		" output.light_direction_vs_BLUE = (float3)lightPosition_Blue-eyeCoordinates;" \


		"output.viewer_vector_vs = (float3)(-eyeCoordinates.xyz);"  \


		"output.position=mul(projectionMatrix, mul(worldViewMatrices, pos));" \

		"return output;" \
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode_PF = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode_PF,
		lstrlenA(vertexShaderSourceCode_PF) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode_PF,
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

	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode_PF->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PF->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_PF);

	if (FAILED(hr))
	{
		Log("CreateVertexShader falid for VS per frag \n");
	}
	else
	{
		Log("CreateVertexShader successfull for VS per frag\n");
	}

	//gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PF, 0, 0);

	//////////****Pixel Shader***///////////////
	const char *pixelShaderSourceCode_PF =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 projectionMatrix;" \
		"float4x4 worldMatrix;" \
		"float4x4 viewMatrix;" \

			"float4 la_Red;" \
			"float4 la_Green;" \
			"float4 la_Blue;" \

			"float4 ld_Red;" \
			"float4 ld_Green;" \
			"float4 ld_Blue;" \

			"float4 ls_Red;" \
			"float4 ls_Green;" \
			"float4 ls_Blue;" \

			"float4 ka;" \

			"float4 kd;" \

			"float4 ks;" \


			"float4 lightPosition_Red;" \
			"float4 lightPosition_Green;" \
			"float4 lightPosition_Blue;" \

			"float materialShininess;" \


			"uint keyPressed ;" \
		"};" \

		"struct vertex_output" \
		"{" \
			"float4 position: SV_POSITION;" \
			"float3 tnorms_vs:NORMAL0;" \

			"float3 light_direction_vs_RED:POSITION1;" \
			"float3 light_direction_vs_GREEN:POSITION2;" \
			"float3 light_direction_vs_BLUE:POSITION3;" \

			"float3 viewer_vector_vs:POSITION4;" \
		"};" \

		"float4 main(vertex_output pixel_input):SV_TARGET" \
		"{" \

		"float4 color = float4(1.0, 1.0, 1.0, 1.0);" \

		"float3 tnorms = normalize(pixel_input.tnorms_vs);" \

		"float3 light_direction_Red = normalize(pixel_input.light_direction_vs_RED);" \
		"float3 light_direction_Green = normalize(pixel_input.light_direction_vs_GREEN);" \
		"float3 light_direction_Blue = normalize(pixel_input.light_direction_vs_BLUE);" \

		"float3 viewer_vector = normalize(pixel_input.viewer_vector_vs);" \

		"if(keyPressed == 1)" \
		"{" \
			"float tn_dot_ld_Red = max(dot(tnorms, light_direction_Red), 0.0);" \
			"float tn_dot_ld_Green = max(dot(tnorms, light_direction_Green), 0.0);" \
			"float tn_dot_ld_Blue = max(dot(tnorms, light_direction_Blue), 0.0);" \

			"float3 ambient_Red = la_Red * ka;"  \
			"float3 ambient_Green = la_Green * ka;"  \
			"float3 ambient_Blue = la_Blue * ka;"  \

			"float3 difuse_Red = ld_Red * kd * tn_dot_ld_Red;"  \
			"float3 difuse_Green = ld_Green * kd * tn_dot_ld_Green;"  \
			"float3 difuse_Blue = ld_Blue * kd * tn_dot_ld_Blue;"  \

			"float3 reflectionVector_Red = reflect(-light_direction_Red, tnorms);" \
			"float3 reflectionVector_Green = reflect(-light_direction_Green, tnorms);" \
			"float3 reflectionVector_Blue = reflect(-light_direction_Blue, tnorms);" \

			"float3 specular_Red = ls_Red * ks * pow(max(dot(reflectionVector_Red, viewer_vector), 0.0), materialShininess);" \
			"float3 specular_Green = ls_Green * ks * pow(max(dot(reflectionVector_Green, viewer_vector), 0.0), materialShininess);" \
			"float3 specular_Blue = ls_Blue * ks * pow(max(dot(reflectionVector_Blue, viewer_vector), 0.0), materialShininess);" \
			
			"float3 redLight = ambient_Red + difuse_Red + specular_Red;" \
			"float3 greenLight = ambient_Green + difuse_Green + specular_Green;" \
			"float3 blueLight = ambient_Blue + difuse_Blue + specular_Blue;" \

			"color = float4(redLight + greenLight + blueLight, 1.0);" \

		"}" \

		"return (color);" \
		"}";

	ID3DBlob *pID3DBlob_PixelShaderCode_PF = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode_PF,
		lstrlenA(pixelShaderSourceCode_PF) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode_PF,
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
		Log("D3DCompile successfull for PS -per frag\n");
	}

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode_PF->GetBufferPointer(),
		pID3DBlob_PixelShaderCode_PF->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader_PF);

	if (FAILED(hr))
	{
		Log("CreateVertexShader falid for PS-per frag \n");
	}
	else
	{
		Log("CreateVertexShader successfull for PS-per frag \n");
	}

	//gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PF, 0, 0);




	////////////////////Per Fragment End////////////////////////

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_texture, sphere_elements);

	gNumberVertices = getNumberOfSphereVertices();
	gNumberElements = getNumberOfSphereElements();

	//
	//create vertex buffer
	D3D11_BUFFER_DESC bufferDesc_vertex;
	ZeroMemory(&bufferDesc_vertex, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_vertex.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_vertex.ByteWidth = _ARRAYSIZE(sphere_vertices) * sizeof(float);
	bufferDesc_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_vertex, NULL, &gpID3D11Buffer_VertexBuffer_sphere_SB_PV);
	if (FAILED(hr))
	{
		Log("CreateBuffer : failed for vertex buffer\n");
	}

	//copy vertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource_vertex;
	ZeroMemory(&mappedSubresource_vertex, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_sphere_SB_PV,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource_vertex);

	memcpy(mappedSubresource_vertex.pData, sphere_vertices, sizeof(sphere_vertices));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_sphere_SB_PV, 0);

	/////////////create normal buffer
	D3D11_BUFFER_DESC bufferDesc_normal;
	ZeroMemory(&bufferDesc_normal, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_normal.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_normal.ByteWidth = _ARRAYSIZE(sphere_normals) * sizeof(float);
	bufferDesc_normal.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_normal.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_normal, NULL, &gpID3D11Buffer_NormalBuffer_sphere_SB_PV);
	if (FAILED(hr))
	{
		Log("CreateBuffer : failed for vertex buffer for cube\n");
	}

	//copy  buffer into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource_normal;

	ZeroMemory(&mappedSubresource_normal, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(gpID3D11Buffer_NormalBuffer_sphere_SB_PV,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource_normal);

	memcpy(mappedSubresource_normal.pData, sphere_normals, sizeof(sphere_normals));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_NormalBuffer_sphere_SB_PV, 0);

	//create Element buffer
	D3D11_BUFFER_DESC bufferDesc_elements;
	ZeroMemory(&bufferDesc_elements, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_elements.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_elements.ByteWidth = gNumberElements * sizeof(short);
	bufferDesc_elements.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc_elements.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_elements, NULL, &gpID3D11Buffer_IndexBuffer_SB);
	if (FAILED(hr))
	{
		Log("Create index buffer faild\n");
	}
	else
	{
		Log("CreateBuffer successfull\n");
	}

	//copy indices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource_indices;

	ZeroMemory(&mappedSubresource_indices, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer_SB,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedSubresource_indices);

	memcpy(mappedSubresource_indices.pData, sphere_elements, gNumberElements * sizeof(short));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer_SB, 0);

	//////////*******create and set input layout******///////////
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];

	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "NORMAL";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	//glPixelStorei
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	//per vertex
	hr = gpID3D11Device->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode_PV->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PV->GetBufferSize(),
		&gpID3D11InputLayout_SB_PV);

	if (FAILED(hr))
	{
		Log("CreateInputLayout():failed for per vertex\n");
	}
	////per fragment
	/*hr = gpID3D11Device->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode_PF->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PF->GetBufferSize(),
		&gpID3D11InputLayout_SB_PF);

	if (FAILED(hr))
	{
		Log("CreateInputLayout(): failed for per fragment\n");
	}
	else
	{
		Log("CreateInputLayout(): Successfull\n");
	}*/
	//Per vertex
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout_SB_PV);

	//per fragment
	//gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout_SB_PF);

	pID3DBlob_VertexShaderCode_PV->Release();
	//pID3DBlob_VertexShaderCode_PF->Release();

	pID3DBlob_PixelShaderCode_PV->Release();
	//pID3DBlob_PixelShaderCode_PF->Release();

	pID3DBlob_VertexShaderCode_PV = NULL;
	pID3DBlob_VertexShaderCode_PF = NULL;

	pID3DBlob_PixelShaderCode_PV = NULL;
	pID3DBlob_PixelShaderCode_PF = NULL;

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

	//For per vertex
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));

	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, 0, &gpID3D11Buffer_ConstantBuffer_SB_PV);

	if (FAILED(hr))
	{
		Log("CreateBuffer: gpID3D11Buffer_ConstantBuffer_SB_PV: failed\n");
	}
	else
	{
		Log("CreateBuffer: gpID3D11Buffer_ConstantBuffer_SB_PV: successfull\n");
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_SB_PV); // 0: index in shader
																						// 1: How many to set

	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_SB_PV);

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

	//create depth stencil view from above DSV (Depth stencile view)buffer
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

	if (!PerVertex)
	{
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PV, 0, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PV, 0, 0);
	}
	else
	{
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PF, 0, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PF, 0, 0);
	}
	//select geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	//select vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;


	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_sphere_SB_PV, &stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_NormalBuffer_sphere_SB_PV, &stride, &offset);

	//set index buffer
	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer_SB, DXGI_FORMAT_R16_UINT, 0); //R16 mapped with short


	//matrix calculations
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	XMMATRIX transaltionMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);
	/*XMMATRIX r1 = XMMatrixRotationX(angle_SB);
	XMMATRIX r2 = XMMatrixRotationY(angle_SB);
	XMMATRIX r3 = XMMatrixRotationZ(angle_SB);*/

	//XMMATRIX scaleMatrix = XMMatrixScaling(0.78f, 0.78f, 0.78f);

	//rotationMatrix = r1 * r2 * r3;

	worldMatrix = transaltionMatrix;


	//load data in constant buffer
	CBUFFER constantBuffer;

	constantBuffer.projectionMatrix = gPerspectiveProjectionMatrix_SB;
	constantBuffer.worldMatrix = worldMatrix;
	constantBuffer.viewMatrix = viewMatrix;

	if (gIsLighting)
	{
		constantBuffer.La_Red = XMVectorSet(lightAmbient_Red[0], lightAmbient_Red[1], lightAmbient_Red[2], lightAmbient_Red[3]);
		constantBuffer.La_Green = XMVectorSet(lightAmbient_Green[0], lightAmbient_Green[1], lightAmbient_Green[2], lightAmbient_Green[3]);
		constantBuffer.La_Blue = XMVectorSet(lightAmbient_Blue[0], lightAmbient_Blue[1], lightAmbient_Blue[2], lightAmbient_Blue[3]);

		constantBuffer.Ld_Red = XMVectorSet(lightDifuse_Red[0], lightDifuse_Red[1], lightDifuse_Red[2], lightDifuse_Red[3]);
		constantBuffer.Ld_Green = XMVectorSet(lightDifuse_Green[0], lightDifuse_Green[1], lightDifuse_Green[2], lightDifuse_Green[3]);
		constantBuffer.Ld_Blue = XMVectorSet(lightDifuse_Blue[0], lightDifuse_Blue[1], lightDifuse_Blue[2], lightDifuse_Blue[3]);

		constantBuffer.Ls_Red = XMVectorSet(lightSpecular_Red[0], lightSpecular_Red[1], lightSpecular_Red[2], lightSpecular_Red[3]);
		constantBuffer.Ls_Green = XMVectorSet(lightSpecular_Green[0], lightSpecular_Green[1], lightSpecular_Green[2], lightSpecular_Green[3]);
		constantBuffer.Ls_Blue = XMVectorSet(lightSpecular_Blue[0], lightSpecular_Blue[1], lightSpecular_Blue[2], lightSpecular_Blue[3]);

		constantBuffer.Kd = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		constantBuffer.Ka = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		constantBuffer.Ks = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		constantBuffer.lightPosition_Red = XMVectorSet(lightPosition_Red[0], lightPosition_Red[1], lightPosition_Red[2], lightPosition_Red[3]);
		constantBuffer.lightPosition_Green = XMVectorSet(lightPosition_Green[0], lightPosition_Green[1], lightPosition_Green[2], lightPosition_Green[3]);
		constantBuffer.lightPosition_Blue = XMVectorSet(lightPosition_Blue[0], lightPosition_Blue[1], lightPosition_Blue[2], lightPosition_Blue[3]);

		constantBuffer.MaterialShininess = materialShininess;

		constantBuffer.keyPressed = 1;

	}
	else
	{
		constantBuffer.keyPressed = 0;
	}

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_SB_PV, 0, NULL, &constantBuffer, 0, 0);

	//draw vertex buffer to render target

	gpID3D11DeviceContext->DrawIndexed(gNumberElements, 0, 0);

	//buffer swapping
	gpIDXGISwapChain->Present(0, 0);
}

void uninitialize(void)
{
	//code

	if (gpID3D11Buffer_ConstantBuffer_SB_PV)
	{
		gpID3D11Buffer_ConstantBuffer_SB_PV->Release();
		gpID3D11Buffer_ConstantBuffer_SB_PV = NULL;
	}

	if (gpID3D11InputLayout_SB_PV)
	{
		gpID3D11InputLayout_SB_PV->Release();
		gpID3D11InputLayout_SB_PV = NULL;
	}


	if (gpID3D11Buffer_VertexBuffer_sphere_SB_PV)
	{
		gpID3D11Buffer_VertexBuffer_sphere_SB_PV->Release();
		gpID3D11Buffer_VertexBuffer_sphere_SB_PV = NULL;
	}

	if (gpID3D11Buffer_NormalBuffer_sphere_SB_PV)
	{
		gpID3D11Buffer_NormalBuffer_sphere_SB_PV->Release();
		gpID3D11Buffer_NormalBuffer_sphere_SB_PV = NULL;
	}

	if (gpID3D11PixelShader_PV)
	{
		gpID3D11PixelShader_PV->Release();
		gpID3D11PixelShader_PV = NULL;
	}

	if (gpID3D11VertexShader_PV)
	{
		gpID3D11VertexShader_PV->Release();
		gpID3D11VertexShader_PV = NULL;
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

void update()
{
	if (angle_X >= 360.0f)
	{
		angle_X = 0.0f;
	}
	else
	{
		angle_X += 0.0009f;
	}

	lightPosition_Red[0] = 0.0f;
	lightPosition_Red[1] = 100.0f*sin(angle_X);
	lightPosition_Red[2] = 100.0f *cos(angle_X);
	lightPosition_Red[3] = 1.0f;

	lightPosition_Green[0] = 100.0f*cos(angle_X);
	lightPosition_Green[1] = 0.0f;
	lightPosition_Green[2] = 100.0f*sin(angle_X);
	lightPosition_Green[3] = 1.0f;

	lightPosition_Blue[0] = 100 * cos(angle_X);
	lightPosition_Blue[1] = 100 * sin(angle_X);
	lightPosition_Blue[2] = 0.0f;
	lightPosition_Blue[3] = 1.0f;



}
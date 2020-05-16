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

#define NUMOFSPHERES 24

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
int iRotation = 0;

int width_vp;
int height_vp;

IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpId3DRenderTargetView = NULL;

//material array
struct MaterialStruct
{
	float materialAmbient[4];

	float materialDifuse[4];


	float materialSpecular[4];

	float materialShininess;

}*myMaterialStruct[24];

float xPosition = 0.0f;
float yPosition = 0.0f;
float zPosition = 0.0f;

float lightPosition[4] = { xPosition, yPosition, zPosition };

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
	

	XMVECTOR Ld_Red;
	

	XMVECTOR Ls_Red;
	

	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;

	XMVECTOR lightPosition_Red;
	

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
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


//material array
float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float materialDifuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


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
		TEXT("D3D 24 Sphere -SHUBHAM"),
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
		case 'X':
		case 'x':
			iRotation = 0;
			break;

		case 'Y':
		case 'y':
			iRotation = 1;
			break;

		case 'Z':
		case 'z':
			iRotation = 2;
			break;

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
	void defStruct(void);

	defStruct();

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

		"float4 ld_Red;" \

		"float4 ls_Red;" \

		"float4 ka;" \

		"float4 kd;" \

		"float4 ks;" \


		"float4 lightPosition_Red;" \

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

		"float tn_dot_ld_Red = max(dot(tnorms, light_direction_Red), 0.0);" \

		"float3 ambient_Red = la_Red * ka;"  \

		"float3 difuse_Red  = ld_Red * kd * tn_dot_ld_Red;"  \

		"float3 reflectionVector_Red = reflect(-light_direction_Red, tnorms);" \

		"float3 viewerVector = normalize((float3)(-eyeCoordinates.xyz));"  \

		"float3 specular_Red = ls_Red * ks * pow(max(dot(reflectionVector_Red, viewerVector), 0.0), materialShininess);" \

		"float3 redLight = ambient_Red + difuse_Red + specular_Red;" \

		"output.phong_ads_color = float4(redLight, 1.0);" \

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

		"float4 ld_Red;" \

		"float4 ls_Red;" \

		"float4 ka;" \

		"float4 kd;" \

		"float4 ks;" \


		"float4 lightPosition_Red;" \

		"float materialShininess;" \


		"uint keyPressed ;" \
		"};" \
		"struct vertex_output" \
		"{" \
		"float4 position: SV_POSITION;" \
		"float3 tnorms_vs:NORMAL0;" \

		"float3 light_direction_vs_RED:POSITION1;" \

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

		"float4 ld_Red;" \

		"float4 ls_Red;" \

		"float4 ka;" \

		"float4 kd;" \

		"float4 ks;" \


		"float4 lightPosition_Red;" \

		"float materialShininess;" \


		"uint keyPressed ;" \
		"};" \

		"struct vertex_output" \
		"{" \
		"float4 position: SV_POSITION;" \
		"float3 tnorms_vs:NORMAL0;" \

		"float3 light_direction_vs_RED:POSITION1;" \

		"float3 viewer_vector_vs:POSITION4;" \
		"};" \

		"float4 main(vertex_output pixel_input):SV_TARGET" \
		"{" \

		"float4 color = float4(1.0, 1.0, 1.0, 1.0);" \

		"float3 tnorms = normalize(pixel_input.tnorms_vs);" \

		"float3 light_direction_Red = normalize(pixel_input.light_direction_vs_RED);" \

		"float3 viewer_vector = normalize(pixel_input.viewer_vector_vs);" \

		"if(keyPressed == 1)" \
		"{" \
		"float tn_dot_ld_Red = max(dot(tnorms, light_direction_Red), 0.0);" \

		"float3 ambient_Red = la_Red * ka;"  \

		"float3 difuse_Red = ld_Red * kd * tn_dot_ld_Red;"  \

		"float3 reflectionVector_Red = reflect(-light_direction_Red, tnorms);" \

		"float3 specular_Red = ls_Red * ks * pow(max(dot(reflectionVector_Red, viewer_vector), 0.0), materialShininess);" \

		"float3 redLight = ambient_Red + difuse_Red + specular_Red;" \

		"color = float4(redLight, 1.0);" \

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
	width_vp = width;
	height_vp = height;

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

	if (PerVertex)
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
	XMMATRIX transaltionMatrix = XMMatrixIdentity();

	float Y = 0.0f;
	float X = 0.0f;
	float Z = 2.0f;

	//Do transformations...
	

	transaltionMatrix = XMMatrixTranslation(X, Y, Z);
	/*XMMATRIX r1 = XMMatrixRotationX(angle_SB);
	XMMATRIX r2 = XMMatrixRotationY(angle_SB);
	XMMATRIX r3 = XMMatrixRotationZ(angle_SB);*/

	//XMMATRIX scaleMatrix = XMMatrixScaling(0.78f, 0.78f, 0.78f);

	//rotationMatrix = r1 * r2 * r3;

	worldMatrix = transaltionMatrix;

	float xPos = 0;
	float yPos = 0;//height_vp - height_vp / 6;

	
	
	//load data in constant buffer
	for (int i = 1; i <= NUMOFSPHERES; i++)
	{
		CBUFFER constantBuffer;
		XMMATRIX worldMatrix = XMMatrixIdentity();
		XMMATRIX transaltionMatrix = XMMatrixIdentity();


		transaltionMatrix = XMMatrixTranslation(X, Y, Z);

		worldMatrix = transaltionMatrix;

		constantBuffer.projectionMatrix = gPerspectiveProjectionMatrix_SB;
		constantBuffer.worldMatrix = worldMatrix;
		constantBuffer.viewMatrix = viewMatrix;

		if (gIsLighting)
		{

			constantBuffer.La_Red = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);

			constantBuffer.Ld_Red = XMVectorSet(lightDifuse[0], lightDifuse[1], lightDifuse[2], lightDifuse[3]);

			constantBuffer.Ls_Red = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);

			constantBuffer.Ka = XMVectorSet(myMaterialStruct[i - 1]->materialAmbient[0], myMaterialStruct[i - 1]->materialAmbient[1], myMaterialStruct[i - 1]->materialAmbient[2], myMaterialStruct[i - 1]->materialAmbient[3]);
			constantBuffer.Kd = XMVectorSet(myMaterialStruct[i - 1]->materialDifuse[0], myMaterialStruct[i - 1]->materialDifuse[1], myMaterialStruct[i - 1]->materialDifuse[2], myMaterialStruct[i - 1]->materialDifuse[3]);
			constantBuffer.Ks = XMVectorSet(myMaterialStruct[i - 1]->materialSpecular[0], myMaterialStruct[i - 1]->materialSpecular[1], myMaterialStruct[i - 1]->materialSpecular[2], myMaterialStruct[i - 1]->materialSpecular[3]);

			constantBuffer.lightPosition_Red = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.MaterialShininess = myMaterialStruct[i - 1]->materialShininess;

			constantBuffer.keyPressed = 1;
		}
		else
		{
			constantBuffer.keyPressed = 0;
		}

		gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_SB_PV, 0, NULL, &constantBuffer, 0, 0);

		D3D11_VIEWPORT d3dViewPort;
		d3dViewPort.TopLeftX = xPos;
		d3dViewPort.TopLeftY = yPos;
		d3dViewPort.Width = float(width_vp/4);
		d3dViewPort.Height = float(height_vp/6);
		d3dViewPort.MaxDepth = 1.0f;
		d3dViewPort.MinDepth = 0.0f;

		gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);
		//draw vertex buffer to render target

		gpID3D11DeviceContext->DrawIndexed(gNumberElements, 0, 0);

		if (i % 6 == 0)
		{
			xPos += width_vp / 4;
			yPos = 0.0f;//height_vp - height_vp / 6;
		}
		else
		{
			yPos += height_vp / 6;
		}
	}
	//buffer swapping
	gpIDXGISwapChain->Present(0, 0);
}

void uninitialize(void)
{
	//code
	//Free material array
	for (int i = 0; i < NUMOFSPHERES; i++)
	{
		free(myMaterialStruct[i]);
		myMaterialStruct[i] = NULL;
	}

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
	if (iRotation == 0)
	{
		lightPosition[0] = 0.0f;
		lightPosition[1] = 100.0f*sin(angle_X);
		lightPosition[2] = 100.0f *cos(angle_X);
		lightPosition[3] = 1.0f;
	}
	else if (iRotation == 1)
	{
		lightPosition[0] = 100.0f*sin(angle_X);
		lightPosition[1] = 0.0f;
		lightPosition[2] = 100.0f *cos(angle_X);
		lightPosition[3] = 1.0f;
	}
	else
	{
		lightPosition[0] = 100.0f*sin(angle_X);
		lightPosition[1] = 100.0f *cos(angle_X);
		lightPosition[2] = 0.0f;
		lightPosition[3] = 1.0f;
	}


}


void defStruct(void)
{
	for (int i = 0; i < NUMOFSPHERES; i++)
	{
		//myMaterialStruct[i] = (struct MaterialStruct*)malloc(4.0f * 20.0f);
		myMaterialStruct[i] = (struct MaterialStruct *)malloc(sizeof(MaterialStruct));
		//fprintf(gpFile, "sizeof myMaterialStruct of %d is %d \n", i, sizeof(myMaterialStruct[i]));

	}
	//fclose(gpfile);
	//0th


	myMaterialStruct[0]->materialAmbient[0] = 0.0215f;
	myMaterialStruct[0]->materialAmbient[1] = 0.1745f;
	myMaterialStruct[0]->materialAmbient[2] = 0.0215f;
	myMaterialStruct[0]->materialAmbient[3] = 1.0f;

	myMaterialStruct[0]->materialDifuse[0] = 0.07568f;
	myMaterialStruct[0]->materialDifuse[1] = 0.61424f;
	myMaterialStruct[0]->materialDifuse[2] = 0.07568f;
	myMaterialStruct[0]->materialDifuse[3] = 1.0f;

	myMaterialStruct[0]->materialSpecular[0] = 0.633f;
	myMaterialStruct[0]->materialSpecular[1] = 0.727811f;
	myMaterialStruct[0]->materialSpecular[2] = 0.633f;
	myMaterialStruct[0]->materialSpecular[3] = 1.0f;

	myMaterialStruct[0]->materialShininess = 0.6f*128.0f;

	//1th		2nd on first col	   ->
	myMaterialStruct[1]->materialAmbient[0] = 0.135f;
	myMaterialStruct[1]->materialAmbient[1] = 0.2225f;
	myMaterialStruct[1]->materialAmbient[2] = 0.1575f;
	myMaterialStruct[1]->materialAmbient[3] = 1.0f;

	myMaterialStruct[1]->materialDifuse[0] = 0.54f;
	myMaterialStruct[1]->materialDifuse[1] = 0.89f;
	myMaterialStruct[1]->materialDifuse[2] = 0.63f;
	myMaterialStruct[1]->materialDifuse[3] = 1.0f;

	myMaterialStruct[1]->materialSpecular[0] = 0.316228f;
	myMaterialStruct[1]->materialSpecular[1] = 0.316228f;
	myMaterialStruct[1]->materialSpecular[2] = 0.316228f;
	myMaterialStruct[1]->materialSpecular[3] = 1.0f;

	myMaterialStruct[1]->materialShininess = 0.1f *128.0f;

	//2th			   ->
	myMaterialStruct[2]->materialAmbient[0] = 0.05375f;
	myMaterialStruct[2]->materialAmbient[1] = 0.05f;
	myMaterialStruct[2]->materialAmbient[2] = 0.06625f;
	myMaterialStruct[2]->materialAmbient[3] = 1.0f;

	myMaterialStruct[2]->materialDifuse[0] = 0.18275f;
	myMaterialStruct[2]->materialDifuse[1] = 0.17f;
	myMaterialStruct[2]->materialDifuse[2] = 0.22525f;
	myMaterialStruct[2]->materialDifuse[3] = 1.0f;

	myMaterialStruct[2]->materialSpecular[0] = 0.332741f;
	myMaterialStruct[2]->materialSpecular[1] = 0.328634f;
	myMaterialStruct[2]->materialSpecular[2] = 0.346435f;
	myMaterialStruct[2]->materialSpecular[3] = 1.0f;

	myMaterialStruct[2]->materialShininess = 0.3f*128.0f;

	//3th		4th on 1st colmn	   ->
	myMaterialStruct[3]->materialAmbient[0] = 0.25f;
	myMaterialStruct[3]->materialAmbient[1] = 0.20725f;
	myMaterialStruct[3]->materialAmbient[2] = 0.20725f;
	myMaterialStruct[3]->materialAmbient[3] = 1.0f;

	myMaterialStruct[3]->materialDifuse[0] = 1.0f;
	myMaterialStruct[3]->materialDifuse[1] = 0.829f;
	myMaterialStruct[3]->materialDifuse[2] = 0.829f;
	myMaterialStruct[3]->materialDifuse[3] = 1.0f;

	myMaterialStruct[3]->materialSpecular[0] = 0.296648f;
	myMaterialStruct[3]->materialSpecular[1] = 0.296648f;
	myMaterialStruct[3]->materialSpecular[2] = 0.296648f;
	myMaterialStruct[3]->materialSpecular[3] = 1.0f;

	myMaterialStruct[3]->materialShininess = 0.88f*128.0f;

	//4th			   5th on 1st colmn->
	myMaterialStruct[4]->materialAmbient[0] = 0.1745f;
	myMaterialStruct[4]->materialAmbient[1] = 0.01175f;
	myMaterialStruct[4]->materialAmbient[2] = 0.01175f;
	myMaterialStruct[4]->materialAmbient[3] = 1.0f;

	myMaterialStruct[4]->materialDifuse[0] = 0.61424f;
	myMaterialStruct[4]->materialDifuse[1] = 0.04136f;
	myMaterialStruct[4]->materialDifuse[2] = 0.04136f;
	myMaterialStruct[4]->materialDifuse[3] = 1.0f;

	myMaterialStruct[4]->materialSpecular[0] = 0.727811f;
	myMaterialStruct[4]->materialSpecular[1] = 0.626959f;
	myMaterialStruct[4]->materialSpecular[2] = 0.626959f;
	myMaterialStruct[4]->materialSpecular[3] = 1.0f;

	myMaterialStruct[4]->materialShininess = 0.6f * 128.0f;
	//5th	6th on 1st col		   ->
	myMaterialStruct[5]->materialAmbient[0] = 0.1f;
	myMaterialStruct[5]->materialAmbient[1] = 0.18725f;
	myMaterialStruct[5]->materialAmbient[2] = 0.1745f;
	myMaterialStruct[5]->materialAmbient[3] = 1.0f;

	myMaterialStruct[5]->materialDifuse[0] = 0.396f;
	myMaterialStruct[5]->materialDifuse[1] = 0.74151f;
	myMaterialStruct[5]->materialDifuse[2] = 0.69102f;
	myMaterialStruct[5]->materialDifuse[3] = 1.0f;

	myMaterialStruct[5]->materialSpecular[0] = 0.297254f;
	myMaterialStruct[5]->materialSpecular[1] = 0.30829f;
	myMaterialStruct[5]->materialSpecular[2] = 0.30829f;
	myMaterialStruct[5]->materialSpecular[3] = 1.0f;

	myMaterialStruct[5]->materialShininess = 0.1f*128.0f;
	////6th			1st on 2nd colmn   ->
	myMaterialStruct[6]->materialAmbient[0] = 0.329412f;
	myMaterialStruct[6]->materialAmbient[1] = 0.223529f;
	myMaterialStruct[6]->materialAmbient[2] = 0.027451f;
	myMaterialStruct[6]->materialAmbient[3] = 1.0f;

	myMaterialStruct[6]->materialDifuse[0] = 0.780392f;
	myMaterialStruct[6]->materialDifuse[1] = 0.568627f;
	myMaterialStruct[6]->materialDifuse[2] = 0.113725f;
	myMaterialStruct[6]->materialDifuse[3] = 1.0f;

	myMaterialStruct[6]->materialSpecular[0] = 0.992157f;
	myMaterialStruct[6]->materialSpecular[1] = 0.941176f;
	myMaterialStruct[6]->materialSpecular[2] = 0.807843f;
	myMaterialStruct[6]->materialSpecular[3] = 1.0f;

	myMaterialStruct[6]->materialShininess = 0.21794872f*128.0f;

	//7th 2nd on 2nd colm  ->
	myMaterialStruct[7]->materialAmbient[0] = 0.2125f;
	myMaterialStruct[7]->materialAmbient[1] = 0.1275f;
	myMaterialStruct[7]->materialAmbient[2] = 0.054f;
	myMaterialStruct[7]->materialAmbient[3] = 1.0f;

	myMaterialStruct[7]->materialDifuse[0] = 0.714f;
	myMaterialStruct[7]->materialDifuse[1] = 0.4284f;
	myMaterialStruct[7]->materialDifuse[2] = 0.18144f;
	myMaterialStruct[7]->materialDifuse[3] = 1.0f;

	myMaterialStruct[7]->materialSpecular[0] = 0.393548f;
	myMaterialStruct[7]->materialSpecular[1] = 0.271906f;
	myMaterialStruct[7]->materialSpecular[2] = 0.166721f;
	myMaterialStruct[7]->materialSpecular[3] = 1.0f;

	myMaterialStruct[7]->materialShininess = 0.2f*128.0f;
	//8th   3rd on 2nd colm->
	myMaterialStruct[8]->materialAmbient[0] = 0.25f;
	myMaterialStruct[8]->materialAmbient[1] = 0.25f;
	myMaterialStruct[8]->materialAmbient[2] = 0.25f;
	myMaterialStruct[8]->materialAmbient[3] = 1.0f;

	myMaterialStruct[8]->materialDifuse[0] = 0.4f;
	myMaterialStruct[8]->materialDifuse[1] = 0.4f;
	myMaterialStruct[8]->materialDifuse[2] = 0.4f;
	myMaterialStruct[8]->materialDifuse[3] = 1.0f;

	myMaterialStruct[8]->materialSpecular[0] = 0.774597f;
	myMaterialStruct[8]->materialSpecular[1] = 0.774597f;
	myMaterialStruct[8]->materialSpecular[2] = 0.774597f;
	myMaterialStruct[8]->materialSpecular[3] = 1.0f;

	myMaterialStruct[8]->materialShininess = 0.6f*128.0f;
	//9th 2nd col 4th sphre
	myMaterialStruct[9]->materialAmbient[0] = 0.19125f;
	myMaterialStruct[9]->materialAmbient[1] = 0.0735f;
	myMaterialStruct[9]->materialAmbient[2] = 0.0225f;
	myMaterialStruct[9]->materialAmbient[3] = 1.0f;

	myMaterialStruct[9]->materialDifuse[0] = 0.7038f;
	myMaterialStruct[9]->materialDifuse[1] = 0.27048f;
	myMaterialStruct[9]->materialDifuse[2] = 0.0828f;
	myMaterialStruct[9]->materialDifuse[3] = 1.0f;

	myMaterialStruct[9]->materialSpecular[0] = 0.256777f;
	myMaterialStruct[9]->materialSpecular[1] = 0.137622f;
	myMaterialStruct[9]->materialSpecular[2] = 0.086014f;
	myMaterialStruct[9]->materialSpecular[3] = 1.0f;

	myMaterialStruct[9]->materialShininess = 0.1f*128.0f;
	//10th 2nd col 5th sphere
	myMaterialStruct[10]->materialAmbient[0] = 0.24725f;
	myMaterialStruct[10]->materialAmbient[1] = 0.1995f;
	myMaterialStruct[10]->materialAmbient[2] = 0.0745f;
	myMaterialStruct[10]->materialAmbient[3] = 1.0f;

	myMaterialStruct[10]->materialDifuse[0] = 0.75164f;
	myMaterialStruct[10]->materialDifuse[1] = 0.60648f;
	myMaterialStruct[10]->materialDifuse[2] = 0.22648f;
	myMaterialStruct[10]->materialDifuse[3] = 1.0f;

	myMaterialStruct[10]->materialSpecular[0] = 0.628281f;
	myMaterialStruct[10]->materialSpecular[1] = 0.555802f;
	myMaterialStruct[10]->materialSpecular[2] = 0.366065f;
	myMaterialStruct[10]->materialSpecular[3] = 1.0f;

	myMaterialStruct[10]->materialShininess = 0.4f*128.0f;
	//11th 2nd col 6th sphere
	myMaterialStruct[11]->materialAmbient[0] = 0.19225f;
	myMaterialStruct[11]->materialAmbient[1] = 0.19225f;
	myMaterialStruct[11]->materialAmbient[2] = 0.19225f;
	myMaterialStruct[11]->materialAmbient[3] = 1.0f;

	myMaterialStruct[11]->materialDifuse[0] = 0.50754f;
	myMaterialStruct[11]->materialDifuse[1] = 0.50754f;
	myMaterialStruct[11]->materialDifuse[2] = 0.50754f;
	myMaterialStruct[11]->materialDifuse[3] = 1.0f;

	myMaterialStruct[11]->materialSpecular[0] = 0.508273f;
	myMaterialStruct[11]->materialSpecular[1] = 0.508273f;
	myMaterialStruct[11]->materialSpecular[2] = 0.508273f;
	myMaterialStruct[11]->materialSpecular[3] = 1.0f;

	myMaterialStruct[11]->materialShininess = 0.4f*128.0f;

	//12th 3rd colm 1
	myMaterialStruct[12]->materialAmbient[0] = 0.0f;
	myMaterialStruct[12]->materialAmbient[1] = 0.0f;
	myMaterialStruct[12]->materialAmbient[2] = 0.0f;
	myMaterialStruct[12]->materialAmbient[3] = 1.0f;

	myMaterialStruct[12]->materialDifuse[0] = 0.01f;
	myMaterialStruct[12]->materialDifuse[1] = 0.01f;
	myMaterialStruct[12]->materialDifuse[2] = 0.01f;
	myMaterialStruct[12]->materialDifuse[3] = 1.0f;

	myMaterialStruct[12]->materialSpecular[0] = 0.50f;
	myMaterialStruct[12]->materialSpecular[1] = 0.50f;
	myMaterialStruct[12]->materialSpecular[2] = 0.50f;
	myMaterialStruct[12]->materialSpecular[3] = 1.0f;

	myMaterialStruct[12]->materialShininess = 0.25f*128.0f;
	//13th 3rd colm 2
	myMaterialStruct[13]->materialAmbient[0] = 0.0f;
	myMaterialStruct[13]->materialAmbient[1] = 0.1f;
	myMaterialStruct[13]->materialAmbient[2] = 0.06f;
	myMaterialStruct[13]->materialAmbient[3] = 1.0f;

	myMaterialStruct[13]->materialDifuse[0] = 0.0f;
	myMaterialStruct[13]->materialDifuse[1] = 0.50980392f;
	myMaterialStruct[13]->materialDifuse[2] = 0.50980392f;
	myMaterialStruct[13]->materialDifuse[3] = 1.0f;

	myMaterialStruct[13]->materialSpecular[0] = 0.50196078f;
	myMaterialStruct[13]->materialSpecular[1] = 0.50196078f;
	myMaterialStruct[13]->materialSpecular[2] = 0.50196078f;
	myMaterialStruct[13]->materialSpecular[3] = 1.0f;

	myMaterialStruct[13]->materialShininess = 0.25f*128.0f;
	//14th 3rd colm 3
	myMaterialStruct[14]->materialAmbient[0] = 0.0f;
	myMaterialStruct[14]->materialAmbient[1] = 0.0f;
	myMaterialStruct[14]->materialAmbient[2] = 0.0f;
	myMaterialStruct[14]->materialAmbient[3] = 1.0f;

	myMaterialStruct[14]->materialDifuse[0] = 0.1f;
	myMaterialStruct[14]->materialDifuse[1] = 0.35f;
	myMaterialStruct[14]->materialDifuse[2] = 0.1f;
	myMaterialStruct[14]->materialDifuse[3] = 1.0f;

	myMaterialStruct[14]->materialSpecular[0] = 0.45f;
	myMaterialStruct[14]->materialSpecular[1] = 0.55f;
	myMaterialStruct[14]->materialSpecular[2] = 0.45f;
	myMaterialStruct[14]->materialSpecular[3] = 1.0f;

	myMaterialStruct[14]->materialShininess = 0.25f*128.0f;
	//15th 3rd colm 4
	myMaterialStruct[15]->materialAmbient[0] = 0.0f;
	myMaterialStruct[15]->materialAmbient[1] = 0.0f;
	myMaterialStruct[15]->materialAmbient[2] = 0.0f;
	myMaterialStruct[15]->materialAmbient[3] = 1.0f;

	myMaterialStruct[15]->materialDifuse[0] = 0.5f;
	myMaterialStruct[15]->materialDifuse[1] = 0.0f;
	myMaterialStruct[15]->materialDifuse[2] = 0.0f;
	myMaterialStruct[15]->materialDifuse[3] = 1.0f;

	myMaterialStruct[15]->materialSpecular[0] = 0.7f;
	myMaterialStruct[15]->materialSpecular[1] = 0.6f;
	myMaterialStruct[15]->materialSpecular[2] = 0.6f;
	myMaterialStruct[15]->materialSpecular[3] = 1.0f;

	myMaterialStruct[15]->materialShininess = 0.25f*128.0f;
	//16th 3rd colm 5
	myMaterialStruct[16]->materialAmbient[0] = 0.0f;
	myMaterialStruct[16]->materialAmbient[1] = 0.0f;
	myMaterialStruct[16]->materialAmbient[2] = 0.0f;
	myMaterialStruct[16]->materialAmbient[3] = 1.0f;

	myMaterialStruct[16]->materialDifuse[0] = 0.55f;
	myMaterialStruct[16]->materialDifuse[1] = 0.55f;
	myMaterialStruct[16]->materialDifuse[2] = 0.55f;
	myMaterialStruct[16]->materialDifuse[3] = 1.0f;

	myMaterialStruct[16]->materialSpecular[0] = 0.70f;
	myMaterialStruct[16]->materialSpecular[1] = 0.70f;
	myMaterialStruct[16]->materialSpecular[2] = 0.70f;
	myMaterialStruct[16]->materialSpecular[3] = 1.0f;

	myMaterialStruct[16]->materialShininess = 0.25f*128.0f;

	//17th 3rd colm 6
	myMaterialStruct[17]->materialAmbient[0] = 0.0f;
	myMaterialStruct[17]->materialAmbient[1] = 0.0f;
	myMaterialStruct[17]->materialAmbient[2] = 0.0f;
	myMaterialStruct[17]->materialAmbient[3] = 1.0f;

	myMaterialStruct[17]->materialDifuse[0] = 0.5f;
	myMaterialStruct[17]->materialDifuse[1] = 0.5f;
	myMaterialStruct[17]->materialDifuse[2] = 0.0f;
	myMaterialStruct[17]->materialDifuse[3] = 1.0f;

	myMaterialStruct[17]->materialSpecular[0] = 0.60f;
	myMaterialStruct[17]->materialSpecular[1] = 0.60f;
	myMaterialStruct[17]->materialSpecular[2] = 0.50f;
	myMaterialStruct[17]->materialSpecular[3] = 1.0f;

	myMaterialStruct[17]->materialShininess = 0.25f*128.0f;
	//18th 4th colm 1
	myMaterialStruct[18]->materialAmbient[0] = 0.02f;
	myMaterialStruct[18]->materialAmbient[1] = 0.02f;
	myMaterialStruct[18]->materialAmbient[2] = 0.02f;
	myMaterialStruct[18]->materialAmbient[3] = 1.0f;

	myMaterialStruct[18]->materialDifuse[0] = 0.01f;
	myMaterialStruct[18]->materialDifuse[1] = 0.01f;
	myMaterialStruct[18]->materialDifuse[2] = 0.01f;
	myMaterialStruct[18]->materialDifuse[3] = 1.0f;

	myMaterialStruct[18]->materialSpecular[0] = 0.4f;
	myMaterialStruct[18]->materialSpecular[1] = 0.4f;
	myMaterialStruct[18]->materialSpecular[2] = 0.4f;
	myMaterialStruct[18]->materialSpecular[3] = 1.0f;

	myMaterialStruct[18]->materialShininess = 0.078125f*128.0f;
	//19th 4th colm 2
	myMaterialStruct[19]->materialAmbient[0] = 0.0f;
	myMaterialStruct[19]->materialAmbient[1] = 0.05f;
	myMaterialStruct[19]->materialAmbient[2] = 0.05f;
	myMaterialStruct[19]->materialAmbient[3] = 1.0f;

	myMaterialStruct[19]->materialDifuse[0] = 0.4f;
	myMaterialStruct[19]->materialDifuse[1] = 0.5f;
	myMaterialStruct[19]->materialDifuse[2] = 0.5f;
	myMaterialStruct[19]->materialDifuse[3] = 1.0f;

	myMaterialStruct[19]->materialSpecular[0] = 0.04f;
	myMaterialStruct[19]->materialSpecular[1] = 0.7f;
	myMaterialStruct[19]->materialSpecular[2] = 0.7f;
	myMaterialStruct[19]->materialSpecular[3] = 1.0f;

	myMaterialStruct[19]->materialShininess = 0.078125f*128.0f;
	//20th 4th col 3
	myMaterialStruct[20]->materialAmbient[0] = 0.0f;
	myMaterialStruct[20]->materialAmbient[1] = 0.05f;
	myMaterialStruct[20]->materialAmbient[2] = 0.0f;
	myMaterialStruct[20]->materialAmbient[3] = 1.0f;

	myMaterialStruct[20]->materialDifuse[0] = 0.4f;
	myMaterialStruct[20]->materialDifuse[1] = 0.5f;
	myMaterialStruct[20]->materialDifuse[2] = 0.4f;
	myMaterialStruct[20]->materialDifuse[3] = 1.0f;

	myMaterialStruct[20]->materialSpecular[0] = 0.04f;
	myMaterialStruct[20]->materialSpecular[1] = 0.7f;
	myMaterialStruct[20]->materialSpecular[2] = 0.04f;
	myMaterialStruct[20]->materialSpecular[3] = 1.0f;

	myMaterialStruct[20]->materialShininess = 0.078125f*128.0f;
	//21th 4th col 4
	myMaterialStruct[21]->materialAmbient[0] = 0.05f;
	myMaterialStruct[21]->materialAmbient[1] = 0.0f;
	myMaterialStruct[21]->materialAmbient[2] = 0.0f;
	myMaterialStruct[21]->materialAmbient[3] = 1.0f;

	myMaterialStruct[21]->materialDifuse[0] = 0.5f;
	myMaterialStruct[21]->materialDifuse[1] = 0.4f;
	myMaterialStruct[21]->materialDifuse[2] = 0.4f;
	myMaterialStruct[21]->materialDifuse[3] = 1.0f;

	myMaterialStruct[21]->materialSpecular[0] = 0.7f;
	myMaterialStruct[21]->materialSpecular[1] = 0.04f;
	myMaterialStruct[21]->materialSpecular[2] = 0.04f;
	myMaterialStruct[21]->materialSpecular[3] = 1.0f;

	myMaterialStruct[21]->materialShininess = 0.078125f*128.0f;
	//22th 4th col 5th
	myMaterialStruct[22]->materialAmbient[0] = 0.5f;
	myMaterialStruct[22]->materialAmbient[1] = 0.5f;
	myMaterialStruct[22]->materialAmbient[2] = 0.5f;
	myMaterialStruct[22]->materialAmbient[3] = 1.0f;

	myMaterialStruct[22]->materialDifuse[0] = 0.5f;
	myMaterialStruct[22]->materialDifuse[1] = 0.5f;
	myMaterialStruct[22]->materialDifuse[2] = 0.5f;
	myMaterialStruct[22]->materialDifuse[3] = 1.0f;

	myMaterialStruct[22]->materialSpecular[0] = 0.7f;
	myMaterialStruct[22]->materialSpecular[1] = 0.7f;
	myMaterialStruct[22]->materialSpecular[2] = 0.7f;
	myMaterialStruct[22]->materialSpecular[3] = 1.0f;

	myMaterialStruct[22]->materialShininess = 0.078125f*128.0f;
	//23th 4th col 6th
	myMaterialStruct[23]->materialAmbient[0] = 0.05f;
	myMaterialStruct[23]->materialAmbient[1] = 0.05f;
	myMaterialStruct[23]->materialAmbient[2] = 0.0f;
	myMaterialStruct[23]->materialAmbient[3] = 1.0f;

	myMaterialStruct[23]->materialDifuse[0] = 0.5f;
	myMaterialStruct[23]->materialDifuse[1] = 0.5f;
	myMaterialStruct[23]->materialDifuse[2] = 0.4f;
	myMaterialStruct[23]->materialDifuse[3] = 1.0f;

	myMaterialStruct[23]->materialSpecular[0] = 0.7f;
	myMaterialStruct[23]->materialSpecular[1] = 0.7f;
	myMaterialStruct[23]->materialSpecular[2] = 0.7f;
	myMaterialStruct[23]->materialSpecular[3] = 1.0f;

	myMaterialStruct[23]->materialShininess = 0.078125f*128.0f;


}


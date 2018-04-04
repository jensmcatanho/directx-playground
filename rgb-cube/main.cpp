#include <windows.h>
#include <windowsx.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

IDXGISwapChain *swapChain;
ID3D11Device *device;
ID3D11DeviceContext *deviceContext;
ID3D11RenderTargetView *backbuffer;
ID3D11InputLayout *pLayout;
ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;
ID3D11Buffer *pVBuffer;
ID3D11Buffer *pIBuffer;

struct VERTEX{ FLOAT X, Y, Z; };

void InitD3D(HWND hWnd);
void CleanD3D();
void InitPipeline();
void InitGraphics();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	HWND windowHandler;
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = "WindowClass1";
	RegisterClassEx(&windowClass);

	RECT clientRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	windowHandler = CreateWindowEx(NULL, "WindowClass1", "RGB Cube", WS_OVERLAPPEDWINDOW, xPos, yPos, width, height, NULL, NULL, hInstance, NULL);
	ShowWindow(windowHandler, nShowCmd);
	InitD3D(windowHandler);

	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;

		} else {
			deviceContext->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

			UINT stride = sizeof(VERTEX);
			UINT offset = 0;
			deviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, offset);
			deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			deviceContext->DrawIndexed(6, 0, 0);

			swapChain->Present(0, 0);
		}
	}

	CleanD3D();
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void InitD3D(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
	swapChainDesc.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 4;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);

	ID3D11Texture2D *pBackBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	deviceContext->OMSetRenderTargets(1, &backbuffer, NULL);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = GetSystemMetrics(SM_CXSCREEN);
	viewport.Height = GetSystemMetrics(SM_CYSCREEN);

	deviceContext->RSSetViewports(1, &viewport);

	InitPipeline();
	InitGraphics();
}

void CleanD3D() {
	swapChain->SetFullscreenState(FALSE, NULL);

	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	pIBuffer->Release();
	swapChain->Release();
	backbuffer->Release();
	device->Release();
	deviceContext->Release();
}

// this function loads and prepares the shaders
void InitPipeline()
{
	// load and compile the two shaders
	ID3D10Blob *vsBlob, *psBlob;
	D3DX11CompileFromFile("resources/cube_vs.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &vsBlob, 0, 0);
	D3DX11CompileFromFile("resources/cube_ps.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &psBlob, 0, 0);

	// encapsulate both shaders into shader objects
	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &pVS);
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	deviceContext->VSSetShader(pVS, 0, 0);
	deviceContext->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC inElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(inElementDesc, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pLayout);
	deviceContext->IASetInputLayout(pLayout);
}

void InitGraphics() {
	VERTEX vertices[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f }
	};

	UINT indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(VERTEX) * 4;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	device->CreateBuffer(&bufferDesc, NULL, &pVBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	deviceContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
	deviceContext->Unmap(pVBuffer, NULL);

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(UINT) * 6;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	device->CreateBuffer(&bufferDesc, NULL, &pIBuffer);

	ZeroMemory(&mappedSubresource, sizeof(mappedSubresource));
	deviceContext->Map(pIBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	memcpy(mappedSubresource.pData, indices, sizeof(indices));
	deviceContext->Unmap(pIBuffer, NULL);

}
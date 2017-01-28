#include "pch.h"
#include "PlaneRenderer.h"
#include "Common\DirectXHelper.h"

using namespace holographicExperimentFramework;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

PlaneRenderer::PlaneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
}

void PlaneRenderer::PositionHologram(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose)
{
	if (pointerPose != nullptr)
	{
		const float3 headPosition = pointerPose->Head->Position;
		const float3 headDirection = pointerPose->Head->ForwardDirection;

		constexpr float distanceFromUser = 2.0f;
		const float3 gazeAtTwoMeters = headPosition + (distanceFromUser + headDirection);

		SetPosition(gazeAtTwoMeters);
	}
}

void PlaneRenderer::Update(const DX::StepTimer& timer)
{
	const XMMATRIX modelTransform = XMMatrixTranslation(0, 0, 0);
	XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));

	if (!m_loadingComplete)
		return;

	const auto context = m_deviceResources->GetD3DDeviceContext();

	context->UpdateSubresource(
		m_modelConstantBuffer.Get(),
		0,
		nullptr,
		&m_modelConstantBufferData,
		0,
		0);
}

void PlaneRenderer::CreateDeviceDependentResources()
{
	if (!m_loadingComplete)
	{
        return;
	}

    const auto context = m_deviceResources->GetD3DDeviceContext();

    const UINT stride = sizeof(VertexPositionColor);
    const UINT offset = 0;
    context->IASetVertexBuffers(
        0, 1, m_vertexBuffer.GetAddressOf(),
        &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetInputLayout(m_inputLayout.Get());

    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->VSSetConstantBuffers(0, 1, m_modelConstantBuffer.GetAddressOf());

    if (!m_usingVprtShaders)
    {
        context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
    }

    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    context->DrawIndexedInstanced(m_indexCount, 2, 0, 0, 0);
}

void PlaneRenderer::ReleaseDeviceDependentResources()
{

}

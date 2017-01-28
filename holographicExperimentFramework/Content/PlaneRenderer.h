#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderStructures.h"

namespace holographicExperimentFramework
{
	class PlaneRenderer
	{
	public:
		PlaneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(const DX::StepTimer& timer);

		void PositionHologram(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose);

		void SetPosition(Windows::Foundation::Numerics::float3 pos) { m_position = pos; }
		Windows::Foundation::Numerics::float3 GetPosition() const { return m_position; }

	private:

		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_modelConstantBuffer;

		ModelConstantBuffer m_modelConstantBufferData;
		uint32 m_indexCount = 0;

		bool m_loadingComplete = false;
		float m_degreesPerSecond = 45.f;
		Windows::Foundation::Numerics::float3 m_position = { 0.f, 0.f, -2.0f };

		bool m_usingVprtShaders = false;
	};
}

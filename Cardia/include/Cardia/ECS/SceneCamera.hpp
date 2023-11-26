#pragma once

#include "Cardia/Renderer/Camera.hpp"


namespace Cardia
{
	struct PerspectiveData
	{
		Radianf VerticalFOV {};
		float NearClip {}, FarClip {};

		constexpr static auto properties = std::make_tuple(
			property(&PerspectiveData::VerticalFOV, "VerticalFOV"),
			property(&PerspectiveData::NearClip, "NearClip"),
			property(&PerspectiveData::FarClip, "FarClip")
		);
	};

	struct OrthographicData
	{
		float Size {}, NearClip {}, FarClip {};

		constexpr static auto properties = std::make_tuple(
			property(&OrthographicData::Size, "Size"),
			property(&OrthographicData::NearClip, "NearClip"),
			property(&OrthographicData::FarClip, "FarClip")
		);
	};

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0, Orthographic = 1
		};

		SceneCamera();

		void SetPerspective(PerspectiveData data);
		PerspectiveData GetPerspective() const;
		void SetOrthographic(OrthographicData data);
		OrthographicData GetOrthographic() const;

		inline ProjectionType GetProjectionType() const { return m_ProjectionType; }
		inline void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecomputeProjection(); }

		inline void SetViewportSize(float width, float height) { m_AspectRatio = width / height; RecomputeProjection(); }

	private:
		void RecomputeProjection();

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		PerspectiveData m_PerspectiveData {Radianf::FromDegree(60.0f), 0.01f, 1000.0f};
		OrthographicData m_OrthographicData {10.0f, -1.0f, 1.0f};

		float m_AspectRatio = 0.0f;

	public:
		static constexpr auto properties = std::make_tuple(
			property(&SceneCamera::m_ProjectionType, "ProjectionType"),
			property(&SceneCamera::m_PerspectiveData, "PerspectiveData"),
			property(&SceneCamera::m_OrthographicData, "OrthographicData"),
			property(&SceneCamera::m_AspectRatio, "AspectRatio")
		);
	};
}
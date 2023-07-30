#pragma once

#include "Cardia/Renderer/Camera.hpp"


namespace Cardia
{
	struct PerspectiveData
	{
		Radianf VerticalFOV {};
		float NearClip {}, FarClip {};

		Json::Value Serialize() const;
		static std::optional<PerspectiveData> Deserialize(const Json::Value& root);
	};

	struct OrthographicData
	{
		float Size {}, NearClip {}, FarClip {};

		Json::Value Serialize() const;
		static std::optional<OrthographicData> Deserialize(const Json::Value& root);
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
	};
}
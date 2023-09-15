#include "RuntimeApp.hpp"
#include "Cardia/Project/Project.hpp"
#include "Cardia/Serialization/Serializer.hpp"
#include "Cardia/Asset/AssetsManager.hpp"

#include <Cardia.hpp>

RuntimeApp::RuntimeApp()
{
	auto project = Cardia::Project::Load("resources/build.cdproj");
	auto& config = project->GetConfig();
	if (const auto scene = Cardia::Serializer<Cardia::Scene>::Deserialize(Cardia::AssetsManager::GetAssetAbsolutePath(config.StartScene)))
	{
		m_CurrentScene = std::make_unique<Cardia::Scene>(*scene);
	} else
	{
		Cardia::Log::CoreWarn("Unable to load {0}", config.StartScene.string());
	}

	auto& window = RuntimeApp::GetWindow();
	m_CurrentScene->OnViewportResize(static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()));

	m_CurrentScene->OnRuntimeStart();
}

RuntimeApp::~RuntimeApp()
{
	m_CurrentScene->OnRuntimeStop();
}

void RuntimeApp::OnUpdate()
{
	Cardia::ScriptEngine::Instance().OnRuntimeUpdate();
}

void RuntimeApp::OnRender()
{
	if (const auto commandBuffer = m_Renderer.Begin())
	{
		// Offscreen rendering
		m_Renderer.BeginSwapChainRenderPass();

		// Render Scene
		m_CurrentScene->OnRuntimeRender(commandBuffer);

		m_Renderer.EndRenderPass();
		m_Renderer.End();
	}


}

void RuntimeApp::OnEvent(Cardia::Event &event)
{
	Cardia::EventDispatcher dispatcher(event);
	dispatcher.dispatch<Cardia::WindowResizeEvent>(CD_BIND_EVENT_FN(OnResize));
}

void RuntimeApp::OnResize(const Cardia::WindowResizeEvent &e)
{
	if (e.getW() == 0 || e.getH() == 0)
		return;
	m_CurrentScene->OnViewportResize(static_cast<float>(e.getW()), static_cast<float>(e.getH()));
}

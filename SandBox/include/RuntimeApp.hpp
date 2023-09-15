#pragma once

#include <Cardia.hpp>
#include <deque>
#include <random>


class RuntimeApp : public Cardia::Application
{
public:
	RuntimeApp();
	~RuntimeApp() override;

	void OnUpdate() override;
	void OnRender() override;
	void OnEvent(Cardia::Event& event) override;
	Cardia::Scene* GetCurrentScene() override{ return m_CurrentScene.get(); }

private:
	void OnResize(const Cardia::WindowResizeEvent& e);

	std::unique_ptr<Cardia::Scene> m_CurrentScene;
};
#include <Utopia.hpp>

class SandBox : public Utopia::Application
{
public:
	SandBox()
	{
		
	}

	~SandBox()
	{
		
	}
};


Utopia::Application* Utopia::CreateApplication()
{
	return new SandBox();
} 
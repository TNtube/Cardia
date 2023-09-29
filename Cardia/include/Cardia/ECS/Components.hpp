#pragma once

#include "Component/Camera.hpp"
#include "Component/Id.hpp"
#include "Component/Label.hpp"
#include "Component/Light.hpp"
#include "Component/ModelRenderer.hpp"
#include "Component/Relationship.hpp"
#include "Component/Script.hpp"
#include "Component/SpriteRenderer.hpp"
#include "Component/Transform.hpp"

namespace Cardia {
	template<typename... Components>
	struct ComponentGroup {};

	using AllComponents = ComponentGroup<
		Component::Relationship, Component::Label, Component::ID,
		Component::Transform, Component::SpriteRenderer, Component::ModelRenderer,
		Component::Camera, Component::Light, Component::Script
	>;

	using SerializableComponents = ComponentGroup<
		Component::Label, Component::ID, Component::Transform,
		Component::SpriteRenderer, Component::ModelRenderer, Component::Camera,
		Component::Light, Component::Script
	>;

	using ScriptableComponents = ComponentGroup<
		Component::Label, Component::ID, Component::Transform,
		Component::Camera, Component::Light>;
}

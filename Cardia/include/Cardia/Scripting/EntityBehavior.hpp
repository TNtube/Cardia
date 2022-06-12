#pragma once

namespace Cardia
{
        class EntityBehavior
        {
        public:
                EntityBehavior() = default;
                virtual ~EntityBehavior() = default;
                EntityBehavior(const Entity& entity) : m_Entity(entity) {}
                
                virtual void start() {}
                virtual void update() {}

                Entity m_Entity;
        };
}

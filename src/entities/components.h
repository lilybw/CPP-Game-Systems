#pragma once

#include <typeindex>
#include <glm/glm.hpp>
#include <functional>

#include <meta/processing.h>
#include <meta/ApplicationContext.h>

// Forward declarations
class IStandaloneEntityComponent;
class IDependentEntityComponent;
class IEntityComponent;

// Bounded generic A<B> where B : C : public IEntityComponent
template<typename T>
concept StandaloneComponent = std::derived_from<T, IStandaloneEntityComponent>;
// Bounded generic A<B> where B : C : public IEntityComponent
template<typename T>
concept DependentComponent = std::derived_from<T, IDependentEntityComponent>;

// This is some TS level of shenanigans
template<typename T>
concept AnyComponent = StandaloneComponent<T> || DependentComponent<T>;

// Function type for getting components of parent, or however the component happens to be used at time of instantiation
using ComponentRetriever = std::function<IEntityComponent*(const std::type_info&)>;

/** Base class for all components. Cannot be used in isolation however, 
 * make sure to use either IStandaloneEntityComponent or IDependentEntityComponent */
class IEntityComponent {
private:
    // Simple id for each component
    static inline long long s_id = 0;
    long long m_id;
public:
    IEntityComponent() : m_id(s_id++) {};
    virtual ~IEntityComponent() = default;
    
    long long getComponentId() const noexcept { return m_id; }
};

class IStandaloneEntityComponent : public IEntityComponent {
public:
    IStandaloneEntityComponent() = default;
    virtual ~IStandaloneEntityComponent() = default;
};
/**
 * Given the default behaviour of IEntity::addAnyComponent, the ComponentRetriever must be the first parameter in the constructor.
 */
class IDependentEntityComponent : public IEntityComponent {
public:
    IDependentEntityComponent(ComponentRetriever retriever) : m_getComponent(retriever) {
        if (m_getComponent == nullptr) {
            throw std::runtime_error("ComponentRetriever given to IDependentEntityComponent must be a valid function");
        }
    };
    virtual ~IDependentEntityComponent() = default;

private:
    ComponentRetriever m_getComponent;

protected:
    // Type-safe helper to get components
    template<AnyComponent T>
    T* requireComponent(const char* msg) const {
        IEntityComponent* component = m_getComponent(typeid(T));
        if (component == nullptr) {
            throw std::runtime_error(std::format("Malconfiguration of IEntityComponent: \n\t{}", msg));
        }
        return static_cast<T*>(component);
    }
};

class TransformComponent : public IStandaloneEntityComponent {
public:
    glm::fvec3 position = glm::fvec3(0.0f, 0.0f, 0.0f);
    glm::fvec2 scale = glm::fvec2(1.0f, 1.0f);
    float rotation = 0.0f;

    TransformComponent() {};
    TransformComponent(glm::fvec3 position) : position(position) {};
    TransformComponent(glm::fvec3 position, glm::fvec2 scale, float rotation) 
        : position(position), scale(scale), rotation(rotation) {};
};

class HealthComponent : public IStandaloneEntityComponent {
public:
    int health = 1;
    HealthComponent() {};
    HealthComponent(int health) : health(health) {};
};

// For gravity or other creative purposes
class ContinuousForceComponent : public IDependentEntityComponent, public ITickable {
private:
    TransformComponent* m_transform;
public:
    glm::fvec3 direction = glm::fvec3(0.0f, 0.0f, 0.0f);
    float force = 1.0f;

    ContinuousForceComponent(ComponentRetriever compRet, glm::fvec3 direction, float force) 
    : IDependentEntityComponent(compRet), direction(direction), force(force) {
        m_transform = requireComponent<TransformComponent>("ContinuousForceComponent requires a TransformComponent");
    };

    void tick(std::shared_ptr<ApplicationContext> ctx) noexcept override {
        m_transform->position += direction * (force * ctx->frames().deltaT());
    }
};

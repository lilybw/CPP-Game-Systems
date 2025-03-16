#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>
#include <functional>
#include <format>

#include <entities/components.h>
#include <types/types.h>

//Forward declaration
class IEntity;

template <typename T>
concept AnyEntity = std::derived_from<T, IEntity>;

/** Base class for all entities. Takes ownership of all added components unless explicitly released
 * after initialization.
*/
class IEntity {
public:
    IEntity() : m_id(s_id++) {};

    /**
     * @brief Add a component to the entity. If the entity already contains a component of the same typeid,
     * it will be replaced and the previous component will be freed.
     * To avoid freeing the previous component, use addComponentGetPrevious<T>(Args&&... args) instead to take ownership.
     */
    template <AnyComponent T, typename... Args>
    void addComponent(Args&&... args) {
        std::unique_ptr<T> previous = addComponentGetPrevious<T>(std::forward<Args>(args)...);

        if (previous != nullptr) {
            // Free previous component
            previous.reset();
        }
    }

    /**
     * @brief Add a component to the entity. If the entity already contains a component of the same typeid,
     * it will be replaced and the previous component will be returned.
     */
    template <AnyComponent T, typename... Args>
    std::unique_ptr<T> addComponentGetPrevious(Args&&... args) {
        return addAnyComponent<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Add a component to the entity and return a pointer to it. If the entity already contains a component of the same typeid,
     * it will be replaced and the previous component will be freed.
     * To avoid freeing the previous component, use addComponentAndGetPrevious<T>(Args&&... args) instead to take ownership.
     */
    template <AnyComponent T, typename... Args>
    T* addComponentAndGetRawPtr(Args&&... args) {
        // Add component, free previous, get raw pointer to new component
        addComponent<T>(std::forward<Args>(args)...);
        return getComponent<T>();
    }

    /**
     * @brief Retrieve a component from the entity. Returns nullptr if the Entity does not have such a component
     */
    template <AnyComponent T>
    [[nodiscard]] T* getComponent() const noexcept {
        return static_cast<T*>(getComponentByTypeInfo(typeid(T)));
    }

    /**
     * @brief Throw an error if a component does not exist, but is required.
     * Use requireComponent(const char* msg) to provide a custom error message.
     */
    template <AnyComponent T>
    [[nodiscard]] const T* requireComponent() const {
        T* component = getComponent<T>();
        if (component == nullptr) {
            throw std::runtime_error("Required component does not exist");
        }
        return component;
    }

    template <AnyComponent T>
    bool removeComponent() {
        return m_components.erase(typeid(T)) > 0;
    }

    /**
     * @brief Iterate over all components that can be cast to type T and apply the provided function
     * 
     * @tparam T The target type to filter by (can be any interface or base class)
     * @param func The function to apply to each matching component
     */
    template <typename T>
    void forEachComponent(std::function<void(T*)> func) const {
        for (const auto& [type, component] : m_components) {
            // Try to dynamic_cast the component to type T
            if (T* typedComponent = dynamic_cast<T*>(component.get())) {
                func(typedComponent);
            }
        }
    }

    long long getEntityId() const noexcept { return m_id; }

    void onDestruction(OnDestructionCallback<IEntity> callback) {
        m_destructionCallbacks.push_back(callback);
    }

    ~IEntity() {
        for (OnDestructionCallback<IEntity> callback : m_destructionCallbacks) {
            callback(this);
        }
    };

private:
    static inline long long s_id = 0;
    long long m_id;

    std::unordered_map<std::type_index, std::unique_ptr<IEntityComponent>> m_components;
    std::vector<OnDestructionCallback<IEntity>> m_destructionCallbacks;

    template<AnyComponent T, typename... Args>
    std::unique_ptr<T> addAnyComponent(Args&&... args) {
        std::unique_ptr<IEntityComponent> previous = releaseComponentByTypeInfo(typeid(T));
        
        std::unique_ptr<T> newComponent;
        if constexpr (DependentComponent<T>) {
            newComponent = std::make_unique<T>(m_retriever, std::forward<Args>(args)...);
        } else if constexpr (StandaloneComponent<T>) {
            newComponent = std::make_unique<T>(std::forward<Args>(args)...);
        } else {
            // This shouldnt ever happen, but just in case
            static_assert(StandaloneComponent<T> || DependentComponent<T>, 
                         "T must satisfy either StandaloneComponent or DependentComponent");
            return nullptr; // This line never executes due to static_assert
        }

        // Move ownership to map
        m_components.emplace(typeid(T), std::move(newComponent));

        if (previous != nullptr) {
            // According to Claude, there is no way to cast directly to unique_ptr<T>, so it is necessary to release it first
            // then cast the raw pointer, then back to unique_ptr<T> to maintain memory management
            T* derived = static_cast<T*>(previous.release());
            return std::unique_ptr<T>(derived);
        }
        
        return nullptr;
    }

    std::unique_ptr<IEntityComponent> releaseComponentByTypeInfo(const std::type_info& type) noexcept {
        auto it = m_components.find(type);
        if (it == m_components.end()) {
            return nullptr;
        }
        std::unique_ptr<IEntityComponent> ptr = std::move(it->second);
        m_components.erase(it);
        return ptr;
    }

    //Who needs type safety anyway? (In all seriousness, make sure that this is never exposed)
    [[nodiscard]] IEntityComponent* getComponentByTypeInfo(const std::type_info& type) const noexcept {
        auto it = m_components.find(type);
        return it == m_components.end() ? nullptr : static_cast<IEntityComponent*>(it->second.get());
    }

    ComponentRetriever m_retriever = [this](const std::type_info& type) -> IEntityComponent* {
        return this->getComponentByTypeInfo(type);
    };

    // Direct component addition without replacement check
    template <StandaloneComponent T, typename... Args>
    void addComponentDirect(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        m_components.emplace(typeid(T), std::move(ptr));
    }
};

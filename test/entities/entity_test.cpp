#include <gtest/gtest.h>
#include <iostream>

#include <entities/entity.h>
#include <entities/components.h>
#include <collisions/collider.h>


TEST(ECSTest, ComponentIDs) {
    
    long long baseline = TransformComponent{}.getComponentId();

    TransformComponent transform1{};
    ASSERT_EQ(transform1.getComponentId(), baseline + 1);
    
    HealthComponent health1{200};
    ASSERT_EQ(health1.getComponentId(), baseline + 2);
    
    TransformComponent transform2{};
    ASSERT_EQ(transform2.getComponentId(), baseline + 3);
    
    HealthComponent health2{200};
    ASSERT_EQ(health2.getComponentId(), baseline + 4);
    
    TransformComponent transform3{};
    ASSERT_EQ(transform3.getComponentId(), baseline + 5);
    
    HealthComponent health3{};
    ASSERT_EQ(health3.getComponentId(), baseline + 6);
}


TEST(ECSTest, AddAndGetComponent) {
    IEntity entity;
    glm::fvec3 position = glm::fvec3(1.0f, 2.0f, 3.0f);
    TransformComponent* returned = entity.addComponentAndGetRawPtr<TransformComponent>(position);

    // Test successful retrieval
    TransformComponent* retrieved = entity.getComponent<TransformComponent>();
    ASSERT_NE(retrieved, nullptr);
    EXPECT_FLOAT_EQ(retrieved->position.x, position.x);
    EXPECT_EQ(retrieved->getComponentId(), returned->getComponentId());
    
    // Test non-existent component
    ASSERT_EQ(entity.getComponent<HealthComponent>(), nullptr);
}

TEST(ECSTest, ReplaceComponent) {
    IEntity entity;
    glm::fvec3 positionA = glm::fvec3(1.0f, 2.0f, 3.0f);
    TransformComponent* firstTransform = entity.addComponentAndGetRawPtr<TransformComponent>(positionA);

    // Replace component
    glm::fvec3 positionB = glm::fvec3(4.0f, 5.0f, 6.0f);
    TransformComponent* previousTransform = entity.addComponentGetPrevious<TransformComponent>(positionB).get();
    
    // Verify replacement
    ASSERT_EQ(previousTransform->getComponentId(), firstTransform->getComponentId());
    ASSERT_FLOAT_EQ(entity.getComponent<TransformComponent>()->position.x, 4.0f);
}

TEST(ECSTest, MultipleComponents) {
    IEntity entity;
    entity.addComponent<TransformComponent>();
    entity.addComponent<HealthComponent>(200);

    // Verify both components exist
    ASSERT_NE(entity.getComponent<TransformComponent>(), nullptr);
    ASSERT_NE(entity.getComponent<HealthComponent>(), nullptr);
    EXPECT_EQ(entity.getComponent<HealthComponent>()->health, 200);
    //But is it consistent
    EXPECT_EQ(entity.getComponent<HealthComponent>()->health, 200);
}

TEST(ECSTest, EmptyEntity) {
    IEntity entity;
    ASSERT_EQ(entity.getComponent<TransformComponent>(), nullptr);
}

TEST(ECSTest, ComponentWithDependencies) {
    IEntity entity;

    try { 
        entity.addComponent<SphereCollider>(1.0f);
        FAIL() << "Expected exception: Theres no transform component yet" << std::endl;
    } catch (std::runtime_error& e) {
        // Check for the full error message
        EXPECT_STREQ(
            e.what(), 
            "Malconfiguration of IEntityComponent: \n\tAny collider requires a base TransformComponent"
        ) << "Expected matching description" << std::endl;
    }

    glm::fvec3 positionA = glm::fvec3(1.0f, 2.0f, 3.0f);
    entity.addComponent<TransformComponent>(positionA);

    // Adding a collider with TransformComponent present should succeed
    ASSERT_NO_THROW(
        entity.addComponent<SphereCollider>(1.0f)
    ) << "Expected matching description";
    ASSERT_NE(entity.getComponent<SphereCollider>(), nullptr) << "expected component to exist" << std::endl;
}

class TestTickableComponentA : public IStandaloneEntityComponent, public ITickable {
public:
    int tickCount = 0;
    void tick(std::shared_ptr<ApplicationContext> ctx) noexcept override {
        tickCount++;
    }
};

class TestTickableComponentB : public IStandaloneEntityComponent, public ITickable {
public:
    int tickCount = 0;
    void tick(std::shared_ptr<ApplicationContext> ctx) noexcept override {
        tickCount++;
    }
};

class TestTickableComponentC : public TestTickableComponentB { };

TEST(ECSTest, RangingOverComponents) {
    IEntity entity;
    TestTickableComponentA* ttCA = entity.addComponentAndGetRawPtr<TestTickableComponentA>();
    TestTickableComponentB* ttCB = entity.addComponentAndGetRawPtr<TestTickableComponentB>();
    TestTickableComponentC* ttCC = entity.addComponentAndGetRawPtr<TestTickableComponentC>();

    int expectedTickCount = 10;
    for (int i = 0; i < expectedTickCount; i++) {
        entity.forEachComponent<ITickable>([&](ITickable* tickable) {
            tickable->tick(nullptr);
        });
    }

    ASSERT_EQ(ttCA->tickCount, expectedTickCount);
    ASSERT_EQ(ttCB->tickCount, expectedTickCount);
    ASSERT_EQ(ttCC->tickCount, expectedTickCount);
}

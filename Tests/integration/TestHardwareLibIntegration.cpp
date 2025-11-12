#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-HardwareLib/Core/UHardwareLibrary.h"
#include <algorithm>
#include <vector>

using namespace RDK;
using namespace RDK::TestHelpers;

namespace {

std::string FindHardwareComponentName(UStorage& storage,
                                      std::initializer_list<const char*> preferredNames,
                                      std::initializer_list<const char*> fallbackPrefixes) {
    for (const auto* name : preferredNames) {
        if (storage.CheckClass(name)) {
            return name;
        }
    }

    std::vector<std::string> names;
    storage.GetClassNameList(names);
    for (const auto* prefix : fallbackPrefixes) {
        auto it = std::find_if(names.begin(), names.end(),
                               [&](const std::string& value) {
                                   return value.rfind(prefix, 0) == 0;
                               });
        if (it != names.end()) {
            return *it;
        }
    }
    return {};
}

} // namespace

class HardwareLibIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateStorageWithLibraries({&RDK::HardwareLibrary});
        RDK::HardwareLibrary.Upload(storage.get());
        RDK::HardwareLibrary.CreateClassSamples(storage.get());
        environment = CreateTestEnvironment(storage);
    }

    void TearDown() override {
        environment.reset();
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
    std::shared_ptr<UEnvironment> environment;
};

TEST_F(HardwareLibIntegrationTest, HardwareLibEngineIntegration) {
    ASSERT_NE(storage, nullptr);
    ASSERT_NE(environment, nullptr);
    EXPECT_GT(storage->GetNumClasses(), 0);
}

TEST_F(HardwareLibIntegrationTest, HardwareLibEnvironmentIntegration) {
    if (!environment->IsInit()) {
        environment->Default();
        environment->Init();
    }

    EXPECT_TRUE(environment->IsInit());
    UStorage* envStorage = environment->GetStorage();
    ASSERT_NE(envStorage, nullptr);
    EXPECT_EQ(envStorage, storage.get());
}

TEST_F(HardwareLibIntegrationTest, HardwareLibModelCreation) {
    if (!environment->IsInit()) {
        environment->Default();
        environment->Init();
    }

    auto model = CreateTestModel(storage, environment);
    if (!model) {
        GTEST_SKIP() << "Cannot create model in environment";
        return;
    }

    const std::string controllerName =
        FindHardwareComponentName(*storage, {"UArduinoControl", "UDcControlDemo"}, {"UArduino", "UDc"});
    if (controllerName.empty()) {
        GTEST_SKIP() << "No hardware control components registered";
        return;
    }

    auto controller = storage->TakeObject(controllerName);
    ASSERT_NE(controller, nullptr);
    controller->SetName("HardwareController");
    ASSERT_TRUE(model->AddComponent(controller));

    EXPECT_GE(model->GetNumComponents(), 1);
}


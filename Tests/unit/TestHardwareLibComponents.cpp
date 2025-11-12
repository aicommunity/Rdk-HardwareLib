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

class HardwareLibComponentsTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateStorageWithLibraries({&RDK::HardwareLibrary});
        RDK::HardwareLibrary.Upload(storage.get());
        RDK::HardwareLibrary.CreateClassSamples(storage.get());
    }

    void TearDown() override {
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
};

TEST_F(HardwareLibComponentsTest, RegistersArduinoControl) {
    ASSERT_NE(storage, nullptr);
    EXPECT_TRUE(storage->CheckClass("UArduinoControl")) << "UArduinoControl should be registered";
}

TEST_F(HardwareLibComponentsTest, CreateHardwareController) {
    const std::string className =
        FindHardwareComponentName(*storage, {"UArduinoControl", "UDcControlDemo"}, {"UArduino", "UDc"});
    if (className.empty()) {
        GTEST_SKIP() << "Hardware library did not expose controller components";
        return;
    }

    auto controller = storage->TakeObject(className);
    ASSERT_NE(controller, nullptr);
    controller->SetName("HardwareController");
    EXPECT_EQ(controller->GetName(), "HardwareController");
    EXPECT_TRUE(storage->CheckObject(controller));
    storage->ReturnObject(controller);
}

TEST_F(HardwareLibComponentsTest, CreateSensorComponent) {
    const std::string className =
        FindHardwareComponentName(*storage, {"UAdcSensor"}, {"UAdc", "USensor"});
    if (className.empty()) {
        GTEST_SKIP() << "Hardware library did not expose sensor components";
        return;
    }

    auto sensor = storage->TakeObject(className);
    ASSERT_NE(sensor, nullptr);
    EXPECT_FALSE(sensor->GetCompClassName().empty());
    EXPECT_TRUE(sensor->Activity);
    storage->ReturnObject(sensor);
}


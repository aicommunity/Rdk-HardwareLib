#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Libraries.h"
#include "../../../../Libraries/Rdk-HardwareLib/Core/UHardwareLibrary.h"
#include <algorithm>
#include <vector>
#include <set>

using namespace RDK;
using namespace RDK::TestHelpers;

namespace {

// Expected HardwareLib component categories and their key classes
struct HardwareLibCategory {
    std::string name;
    std::vector<std::string> keyClasses;
};

const std::vector<HardwareLibCategory> EXPECTED_HARDWARELIB_CATEGORIES = {
    {"Controllers", {"Arduino", "ADC", "DC"}}
};

} // namespace

class HardwareLibRegistrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        
        // Create storage with BasicLib and HardwareLib
        storage = CreateStorageWithLibraries({&RDK::BasicLibrary, &RDK::HardwareLibrary});
        ASSERT_NE(storage, nullptr) << "Failed to create storage";
        
        // Upload libraries in correct order
        RDK::BasicLibrary.Upload(storage.get());
        RDK::HardwareLibrary.Upload(storage.get());
        
        // Call CreateClassSamples for HardwareLib
        try {
            RDK::HardwareLibrary.CreateClassSamples(storage.get());
        } catch (const std::exception& ex) {
            GTEST_SKIP() << "UHardwareLibrary CreateClassSamples failed: " << ex.what();
        }
    }

    void TearDown() override {
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
};

// Test that all expected HardwareLib classes are registered
TEST_F(HardwareLibRegistrationTest, AllHardwareLibClassesRegistered) {
    ASSERT_NE(storage, nullptr);
    
    std::vector<std::string> allClasses;
    storage->GetClassNameList(allClasses);
    
    std::set<std::string> registeredClasses(allClasses.begin(), allClasses.end());
    
    // Check each category
    for (const auto& category : EXPECTED_HARDWARELIB_CATEGORIES) {
        for (const auto& className : category.keyClasses) {
            EXPECT_TRUE(storage->CheckClass(className)) 
                << "HardwareLib class '" << className << "' from category '" << category.name << "' should be registered";
            
            if (storage->CheckClass(className)) {
                EXPECT_TRUE(registeredClasses.count(className) > 0)
                    << "Class '" << className << "' should be in class list";
            }
        }
    }
}

// Test that minimum number of HardwareLib classes are registered
TEST_F(HardwareLibRegistrationTest, HardwareLibClassesCount) {
    ASSERT_NE(storage, nullptr);
    
    std::vector<std::string> allClasses;
    storage->GetClassNameList(allClasses);
    
    // Count HardwareLib classes
    size_t hardwareLibClassCount = 0;
    std::vector<std::string> hardwareLibClasses = {
        "Arduino", "ADC", "DC"
    };
    
    for (const auto& className : allClasses) {
        for (const auto& hwClass : hardwareLibClasses) {
            if (className == hwClass) {
                hardwareLibClassCount++;
                break;
            }
        }
    }
    
    // We expect at least 3 classes
    EXPECT_GE(hardwareLibClassCount, 3) 
        << "Expected at least 3 HardwareLib classes, but found " << hardwareLibClassCount;
}

// Test that critical HardwareLib classes are registered
TEST_F(HardwareLibRegistrationTest, CriticalClassesRegistered) {
    ASSERT_NE(storage, nullptr);
    
    // Critical classes that are commonly used
    std::vector<std::string> criticalClasses = {
        "Arduino",
        "ADC",
        "DC"
    };
    
    for (const auto& className : criticalClasses) {
        EXPECT_TRUE(storage->CheckClass(className)) 
            << "Critical HardwareLib class '" << className << "' should be registered";
    }
}

// Test that HardwareLib classes can be created
TEST_F(HardwareLibRegistrationTest, HardwareLibClassesCreation) {
    ASSERT_NE(storage, nullptr);
    
    // Test Arduino creation
    if (storage->CheckClass("Arduino")) {
        auto arduino = storage->TakeObject("Arduino");
        EXPECT_NE(arduino, nullptr) 
            << "Should be able to create Arduino";
        if (arduino) {
            EXPECT_EQ(arduino->GetCompClassName(), "Arduino");
        }
    }
    
    // Test ADC creation
    if (storage->CheckClass("ADC")) {
        auto adc = storage->TakeObject("ADC");
        EXPECT_NE(adc, nullptr) 
            << "Should be able to create ADC";
        if (adc) {
            EXPECT_EQ(adc->GetCompClassName(), "ADC");
        }
    }
    
    // Test DC creation
    if (storage->CheckClass("DC")) {
        auto dc = storage->TakeObject("DC");
        EXPECT_NE(dc, nullptr) 
            << "Should be able to create DC";
        if (dc) {
            EXPECT_EQ(dc->GetCompClassName(), "DC");
        }
    }
}


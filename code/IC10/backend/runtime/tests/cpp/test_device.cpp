// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file test_device.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/10/05
 * @brief 设备型号（`#>` 设备块）与模拟设备测试
 * @details 覆盖型号表的构建、型号化设备与无型号设备的语义差异、活动成员子集，以及端口按源码
 *          声明自动绑定。用例按等价类与边界值划分：无型号 / 有型号但空成员 / 声明成员 / 未声明成员 /
 *          活动子集（含空集）/ 静态端口与动态端口。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler/locals/languages/zh_hans.hpp"
#include "ic10_runtime/locals/languages/en_us.hpp"
#include "ic10_compiler/semantic/analyser.hpp"
#include "ic10_compiler/parser/parser.hpp"
#include "ic10_compiler/lexer/lexer.hpp"
#include "ic10_runtime/device.hpp"
#include "ic10_runtime/device_type.hpp"
#include "ic10_runtime/engine.hpp"

#include <gtest/gtest.h>

using namespace stationeers::ic10;

namespace {

    /// 测试用型号：两条逻辑属性 + 一个槽位 + 型号/名称哈希
    DeviceType sensorType() {
        DeviceType type;

        type.name       = "Sensor";
        type.deviceHash = 12345;
        type.nameHash   = 777;

        type.members = {
            {"deviceHash", "12345", "hash"},
            {"Pressure",   "5",     "logic"},
            {"Setting",    "12",    "logic"},
            {"Slot0",      "0",     "slot" },
        };

        return type;
    }

    /// 只含两条逻辑属性的型号，便于断言活动子集
    DeviceType twoLogicType() {
        DeviceType type;

        type.name    = "Sensor";
        type.members = {{"Pressure", "5", "logic"}, {"Setting", "12", "logic"}};

        return type;
    }

    /// 声明了默认值的型号：一条有默认值（`@logic Setting 12 1`）、一条没有（`@logic Pressure 5`）
    DeviceType defaultedType() {
        DeviceType type;

        type.name    = "Sensor";
        type.members = {{"Pressure", "5", "logic"}, {"Setting", "12", "logic", 1.0}};

        return type;
    }

}  // namespace

class DeviceTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        ICLoc::registerLanguage<ZhHans>("en_us");
        ICLoc::setLanguage("en_us");
        IRLoc::registerLanguage<EnUs>("en_us");
        IRLoc::setLanguage("en_us");
    }

    /// 编译源码并建立引擎（执行上下文会在构造时构建型号表并绑定声明了型号的端口）
    void compile(const std::string& source) {
        auto tokens = Lexer::tokenize(source);
        ast_        = Parser::parsing(tokens);
        analyser_   = std::make_shared<Analyser>();
        analyser_->visit(ast_).getFuture().get();

        Config cfg;
        engine_ = std::make_unique<Engine>(ast_, analyser_->getSymbolTable(), cfg, analyser_->getTypeTable());
    }

    [[nodiscard]] const DeviceRegistry& registry() const { return engine_->getContext().manager.registry(); }

    [[nodiscard]] Manager& manager() { return engine_->getContext().manager; }

    Program ast_;
    std::shared_ptr<Analyser> analyser_;
    std::unique_ptr<Engine> engine_;
};

// ============================================================
// 等价类：无型号设备
// ============================================================

TEST_F(DeviceTestFixture, UntypedDeviceKeepsVirtualSemantics) {
    // 无型号：不约束成员，读写与哈希都与 VirtualDevice 一致（未声明型号的程序语义不变）
    SimDevice device;

    EXPECT_TRUE(device.typeName().empty());
    EXPECT_EQ(device.getTypeHash(), 0);
    EXPECT_EQ(device.getNameHash(), 0);
    EXPECT_FALSE(device.canReadLogic("Setting"));
    EXPECT_TRUE(device.canWriteLogic("Setting"));
    EXPECT_DOUBLE_EQ(device.readLogic("Setting"), 0.0);
    EXPECT_TRUE(device.snapshot().empty());

    device.writeLogic("Setting", 3.0);

    EXPECT_DOUBLE_EQ(device.readLogic("Setting"), 3.0);
    EXPECT_TRUE(device.canReadLogic("Setting"));

    // 成员表只反映程序写过的属性，且标记为未声明
    const auto views = device.snapshot();

    ASSERT_EQ(views.size(), 1u);
    EXPECT_EQ(views[0].name, "Setting");
    EXPECT_FALSE(views[0].isDeclared);
    EXPECT_TRUE(views[0].assigned);
    ASSERT_TRUE(views[0].value.has_value());
    EXPECT_DOUBLE_EQ(*views[0].value, 3.0);
}

TEST_F(DeviceTestFixture, EmptyTypedDeviceIsStillLenient) {
    // 边界：注解块没有任何声明成员 → 视为无型号，不约束成员（否则该端口将无法读写任何属性）
    DeviceType type;
    type.name = "Empty";

    SimDevice device(type);

    EXPECT_EQ(device.typeName(), "Empty");
    EXPECT_TRUE(device.canWriteLogic("Anything"));
    EXPECT_FALSE(device.canReadLogic("Anything"));
}

// ============================================================
// 等价类：型号化设备
// ============================================================

TEST_F(DeviceTestFixture, TypedDeviceDeclaresMembersAndIdentity) {
    SimDevice device(sensorType());

    EXPECT_EQ(device.typeName(), "Sensor");
    EXPECT_EQ(device.getTypeHash(), 12345);
    EXPECT_EQ(device.getNameHash(), 777);

    // 声明但从未赋过值：读回 0、canReadLogic 为 false（= 没被赋过值），可写
    EXPECT_FALSE(device.canReadLogic("Pressure"));
    EXPECT_TRUE(device.canWriteLogic("Pressure"));
    EXPECT_DOUBLE_EQ(device.readLogic("Pressure"), 0.0);

    // 未声明的成员：执行语义与无型号设备一致（型号不参与执行），但快照里标记为未声明
    device.writeLogic("Color", 7.0);
    EXPECT_DOUBLE_EQ(device.readLogic("Color"), 7.0);

    const auto views = device.snapshot();

    // 4 条声明成员（顺序与注解一致）+ 1 条未声明成员
    ASSERT_EQ(views.size(), 5u);
    EXPECT_EQ(views[0].name, "deviceHash");
    EXPECT_EQ(views[0].kind, "hash");
    EXPECT_EQ(views[0].declaredValue, "12345");
    EXPECT_FALSE(views[0].assigned);
    EXPECT_EQ(views[1].name, "Pressure");
    EXPECT_TRUE(views[1].isDeclared);
    EXPECT_FALSE(views[1].assigned);
    EXPECT_FALSE(views[1].value.has_value());
    EXPECT_EQ(views[3].name, "Slot0");
    EXPECT_EQ(views[3].kind, "slot");
    EXPECT_FALSE(views[3].assigned);
    EXPECT_EQ(views[4].name, "Color");
    EXPECT_FALSE(views[4].isDeclared);
    EXPECT_TRUE(views[4].assigned);
    ASSERT_TRUE(views[4].value.has_value());
    EXPECT_DOUBLE_EQ(*views[4].value, 7.0);
}

TEST_F(DeviceTestFixture, AssignedZeroIsDistinctFromNeverAssigned) {
    // 边界：赋值为 0 也是"有值"，与从未赋值必须区分开（设备默认值不一定是 0）
    SimDevice device(twoLogicType());

    device.writeLogic("Pressure", 0.0);

    const auto views = device.snapshot();

    ASSERT_EQ(views.size(), 2u);
    EXPECT_TRUE(views[0].assigned);
    ASSERT_TRUE(views[0].value.has_value());
    EXPECT_DOUBLE_EQ(*views[0].value, 0.0);
    EXPECT_FALSE(views[1].assigned);
    EXPECT_FALSE(views[1].value.has_value());
}

// ============================================================
// 等价类：注解声明的默认值
// ============================================================

TEST_F(DeviceTestFixture, DeclaredDefaultAnswersBeforeAssignment) {
    // 声明了默认值的成员：赋值前就以默认值作答（默认为 1，不能拿 0 顶替）
    SimDevice device(defaultedType());

    EXPECT_TRUE(device.canReadLogic("Setting"));
    EXPECT_DOUBLE_EQ(device.readLogic("Setting"), 1.0);

    // 快照如实区分"声明的默认值"与"此刻的值"：未赋值 → value 为空、defaultValue 有值
    const auto views = device.snapshot();

    ASSERT_EQ(views.size(), 2u);
    EXPECT_EQ(views[1].name, "Setting");
    ASSERT_TRUE(views[1].defaultValue.has_value());
    EXPECT_DOUBLE_EQ(*views[1].defaultValue, 1.0);
    EXPECT_FALSE(views[1].assigned);
    EXPECT_FALSE(views[1].value.has_value());
}

TEST_F(DeviceTestFixture, AssignmentOverridesDeclaredDefault) {
    // 赋过值后，真实取值盖过默认值（默认值只是兜底）
    SimDevice device(defaultedType());

    device.writeLogic("Setting", 9.0);

    EXPECT_DOUBLE_EQ(device.readLogic("Setting"), 9.0);

    const auto views = device.snapshot();

    ASSERT_EQ(views.size(), 2u);
    EXPECT_TRUE(views[1].assigned);
    ASSERT_TRUE(views[1].value.has_value());
    EXPECT_DOUBLE_EQ(*views[1].value, 9.0);
    ASSERT_TRUE(views[1].defaultValue.has_value());
    EXPECT_DOUBLE_EQ(*views[1].defaultValue, 1.0);
}

TEST_F(DeviceTestFixture, MemberWithoutDefaultKeepsVirtualSemantics) {
    // 边界：没声明默认值的成员与无型号设备一致——读回 0、canReadLogic 为 false、defaultValue 为空
    SimDevice device(defaultedType());

    EXPECT_FALSE(device.canReadLogic("Pressure"));
    EXPECT_DOUBLE_EQ(device.readLogic("Pressure"), 0.0);

    const auto views = device.snapshot();

    ASSERT_EQ(views.size(), 2u);
    EXPECT_EQ(views[0].name, "Pressure");
    EXPECT_FALSE(views[0].defaultValue.has_value());
    EXPECT_FALSE(views[0].assigned);
    EXPECT_FALSE(views[0].value.has_value());
}

// ============================================================
// 型号表
// ============================================================

TEST_F(DeviceTestFixture, RegistryKeepsDeviceAnnotationsOnly) {
    compile(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @device-hash 12345\n"
        "#> @name-hash 777\n"
        "#> @logic Setting 12\n"
        "#> @logic-slot Quantity 3\n"
        "#> @slot Slot0 0\n"
        "#> @reagent-hash 1680000\n"
        "#> @end-device\n"
        "#> @enum\n"
        "#> @name Color\n"
        "#> @value Green 2\n"
        "#> @end-enum\n"
        "hcf\n"
    );

    const DeviceType* sensor = registry().find("Sensor");

    ASSERT_NE(sensor, nullptr);
    EXPECT_EQ(sensor->deviceHash, 12345);
    EXPECT_EQ(sensor->nameHash, 777);
    ASSERT_EQ(sensor->members.size(), 6u);
    EXPECT_EQ(sensor->members[0].name, "deviceHash");
    EXPECT_EQ(sensor->members[0].kind, "hash");
    EXPECT_EQ(sensor->members[1].name, "nameHash");
    EXPECT_EQ(sensor->members[1].kind, "hash");
    EXPECT_EQ(sensor->members[2].name, "Setting");
    EXPECT_EQ(sensor->members[2].kind, "logic");
    EXPECT_EQ(sensor->members[3].name, "Quantity");
    EXPECT_EQ(sensor->members[3].kind, "logic-slot");
    EXPECT_EQ(sensor->members[4].name, "Slot0");
    EXPECT_EQ(sensor->members[4].kind, "slot");
    EXPECT_EQ(sensor->members[5].kind, "reagent-hash");
    EXPECT_EQ(sensor->members[5].value, "1680000");

    // 枚举注解不是设备型号；未声明的名字也查不到
    EXPECT_EQ(registry().find("Color"), nullptr);
    EXPECT_EQ(registry().find("Nonexistent"), nullptr);
}

TEST_F(DeviceTestFixture, RegistryHashDefaultsToZero) {
    // 边界：注解没写哈希行时型号哈希为 0（与无型号设备一致）
    compile("#> @device\n#> @name Sensor\n#> @logic Setting 12\n#> @end-device\nhcf\n");

    const DeviceType* sensor = registry().find("Sensor");

    ASSERT_NE(sensor, nullptr);
    EXPECT_EQ(sensor->deviceHash, 0);
    EXPECT_EQ(sensor->nameHash, 0);
    EXPECT_TRUE(sensor->contains("Setting"));
    EXPECT_FALSE(sensor->contains("Pressure"));
}

TEST_F(DeviceTestFixture, RegistryKeepsDeclaredDefaults) {
    // 成员行末尾追加的取值即默认值（不写就没有默认值，而不是 0）
    compile(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Setting 12 1\n"
        "#> @logic Pressure 5\n"
        "#> @logic-slot Quantity 3 0\n"
        "#> @end-device\n"
        "hcf\n"
    );

    const DeviceType* sensor = registry().find("Sensor");

    ASSERT_NE(sensor, nullptr);

    const DeviceMemberDecl* setting = sensor->findMember("Setting");
    const DeviceMemberDecl* pressure = sensor->findMember("Pressure");
    const DeviceMemberDecl* quantity = sensor->findMember("Quantity");

    ASSERT_NE(setting, nullptr);
    ASSERT_NE(pressure, nullptr);
    ASSERT_NE(quantity, nullptr);

    ASSERT_TRUE(setting->defaultValue.has_value());
    EXPECT_DOUBLE_EQ(*setting->defaultValue, 1.0);
    EXPECT_FALSE(pressure->defaultValue.has_value());
    ASSERT_TRUE(quantity->defaultValue.has_value());
    EXPECT_DOUBLE_EQ(*quantity->defaultValue, 0.0);
}

// ============================================================
// 端口自动绑定（源码声明型号 → 无需宿主注册即可读写）
// ============================================================

TEST_F(DeviceTestFixture, DeclaredTypeBindsPortAutomatically) {
    compile(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @device-hash 12345\n"
        "#> @logic Setting 12\n"
        "#> @end-device\n"
        "alias sensor d0 #: @type Sensor\n"
        "s sensor Setting 5\n"
        "l r0 sensor Setting\n"
        "hcf\n"
    );

    // 端口 d0 在上下文构造时已按别名声明的型号绑定，不需要宿主调用 setExternalDevice
    IDevice* device = manager().getDevice("d0");

    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->typeName(), "Sensor");
    EXPECT_EQ(device->getTypeHash(), 12345);
    EXPECT_EQ(manager().ports(), (std::vector<std::string>{"d0"}));

    engine_->runFull();

    EXPECT_DOUBLE_EQ(device->readLogic("Setting"), 5.0);
    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r0"), 5.0);
}

TEST_F(DeviceTestFixture, DynamicPortAliasIsNotBound) {
    // 边界：动态端口别名（dr0）在运行期才落到具体端口，构造时不预绑定
    compile(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Setting 12\n"
        "#> @end-device\n"
        "alias dyn dr0 #: @type Sensor\n"
        "hcf\n"
    );

    EXPECT_EQ(manager().getDevice("dr0"), nullptr);
    EXPECT_TRUE(manager().ports().empty());
}

TEST_F(DeviceTestFixture, BindTypedOverridesDeclaredType) {
    // 宿主显式指定型号：端口换型号、状态重置
    compile(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Setting 12\n"
        "#> @end-device\n"
        "#> @device\n"
        "#> @name Pump\n"
        "#> @logic Setting 3\n"
        "#> @logic On 28\n"
        "#> @end-device\n"
        "alias dev d0 #: @type Sensor\n"
        "hcf\n"
    );

    IDevice* device = manager().bindTyped("d0", "Pump");

    ASSERT_NE(device, nullptr);
    EXPECT_EQ(device->typeName(), "Pump");

    // 换型号后声明成员跟着变（内省看得到），但都需要被赋过值才有值
    const auto* pump = dynamic_cast<const SimDevice*>(device);

    ASSERT_NE(pump, nullptr);
    EXPECT_TRUE(pump->type().contains("On"));
    EXPECT_TRUE(pump->type().contains("Setting"));
    EXPECT_FALSE(pump->type().contains("Pressure"));
    EXPECT_FALSE(device->canReadLogic("On"));

    // 型号表里没有的名字：退化为无型号设备，但型号名保留
    IDevice* unknown = manager().bindTyped("d1", "Nonexistent");

    ASSERT_NE(unknown, nullptr);
    EXPECT_EQ(unknown->typeName(), "Nonexistent");
    EXPECT_EQ(unknown->getTypeHash(), 0);
    EXPECT_TRUE(unknown->canWriteLogic("Anything"));
    EXPECT_EQ(manager().ports(), (std::vector<std::string>{"d0", "d1"}));
}

// ============================================================
// 类型提示上的默认值（`#: @default 分组 字段 值` / `#: @default 值`）
// ============================================================

TEST_F(DeviceTestFixture, HintDefaultOverridesAnnotationDefault) {
    // 同一型号的不同端口各有自己的默认值：提示覆写注解声明的默认值
    compile(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Setting 12 1\n"
        "#> @end-device\n"
        "alias sensor d0 #: @type Sensor @default logic Setting 3\n"
        "hcf\n"
    );

    IDevice* device = manager().getDevice("d0");

    ASSERT_NE(device, nullptr);
    EXPECT_TRUE(device->canReadLogic("Setting"));
    EXPECT_DOUBLE_EQ(device->readLogic("Setting"), 3.0);
}

TEST_F(DeviceTestFixture, HintDefaultForUndeclaredMemberIsIgnored) {
    // 边界：注解没声明过的成员不接受默认值——注解给出的字段范围就是默认值的落点
    compile(
        "#> @device\n"
        "#> @name Sensor\n"
        "#> @logic Setting 12\n"
        "#> @end-device\n"
        "alias sensor d0 #: @type Sensor @default logic Pressure 3\n"
        "hcf\n"
    );

    IDevice* device = manager().getDevice("d0");

    ASSERT_NE(device, nullptr);
    EXPECT_FALSE(device->canReadLogic("Pressure"));
    EXPECT_DOUBLE_EQ(device->readLogic("Pressure"), 0.0);
    EXPECT_FALSE(device->canReadLogic("Setting"));
}

TEST_F(DeviceTestFixture, RegisterDefaultSeedsRegister) {
    // 寄存器默认值：没写过也读得到默认值（而不是 0）
    compile("alias counter r1 #: @default 7\nl r0 counter\nhcf\n");

    engine_->runFull();

    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r0"), 7.0);
}

TEST_F(DeviceTestFixture, DynamicRegisterDefaultIsNotSeeded) {
    // 边界：动态寄存器（rr0）的目标编号构造期无法确定，默认值不播种
    compile("alias counter rr0 #: @default 7\nl r1 counter\nhcf\n");

    engine_->runFull();

    EXPECT_DOUBLE_EQ(engine_->getContext().memory.getReg("r1"), 0.0);
}

TEST_F(DeviceTestFixture, HintDefaultIgnoredWithoutDeclaredType) {
    // 边界：端口没有声明型号（没有 `@type`）时不绑定设备，也就没有可覆写的成员范围
    compile("alias sensor d0 #: @default logic Setting 3\nhcf\n");

    EXPECT_EQ(manager().getDevice("d0"), nullptr);
}


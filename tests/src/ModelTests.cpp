// Copyright (c) 2018-2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#include <iostream>

#include "Test.h"

#include "Models/BatteryModel.h"
#include "Models/SystemModel.h"
#include "Tank3.h"

using namespace PCOE;
using namespace PCOE::Test;

namespace ModelTests {
    void testTankInitialize() {
        // Create Tank3 model
        Tank3 TankModel = Tank3();

        // Initialize it
        auto u = TankModel.getInputVector();
        auto z = TankModel.getOutputVector();
        auto x = TankModel.initialize(u, z);

        // Check all states set to zero
        Assert::AreEqual(0, x[0], 1e-12);
        Assert::AreEqual(0, x[1], 1e-12);
        Assert::AreEqual(0, x[2], 1e-12);
    }

    void testTankStateEqn() {
        // Create Tank3 model
        Tank3 TankModel = Tank3();

        // Set parameter values
        TankModel.parameters.K1 = 1;
        TankModel.parameters.K2 = 2;
        TankModel.parameters.K3 = 3;
        TankModel.parameters.R1 = 1;
        TankModel.parameters.R2 = 2;
        TankModel.parameters.R3 = 3;
        TankModel.parameters.R1c2 = 1;
        TankModel.parameters.R2c3 = 2;

        // Set up u (input flows)
        auto u = TankModel.getInputVector();
        u[0] = 1;
        u[1] = 1;
        u[2] = 1;

        // Set up x (masses)
        auto x = TankModel.getStateVector();
        x[0] = 0;
        x[1] = 0;
        x[2] = 0;

        // Set up state noise
        std::vector<double> ns;
        double nsValue = 0;
        ns.push_back(nsValue);
        ns.push_back(nsValue);
        ns.push_back(nsValue);

        // Step state equation
        double t = 0;
        TankModel.setDefaultTimeStep(0.1);
        x = TankModel.stateEqn(t, x, u, ns);

        // Check values of x
        Assert::AreEqual(0.1, x[0], 1e-12);
        Assert::AreEqual(0.1, x[1], 1e-12);
        Assert::AreEqual(0.1, x[2], 1e-12);

        // Check that dt was set properly
        Assert::AreEqual(0.1, TankModel.getDefaultTimeStep(), 1e-12);
    }

    void testTankOutputEqn() {
        // Create Tank3 model
        Tank3 TankModel = Tank3();

        // Set parameter values
        TankModel.parameters.K1 = 1;
        TankModel.parameters.K2 = 2;
        TankModel.parameters.K3 = 3;
        TankModel.parameters.R1 = 1;
        TankModel.parameters.R2 = 2;
        TankModel.parameters.R3 = 3;
        TankModel.parameters.R1c2 = 1;
        TankModel.parameters.R2c3 = 2;

        // Set up u (input flows)
        auto u = TankModel.getInputVector();
        u[0] = 1;
        u[1] = 1;
        u[2] = 1;

        // Set up x (masses)
        auto x = TankModel.getStateVector();
        x[0] = 0.1;
        x[1] = 0.1;
        x[2] = 0.1;

        // Set up output noise
        std::vector<double> no;
        double noValue = 0;
        no.push_back(noValue);
        no.push_back(noValue);
        no.push_back(noValue);

        // Output equation
        double t = 0;
        auto z = TankModel.outputEqn(t, x, no);

        // Check values of z
        Assert::AreEqual(0.1, z[0], 1e-12);
        Assert::AreEqual(0.05, z[1], 1e-12);
        Assert::AreEqual(1.0 / 30.0, z[2], 1e-12);
    }

    void testBatterySetParameters() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Set parameters to defaults
        battery.setParameters(7500);

        // Check a few parameter values
        Assert::AreEqual(1250, battery.parameters.qpSMax, 1e-3);
        Assert::AreEqual(5000, battery.parameters.qpMin, 1e-12);
        Assert::AreEqual(12500, battery.parameters.qpMax, 1e-12);
    }

    void testBatteryInitialization() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Initialize
        auto u0 = BatteryModel::input_type({0.4});
        auto z0 = BatteryModel::output_type({20, 4.0});
        auto x = battery.initialize(u0, z0);

        // Check states
        Assert::AreEqual(293.15, x[battery.indices.states.Tb], 1e-12);
        Assert::AreEqual(battery.parameters.Ro * 0.1, x[battery.indices.states.Vo], 1e-12);
        Assert::AreEqual(0, x[battery.indices.states.Vsn], 1e-12);
        Assert::AreEqual(0, x[battery.indices.states.Vsp], 1e-12);
        Assert::IsTrue(x[battery.indices.states.qnB] > 5.62e3 &&
                       x[battery.indices.states.qnB] < 5.63e3);
        Assert::IsTrue(x[battery.indices.states.qpB] > 5.771e3 &&
                       x[battery.indices.states.qpB] < 5.772e3);
        Assert::IsTrue(x[battery.indices.states.qnS] > 6.2535e2 &&
                       x[battery.indices.states.qnS] < 6.2536e2);
        Assert::IsTrue(x[battery.indices.states.qpS] > 6.413e2 &&
                       x[battery.indices.states.qpS] < 6.4132e2);
    }

    void testBatteryStateEqn() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Initialize
        auto u0 = BatteryModel::input_type({0.4});
        auto z0 = BatteryModel::output_type({20, 4.0});
        auto x = battery.initialize(u0, z0);

        // Set noise vector
        std::vector<double> zeroNoise(8);

        // Set input vector
        auto u = BatteryModel::input_type({1});

        // Compute next state
        x = battery.SystemModel::stateEqn(0, x, u, zeroNoise);

        // Check states
        Assert::AreEqual(293.15, x[battery.indices.states.Tb], 1e-12);
        Assert::IsTrue(x[battery.indices.states.Vo] > 0.01461 &&
                       x[battery.indices.states.Vo] < 0.14611);
        Assert::IsTrue(x[battery.indices.states.Vsn] > 1.34338e-5 &&
                       x[battery.indices.states.Vsn] < 1.34339e-5);
        Assert::AreEqual(3.5303160541594348e-06, x[battery.indices.states.Vsp], 1e-11);
        Assert::IsTrue(x[battery.indices.states.qnB] > 5.62818e3 &&
                       x[battery.indices.states.qnB] < 5.62819e3);
        Assert::IsTrue(x[battery.indices.states.qnS] > 6.251e2 &&
                       x[battery.indices.states.qnS] < 6.2511e2);
        Assert::IsTrue(x[battery.indices.states.qpB] > 5.77181e3 &&
                       x[battery.indices.states.qpB] < 5.771821e3);
        Assert::IsTrue(x[battery.indices.states.qpS] > 6.41563e2 &&
                       x[battery.indices.states.qpS] < 6.4156335e2);
    }

    void testBatteryOutputEqn() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Initialize
        auto u0 = BatteryModel::input_type({0.4});
        auto z0 = BatteryModel::output_type({20, 4.0});
        auto x = battery.initialize(u0, z0);

        // Set noise vector
        std::vector<double> zeroNoise(2);

        // Set input vector
        auto u = BatteryModel::input_type({1});

        // Compute output
        auto z = battery.outputEqn(0, x, zeroNoise);

        // Check outputs
        Assert::IsTrue(z[battery.indices.outputs.Vm] > 3.999871 &&
                       z[battery.indices.outputs.Vm] < 3.9998711);
        Assert::AreEqual(20, z[battery.indices.outputs.Tbm], 1e-12);
    }

    void testBatteryThresholdEqn() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Set input vector
        auto u = BatteryModel::input_type({1});

        // Initialize
        auto u0 = BatteryModel::input_type({0.4});
        auto z0 = BatteryModel::output_type({20, 4.0});
        auto x = battery.initialize(u0, z0);

        // Check that not at threshold
        bool result = battery.thresholdEqn(0, x)[0];
        Assert::AreEqual(false, result);

        // Re-initialize to lower voltage
        u0[0] = 0.3;
        z0[0] = 20;
        z0[1] = 3.0;
        x = battery.initialize(u0, z0);

        // Check that at threshold
        result = battery.thresholdEqn(0, x)[0];
        Assert::AreEqual(true, result);
    }

    void testBatteryPredictedOutputEqn() {
        // Create battery model
        BatteryModel battery = BatteryModel();

        // Set input vector
        auto u = BatteryModel::input_type({1});

        // Initialize
        auto u0 = BatteryModel::input_type({0.4});
        auto z0 = BatteryModel::output_type({20, 4.2});
        auto x = battery.initialize(u0, z0);

        // Set up predicted outputs
        auto z = battery.getOutputVector();
        auto observables = battery.observablesEqn(0, x);

        // Check values
        Assert::AreEqual(0, observables.size());
    }

    void registerTests(PCOE::Test::TestContext& context) {
        context.AddTest("Tank Initialization", testTankInitialize, "Model Tank");
        context.AddTest("Tank State Eqn", testTankStateEqn, "Model Tank");
        context.AddTest("Tank Output Eqn", testTankOutputEqn, "Model Tank");
        
        context.AddTest("Battery Set Parameters", testBatterySetParameters, "Model Battery");
        context.AddTest("Battery Initialization", testBatteryInitialization, "Model Battery");
        context.AddTest("Battery State Eqn", testBatteryStateEqn, "Model Battery");
        context.AddTest("Battery Output Eqn", testBatteryOutputEqn, "Model Battery");
        context.AddTest("Battery Threshold Eqn", testBatteryThresholdEqn, "Model Battery");
        context.AddTest("Battery Predicted Output Eqn", testBatteryPredictedOutputEqn, "Model Battery");
    }
}

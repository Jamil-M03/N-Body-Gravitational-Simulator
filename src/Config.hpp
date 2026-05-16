#pragma once
#include <string>
#include "System.hpp"

// All the simulation parameters that aren't part of the physical system.
struct SimParams {
    std::string name;
    double dt;           // integration timestep in years
    double total_time;   // total simulation duration in years
    double output_dt;    // write a CSV frame every output_dt years
};

struct Scenario {
    System    system;
    SimParams params;
};

// Load a scenario from a JSON file. Throws std::runtime_error on bad input.
Scenario load_scenario(const std::string& path);

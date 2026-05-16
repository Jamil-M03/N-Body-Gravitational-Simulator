#include "Config.hpp"

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Helper: read an array of exactly 3 doubles into a Vec3, with a clear
// error message naming where it came from.
static Vec3 parse_vec3(const json& arr, const std::string& where) {
    if (!arr.is_array() || arr.size() != 3) {
        throw std::runtime_error(where + ": expected array of 3 numbers");
    }
    return Vec3{arr[0].get<double>(),
                arr[1].get<double>(),
                arr[2].get<double>()};
}

Scenario load_scenario(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Cannot open config file: " + path);
    }

    json j;
    in >> j;  // throws on malformed JSON

    Scenario s;

    // Required: name and the three time parameters
    s.params.name       = j.at("name").get<std::string>();
    s.params.dt         = j.at("dt").get<double>();
    s.params.total_time = j.at("total_time").get<double>();
    s.params.output_dt  = j.at("output_dt").get<double>();

    // Optional: G (defaults to 4*pi^2 in AU/M_sun/yr) and epsilon (0)
    s.system.G       = j.value("G",       4.0 * M_PI * M_PI);
    s.system.epsilon = j.value("epsilon", 0.0);

    // Required: bodies array
    const auto& bodies_json = j.at("bodies");
    if (!bodies_json.is_array() || bodies_json.empty()) {
        throw std::runtime_error("'bodies' must be a non-empty array");
    }
    for (size_t i = 0; i < bodies_json.size(); ++i) {
        const auto& bj = bodies_json[i];
        const std::string where = "bodies[" + std::to_string(i) + "]";

        Body b;
        b.name     = bj.at("name").get<std::string>();
        b.mass     = bj.at("mass").get<double>();
        b.position = parse_vec3(bj.at("position"), where + ".position");
        b.velocity = parse_vec3(bj.at("velocity"), where + ".velocity");
        s.system.bodies.push_back(std::move(b));
    }

    // Sanity check: output_dt should be >= dt
    if (s.params.output_dt < s.params.dt) {
        throw std::runtime_error(
            "output_dt must be >= dt (otherwise we'd write more than we simulate)");
    }

    return s;
}

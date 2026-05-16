CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Isrc -Ithird_party
SRC      := $(wildcard src/*.cpp)
OBJ      := $(SRC:.cpp=.o)
TARGET   := nbody

JSON_HPP := third_party/nlohmann/json.hpp
JSON_URL := https://raw.githubusercontent.com/nlohmann/json/v3.11.3/single_include/nlohmann/json.hpp

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# main.cpp #include's json.hpp, so any source that uses it must depend on it.
# The bare rule below auto-fetches the header if missing.
src/main.o: $(JSON_HPP)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(JSON_HPP):
	@mkdir -p third_party/nlohmann
	@echo "Fetching nlohmann/json.hpp ..."
	@curl -fsSL $(JSON_URL) -o $@ || \
	  (echo "curl failed; trying git clone fallback..." && \
	   git clone --depth=1 --branch=v3.11.3 https://github.com/nlohmann/json.git /tmp/_nl_json && \
	   cp /tmp/_nl_json/single_include/nlohmann/json.hpp $@ && \
	   rm -rf /tmp/_nl_json)
	@echo "Got $@"

clean:
	rm -f src/*.o $(TARGET)

distclean: clean
	rm -f $(JSON_HPP)

.PHONY: clean distclean

add_library(Toadwart.PreCompiledHeaders INTERFACE)

target_precompile_headers(Toadwart.PreCompiledHeaders INTERFACE
  <cstring>
  <cstdio>
  <cstdlib>
  <cstdint>
  <stdfloat>
  <cmath>
  <cassert>
  <ctime>
  <string_view>
  <string>
  <format>
  <vector>
  <array>
  <functional>
  <algorithm>
  <utility>
  <memory>
  <bitset>
  <new>
  <bit>
  <type_traits>
  <optional>
  <variant>
  <tuple>
  <expected>
  <span>
  <ranges>
  <iterator>
  <numeric>
  <execution>
  <random>
  <chrono>
  <thread>
  <mutex>
  <condition_variable>
  <future>
  <filesystem>
  <future>
  <compare>
  <stack>
  <queue>
  <regex>
  <filesystem>
  <atomic>
)

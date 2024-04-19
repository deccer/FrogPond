#pragma once

#define TOADWART_UNUSED(...) (void)(__VA_ARGS__)

#if defined(TOADWART_ENABLE_PROFILER)
  #ifndef TRACY_ENABLE
    #define TRACY_ENABLE
  #endif
  #include <tracy/Tracy.hpp>
  #include <tracy/TracyOpenGL.hpp>

  #define TOADWART_PROFILE_NAMED_SCOPE(name) ZoneScopedN(name)
  #define TOADWART_PROFILE_NAMED_SIZED_SCOPE(name, size) ZoneScopedN(name, size)
  #define TOADWART_PROFILE_SCOPED() ZoneScoped
  #define TOADWART_MARK_FRAME() FrameMark
  #define TOADWART_MARK_GPU_FRAME() TracyGpuCollect
#else
  #define TOADWART_PROFILE_NAMED_SCOPE(name) (void)(name)
  #define TOADWART_PROFILE_NAMED_SIZED_SCOPE(name, size) TOADWART_UNUSED(0)
  #define TOADWART_PROFILE_SCOPED() TOADWART_UNUSED(0)
  #define TOADWART_MARK_FRAME() TOADWART_UNUSED(0)
  #define TOADWART_MARK_GPU_FRAME() TOADWART_UNUSED(0)
#endif
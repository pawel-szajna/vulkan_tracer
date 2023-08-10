Project structure
=================

============ =========== ======================================================
Library      Type        Description
============ =========== ======================================================
**front**    executable  Application front-end: the main executable file
**preview**  static      Live preview during rendering, uses SDL2
**runner**   static      Scheduler and Vulkan interface operations
**scene**    static      Types for describing and reading scene definitions
**shaders**  glsl code   Compute shader implementing the ray tracing algorithm
**types**    interface   Definitions of simple types used throughout the app
**utils**    static      Utility functions and classes
============ =========== ======================================================

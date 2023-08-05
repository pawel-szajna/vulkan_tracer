vulkan tracer
=============

A raytracer running on the GPU, utilizing the Vulkan framework to execute the
raytracing algorithm on compute shaders.

Dependencies
------------

* **C++ compiler (modern)** – obviously
* **CMake** – for the build process, recommended with Ninja
* **glslang** – shader compilation
* **Python 3** – shader code preprocessing
* **spdlog** – application logging
* **Vulkan SDK** – Vulkan API is used to use compute shaders
* **yaml-cpp** – scene configuration

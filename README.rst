vulkan tracer
=============

A raytracer running on the GPU, utilizing the Vulkan framework to execute the
raytracing algorithm on compute shaders.

Dependencies
------------

* Tools:
    * **C++ compiler (modern)** – obviously
    * **CMake** – for the build process, recommended with Ninja
    * **glslang** – shader compilation
    * **Python 3** – shader code preprocessing
    * **Vulkan SDK** – Vulkan API is used to use compute shaders
* C++ Libraries:
    * **argparse** – argument parsing
    * **spdlog** – application logging
    * **yaml-cpp** – configuration

Building
--------

Get all the dependencies, type ``cmake -S. -Bbuild -GNinja && cmake --build build``
and hope for the best!

Usage
-----

Calling ``./vulkan_tracer scene.yml`` should produce a file called ``output.ppm`` with
the rendered scene.

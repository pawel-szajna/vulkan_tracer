vulkan tracer
=============

A raytracer running on the GPU, utilizing the Vulkan framework to execute the
raytracing algorithm on compute shaders.

Currently implemented features
------------------------------

* Basic shapes (sphere, prism, volumetric cloud)
* Basic materials (diffuse surface, mirror/specular surface, light-emitting surface, glass/refracting surface, fog)
* Physically based GPU Monte Carlo path tracer with global illumination and ray refractions based on wavelength
* Live preview of the rendered scene
* CPU filtering framework (with very simple filters implemented)
* A custom format and interface for delivering objects data to the GPU

Dependencies
------------

* Tools:
    * **C++ compiler (modern)** – obviously, checked with GCC and Clang
    * **CMake** – for the build process, recommended with Ninja
    * **glslc/glslang** – shader compilation
    * **Python 3** – shader code preprocessing, interface generators
    * **Vulkan SDK** – Vulkan API is used to use compute shaders (homebrew MoltenVK
      works on Apple platforms)
* C++ Libraries:
    * **argparse** – argument parsing
    * **indicators** – progress display (included single-header)
    * **SDL2** – live preview (if ``LIVE_VIEW`` CMake flag is on)
    * **spdlog** – application logging
    * **yaml-cpp** – configuration

Building
--------

Get all the dependencies, type ``cmake -S. -Bbuild -GNinja && cmake --build build``
and hope for the best!

Usage
-----

Calling ``./vulkan_tracer -i scene.yml`` should produce a file called ``output.ppm`` with
the rendered scene.

Compatibility
-------------

This has only been tested on Apple M1 (and it worked), and on Intel i5-10310U (and it
worked), and on GeForce MX 150 (and it worked), checked with macOS, Linux, and Windows,
so pretty much should work everywhere.

<h1 align="center">
  <br>
  <a href="http://github.com/e2fo2l/Nabla2D"><img src="assets/logo.svg" alt="Markdownify" width="200"></a>
  <br>
  Nabla2D
  <br>
</h1>

<h4 align="center">(WIP) Cross-platform modern C++ game engine with a focus on 2D.</h4>

<p align="center">
<img src="https://img.shields.io/github/license/e2fo2l/Nabla2D">
<a href="https://www.codefactor.io/repository/github/e2fo2l/nabla2d"><img src="https://www.codefactor.io/repository/github/e2fo2l/nabla2d/badge" alt="CodeFactor" /></a>
<a href="https://github.com/e2fo2l/Nabla2D/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/e2fo2l/Nabla2D/debug.yml"></a>
<a href="https://github.com/e2fo2l/Nabla2D/issues"><img alt="GitHub issues" src="https://img.shields.io/github/issues/e2fo2l/Nabla2D"></a>
<a href="https://github.com/e2fo2l/Nabla2D/pulls"><img alt="GitHub pull requests" src="https://img.shields.io/github/issues-pr/e2fo2l/Nabla2D"></a>
</p>

# License
Nabla2D is licensed under the [GNU General Public License v3.0](https://www.gnu.org/licenses/).

# Features
* Cross-platform
* 2D/3D rendering
* In-engine editor
* Sprite import from [Aseprite](https://www.aseprite.org/)
* Tilemap import from [Tiled](https://www.mapeditor.org/)
* Basic 3D mesh support
* GLSL shaders
* GUI with [ImGui](https://github.com/ocornut/imgui)
* Powerful Entity Component System
* Scripting with Lua (w/ optional [LuaJIT](https://luajit.org/) support)

# Download
You can download the latest release from the [releases page](https://github.com/e2fo2l/Nabla2D/releases).

# Screenshots
TODO

# Building
### Requirements
* CMake
* C++ compiler with C++17 support (e.g. GCC 7.0+)
* [Conan](https://conan.io/) package manager
* Linux: OpenGL development libraries (`libopengl-dev` on Debian)

### Build instructions
1. Clone the repository
2. Create a `build` directory and `cd` into it
3. Run `conan install .. --build=missing` to install dependencies
4. Run `cmake ..` to generate the build files
5. Run `cmake --build .` to build the project
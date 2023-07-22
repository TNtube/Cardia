# Cardia Engine

<img src=".github/image/cardia-logo.png" alt="logo" width="128" height="128">

[![License](https://img.shields.io/badge/license-Apache_2.0-blue.svg)](LICENSE)

Cardia is a C++20 game engine that uses python as its scripting language.

The initial aim was primarily educational, to learn game engine programming,
but eventually evolved into an open source project with the intention of
releasing a game with it.

## Features

### Python scripting :

Cardia integrates a scripting system using Python.

Scripts are attached to entities, and reflection allows fields to be simply
modified in real time from within the editor.

For example, the following script can be attached to an entity:
```python
class Moving(Behavior):
    velocity: int
    _tick_count: float
    text: str
    color: Vector3
```

And then, from the editor, the "public" fields can be modified:

![scripting.png](.github/image/scripting.png)

### Editor :

Cardia comes with an editor, CardiaTor, which lets you create and edit scenes,
and modify entities and their components. Its long-term aim is to provide a
whole set of tools to make life easier for engine users, although for the moment
it remains very simple.  It takes inspiration from the Unity editor, and is written in C++ using the
[Dear ImGui](https://github.com/ocornut/imgui) library.

![editor.png](.github/image/cardiator.png)

### Entity-Component-System :

Cardia uses an ECS architecture, which allows for a lot of flexibility and
performance. The ECS is implemented using the [EnTT](https://github.com/skypjack/entt)
library. The ECS is exposed to python, allowing for easy creation of new components.

### Rendering :

Cardia uses Vulkan for rendering, mainly for it being a modern API, and for its
precise control over the rendering pipeline.

## How to build

As a build system, Cardia uses [XMake](https://xmake.io/#/), which is a
cross-platform build system written in Lua. It is very simple to use, and
supports a lot of platforms and compilers. It is also very fast, and can
generate project files for IDEs such as Visual Studio, XCode, CLion, etc.

First, you need to install XMake. You can find instructions on how to do so
[here](https://xmake.io/#/getting_started?id=installation).

Then, you can simply clone the repository and build the project:
```bash
xmake -y
xmake run CardiaTor
```

## How to contribute

If you want to contribute to Cardia, you can do so by opening a pull request.
If you want to add a new feature, please open an issue first so we can discuss it.
If you want to report a bug, please open an issue with a minimal example
showing the bug.

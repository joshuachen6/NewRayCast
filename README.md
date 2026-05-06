# NewRayCast Game Engine

A high-performance, modular game engine written in modern C++ (C++23) utilizing a ray-casting rendering architecture. This engine is designed for efficiency and flexibility, featuring a custom physics implementation and Lua-based entity scripting.

## Core Features

### Rendering Architecture
- **Ray-Casting Engine**: Real-time rendering of 2D map data into a 3D perspective using optimized ray-casting techniques.
- **Custom Model Format**: Support for proprietary 3D model structures optimized for the rendering pipeline.
- **Post-Processing**: Integrated shader support for advanced visual effects.
- **Dynamic UI**: Built-in support for minimaps, text rendering, and geometric primitive drawing.

### Physics and Simulation
- **Vector-Based Physics**: A robust physics engine handling momentum, acceleration, and complex collisions.
- **Geometric Slicing**: Sophisticated collision detection using vertex and edge intersection logic.
- **Custom Linear Algebra**: Specialized utility functions for vector normalization, projection, and dot product operations.

### Scripting and Extensibility
- **Lua Integration**: Deep integration with Lua via LuaBridge3, allowing for dynamic entity behavior and game logic without recompilation.
- **Event-Driven API**: Script-accessible hooks for entity lifecycle events, including `onStart`, `onUpdate`, `onCollide`, and `onDeath`.
- **Modular Entity System**: Flexible tag-based entity management and dynamic model switching.

## Technical Stack

- **Language**: C++23
- **Compiler**: Clang
- **Graphics & Windowing**: SFML 2.6.1
- **Scripting**: Lua 5.4 / LuaBridge3
- **Concurrency**: Intel Threading Building Blocks (TBB)
- **Utilities**: Boost, spdlog, rapidcsv
- **Build System**: xmake

## Build Instructions

### Prerequisites

- [xmake](https://xmake.io/)
- Clang compiler with C++23 support

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/joshuachen6/NewRayCast.git
   cd NewRayCast
   ```

2. Compile the project:
   ```bash
   xmake
   ```

3. Run the engine:
   ```bash
   xmake run GameEngine
   ```

## Development and Contributions

This project uses xmake for dependency management and build configuration. All core engine headers are located in the `include` directory, with implementation details in `src`. Resources, including Lua scripts and textures, are managed within the `resources` folder and automatically deployed to the build directory during compilation.

## License

This project is open-source and intended for development and educational use.

# Bounce Back

An Action RPG depicting a bouncy ball factory employee rebeling against the meniacle CEO. By using simple movement mechanics and a reactive bouncy ball to destroy generator, defeat enemies, and score points. Through clever use of that balls phycis and the environment the play advances through the factory

### Nicholas Shaw Contributions
Physics System - https://github.com/hobbes144/LaurelEye-game-project-BounceBack/tree/main/LaurelEye/include/LaurelEyeEngine/physics
- Physics Agnostic Layer Implemented
- Bullet Integration
- Rigid & Ghost Body Functionality
- Collision Event Generation

Particle System - https://github.com/hobbes144/LaurelEye-game-project-BounceBack/tree/main/LaurelEye/include/LaurelEyeEngine/particles
- Particle Emmision & Simulation
- Particle Rendering

UI System - https://github.com/hobbes144/LaurelEye-game-project-BounceBack/tree/main/LaurelEye/include/LaurelEyeEngine/UI
- UI Rendering
- UI Layout & Anchoring
- UI Interaction

Scripting System  - https://github.com/hobbes144/LaurelEye-game-project-BounceBack/tree/main/LaurelEye/include/LaurelEyeEngine/scripting
- Serialization
- Bindings for Physics Particle UI

 
# Team Laurel Game Project

## Steps to Clone and Run
1. From git bash call 'git clone --recursive'. Do not use GitHub Desktop, and recommended not to use other GUI based implementations due to the use of submodules.
2. From git bash call 'git submodule init vcpkg' from the root of the directory to initialize the vcpkg submodule in your local .git folder.
3. From git bash call 'git submodule update vcpkg' from the root of the directory to fetch updates to vcpkg and the packages we currently utilize.
4. Navigate to the repo on your local desktop. Open the vcpkg folder and run the 'bootstrap-vcpkg' .bat file in order to connect vcpkg to your local submodule.
5. Open the repo in your IDE of choice and begin coding.

## Directory Layout

- Separate the "public" header files of your engine and other libraries from their private files.
   - This allows consuming projects to add just the `include` directory to the C++ `include` path
   - consuming projects then `#include <LaurelGameEngine/core/SomeHeader.h>` making it very explicit what's being included
- Do NOT add private project paths to other projects ... this breaks encapsulation
- Modify the default build settings for you projects so that they use the `/build/...` directory. Three reasons for this
   - enables the `.gitignore` to have one line that ignores all build artifacts
   - organizes the build output making diagnosing problems easier
   - allows you to perform a very fast "clean" by deleting the `/build` directory
- Replace all instances of `Laurel` (as in `LaurelGameEngine`) with your team codename (e.g., `BirchGameEngine`). This applies to folder names, project files, etc.
- prefer `lowercase` directly names except where inappropriate.
- Regarding coding conventions: I've included
```

gam55X-laurel/
├── CMakeLists.txt                    # Global CMake project linking to all the sub-projects
├── coding_conventions.md             # An example coding convention file for the team
├── external/                         # Third-party libraries (headers + binaries)
│   ├── glfw/                         # Window/input management
│   ├── glad/                         # OpenGL function loader
│   ├── glm/                          # Math library
│   ├── fmod/                         # Audio middleware
│   ├── assimp/                       # Model importer
│   ├── imgui/                        # Immediate-mode GUI
│   └── ...etc...
├── include/                          # Public headers for your lib/dll projects
│   └── LaurelEyeEngine/              # Your game engine's public headers
│       ├── core/                     # Application, window, input
│       ├── graphics/                 # Shader, texture, mesh, camera
│       ├── audio/                    # FMOD wrappers
│       ├── scene/                    # Entity, transform, hierarchy
│       ├── utils/                    # Logging, timing, file I/O
│       └── ...etc...
├── source/                           # C++ implementation projects
│   ├── LaurelGameEngine/              # Game engine (.lib or .exe if script-based)
│   │   ├── CMakeLists.txt   # Project file
│   │   ├── core/                     # Internal hierarchy
│   │   └── ...etc...
│   ├── LaurelGame/                    # Game executable project (optional)
│   │   ├── CMakeLists.txt
│   │   └── ...etc...
│   └── ...etc...                     # Other projects (examples, prototypes, etc.)
├── tests/                            # Test code
│   ├── LaurelGameEngineUnitTests/
│   │   ├── CMakeLists.txt
│   │   └── ...etc...
│   └── LaurelGameEngineStressTests/
│       ├── CMakeLists.txt
│       └── ...etc...
├── assets/                           # Assets folder
│   ├── shaders/
│   ├── textures/
│   ├── models/
│   ├── audio/
│   └── ...etc...
├── build/                            # Build artifacts (.gitignored)
│   ├── intermediate/                 # Intermediate files
│   └── output/                       # Output binaries
└── ...etc...

```

## Common Mistakes to Avoid
- Including private headers from other projects
- Mixing public and private headers in the same folder
- Committing build artifacts or `.vs/` folders
- Using inconsistent naming conventions across folders
- Confusing organizations




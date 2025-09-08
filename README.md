# GAM55X Game Project

## Directory Layout

You get to determine the folder layout in GAM550, but the instructor will mark down if unorganized, sloppy, or just looks like copy-paste from other projects.  Your approach must be intentional.

- Below is a suggested layout (naturally replace `Aspen` below with your team codename)
- Separate the "public" header files of your engine and other libraries from their private files.
   - This allows consuming projects to add just the `include` directory to the C++ `include` path
   - consuming projects then `#include <AspenGameEngine/core/SomeHeader.h>` making it very explicit what's being included
- Do NOT add private project paths to other projects ... this breaks encapsulation
- Modify the default build settings for you projects so that they use the `/build/...` directory. Three reasons for this
   - enables the `.gitignore` to have one line that ignores all build artifacts
   - organizes the build output making diagnosing problems easier
   - allows you to perform a very fast "clean" by deleting the `/build` directory
- Replace all instances of `Aspen` (as in `AspenGameEngine`) with your team codename (e.g., `BirchGameEngine`). This applies to folder names, project files, etc.
- prefer `lowercase` directy names except where inappropriate.  
- Regarding coding conventions: I've included
```

GAM55X-Aspen/
├── GAM550.sln                        # Global solution file containing all the projects
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
│   └── AspenGameEngine/              # Your game engine's public headers
│       ├── core/                     # Application, window, input
│       ├── graphics/                 # Shader, texture, mesh, camera
│       ├── audio/                    # FMOD wrappers
│       ├── scene/                    # Entity, transform, hierarchy
│       ├── utils/                    # Logging, timing, file I/O
│       └── ...etc...
├── source/                           # C++ implementation projects
│   ├── AspenGameEngine/              # Game engine (.lib or .exe if script-based)
│   │   ├── AspenGameEngine.vcxproj   # Project file
│   │   ├── core/                     # Internal hierarchy
│   │   └── ...etc...                 
│   ├── AspenGame/                    # Game executable project (optional)
│   │   ├── AspenGame.vcxproj         
│   │   └── ...etc...                 
│   └── ...etc...                     # Other projects (examples, prototypes, etc.)
├── tests/                            # Test code
│   ├── AspenGameEngineUnitTests/
│   │   ├── AspenGameEngineUnitTests.vcxproj
│   │   └── ...etc...
│   └── AspenGameEngineStressTests/
│       ├── AspenGameEngineStressTests.vcxproj
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

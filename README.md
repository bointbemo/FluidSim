# Fluid Simulation in Compute Shaders

Smooth Particle Hydrodynamics (SPH)is an interpolation method which approximates and derives of continuous field quantities by using discrete sample points. The points are the said smooth particles that carry mass, position and velocity and also pressure, temperature and density. SPH can approximate derivates of continuous fields using analytical differentiation on particles located arbitrarily. i.e. does not have to be confined to a particular location in space. Using my physics background I implemented a SPH physics system into the Newcastle University game engine, solving all fluid particle calculations in compute shaders and providing the ability for fluid bodies to act on rigid bodies. the simulation currently supports up to 7200 particles being rendered with forces being updated on the GPU achieving 240 frame per second in most scenarios.

#How to run
The solution is in the Build folder, If there isnt one, create a empty folder titled build.

Set the CSC8503 project as the startup project, and set the configuration type to x64. 

Make sure that the CSC8503 project has CSC8503CoreClasses, NCLCoreClasses, and OpenGLRendering set as dependencies. 

In CMake - go to File->Delete Cache before trying to set up the file paths; it seems that CMake likes to remember previous paths and will replace them as you set it up!
To generate your own working .sln you need CMake. Fortunately, there's a 'portable' version of CMake that doesn't require any installation, found here: https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0-windows-x86_64.zip

Launch cMake GUI, add file path of source code into "where is the source code" e.g ..//..//FluidModel and add build path of build folder to where to build binaries e.g ..//..//FluidModel/Build.

Press configure, and finally generate.

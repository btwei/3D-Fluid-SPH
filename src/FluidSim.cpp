#include "FluidSim.h"

void FluidSim::run() {
    init();
    mainLoop();
    cleanup();
}

void FluidSim::init() {
    window.init(WIDTH, HEIGHT, "3D SPH Fluid Sim");
    solver.init();
    renderer.init(window.getGLFWWindow(), &solver);
}

void FluidSim::mainLoop() {
    while(!window.shouldClose()){

        solver.mainLoop();

        renderer.mainLoop();

        window.pollEvents();
    }
}

void FluidSim::cleanup() {
    renderer.cleanup();

    window.cleanup();
}
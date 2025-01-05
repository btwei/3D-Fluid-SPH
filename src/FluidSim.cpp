#include "FluidSim.h"

void FluidSim::run() {
    init();
    mainLoop();
    cleanup();
}

void FluidSim::init() {
    window.init(WIDTH, HEIGHT, "2D Fluid Sim");
    renderer.init(window.getGLFWWindow());
}

void FluidSim::mainLoop() {
    while(!window.shouldClose()){

        renderer.mainLoop();

        window.pollEvents();
    }
}

void FluidSim::cleanup() {
    renderer.cleanup();

    window.cleanup();
}
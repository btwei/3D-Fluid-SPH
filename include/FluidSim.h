#ifndef FLUIDSIM_H
#define FLUIDSIM_H

#include "Solver.h"
#include "Renderer.h"
#include "Window.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

class FluidSim {
public:
    void run();
private:
    SPH solver;
    Window window;
    Renderer renderer;

    void init();
    void mainLoop();
    void cleanup();
};

#endif
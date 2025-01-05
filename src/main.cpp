#include <iostream>

#include "FluidSim.h"

int main(){
    FluidSim app;

    try {
		app.run();
	} catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
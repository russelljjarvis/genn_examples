// Standard C++ includes
#include <random>
#include <vector>

// GeNN robotics includes
#include "connectors.h"
#include "spike_csv_recorder.h"
#include "timer.h"

// Model parameters
#include "parameters.h"

// Auto-generated model code
#include "potjans_microcircuit_CODE/definitions.h"

#define ADD_SPIKE_RECORDER(LAYER, POPULATION) spikeRecorders.emplace_back(#LAYER#POPULATION".csv",  glbSpkCnt##LAYER##POPULATION, glbSpk##LAYER##POPULATION)

#define BUILD_PROJECTION(SRC_LAYER, SRC_POP, TRG_LAYER, TRG_POP)                                                                                            \
    buildFixedNumberTotalWithReplacementConnector(Parameters::getNumNeurons(Parameters::Layer##SRC_LAYER, Parameters::Population##SRC_POP),                 \
                                                  Parameters::getNumNeurons(Parameters::Layer##TRG_LAYER, Parameters::Population##TRG_POP),                 \
                                                  Parameters::getNumConnections(Parameters::Layer##SRC_LAYER, Parameters::Population##SRC_POP,              \
                                                                                Parameters::Layer##TRG_LAYER, Parameters::Population##TRG_POP),             \
                                                  C##SRC_LAYER##SRC_POP##_##TRG_LAYER##TRG_POP, &allocate##SRC_LAYER##SRC_POP##_##TRG_LAYER##TRG_POP, rng)
int main()
{
    {
        Timer<> timer("Allocation:");
        allocateMem();
    }
    {
        Timer<> timer("Initialization:");
        initialize();
    }

    {
        Timer<> timer("Building connectivity:");

        std::mt19937 rng;
        BUILD_PROJECTION(23, E, 23, E);
        BUILD_PROJECTION(23, E, 23, I);
        BUILD_PROJECTION(23, E, 4, E);
        BUILD_PROJECTION(23, E, 4, I);
        BUILD_PROJECTION(23, E, 5, E);
        BUILD_PROJECTION(23, E, 5, I);
        BUILD_PROJECTION(23, E, 6, E);
        BUILD_PROJECTION(23, E, 6, I);
        BUILD_PROJECTION(23, I, 23, E);
        BUILD_PROJECTION(23, I, 23, I);
        BUILD_PROJECTION(23, I, 4, E);
        BUILD_PROJECTION(23, I, 4, I);
        BUILD_PROJECTION(23, I, 5, E);
        BUILD_PROJECTION(23, I, 5, I);
        BUILD_PROJECTION(23, I, 6, E);
        BUILD_PROJECTION(23, I, 6, I);
        BUILD_PROJECTION(4, E, 23, E);
        BUILD_PROJECTION(4, E, 23, I);
        BUILD_PROJECTION(4, E, 4, E);
        BUILD_PROJECTION(4, E, 4, I);
        BUILD_PROJECTION(4, E, 5, E);
        BUILD_PROJECTION(4, E, 5, I);
        BUILD_PROJECTION(4, E, 6, E);
        BUILD_PROJECTION(4, E, 6, I);
        BUILD_PROJECTION(4, I, 23, E);
        BUILD_PROJECTION(4, I, 23, I);
        BUILD_PROJECTION(4, I, 4, E);
        BUILD_PROJECTION(4, I, 4, I);
        BUILD_PROJECTION(4, I, 5, E);
        BUILD_PROJECTION(4, I, 5, I);
        BUILD_PROJECTION(4, I, 6, E);
        BUILD_PROJECTION(4, I, 6, I);
        BUILD_PROJECTION(5, E, 23, E);
        BUILD_PROJECTION(5, E, 23, I);
        BUILD_PROJECTION(5, E, 4, E);
        BUILD_PROJECTION(5, E, 4, I);
        BUILD_PROJECTION(5, E, 5, E);
        BUILD_PROJECTION(5, E, 5, I);
        BUILD_PROJECTION(5, E, 6, E);
        BUILD_PROJECTION(5, E, 6, I);
        //BUILD_PROJECTION(5, I, 23, E);
        //BUILD_PROJECTION(5, I, 23, I);
        BUILD_PROJECTION(5, I, 4, E);
        //BUILD_PROJECTION(5, I, 4, I);
        BUILD_PROJECTION(5, I, 5, E);
        BUILD_PROJECTION(5, I, 5, I);
        BUILD_PROJECTION(5, I, 6, E);
        BUILD_PROJECTION(5, I, 6, I);
        BUILD_PROJECTION(6, E, 23, E);
        BUILD_PROJECTION(6, E, 23, I);
        BUILD_PROJECTION(6, E, 4, E);
        BUILD_PROJECTION(6, E, 4, I);
        BUILD_PROJECTION(6, E, 5, E);
        BUILD_PROJECTION(6, E, 5, I);
        BUILD_PROJECTION(6, E, 6, E);
        BUILD_PROJECTION(6, E, 6, I);
        //BUILD_PROJECTION(6, I, 23, E);
        //BUILD_PROJECTION(6, I, 23, I);
        //BUILD_PROJECTION(6, I, 4, E);
        //BUILD_PROJECTION(6, I, 4, I);
        //BUILD_PROJECTION(6, I, 5, E);
        //BUILD_PROJECTION(6, I, 5, I);
        BUILD_PROJECTION(6, I, 6, E);
        BUILD_PROJECTION(6, I, 6, I);
    }

    // Final setup
    {
        Timer<> timer("Sparse init:");
        initpotjans_microcircuit();
    }

    // Open CSV output files
    //SpikeCSVRecorder spikes("spikes.csv", glbSpkCntE, glbSpkE);
    std::vector<SpikeCSVRecorder> spikeRecorders;
    spikeRecorders.reserve(Parameters::LayerMax * Parameters::PopulationMax);
    ADD_SPIKE_RECORDER(23, E);
    ADD_SPIKE_RECORDER(23, I);
    ADD_SPIKE_RECORDER(4, E);
    ADD_SPIKE_RECORDER(4, I);
    ADD_SPIKE_RECORDER(5, E);
    ADD_SPIKE_RECORDER(5, I);
    ADD_SPIKE_RECORDER(6, E);
    ADD_SPIKE_RECORDER(6, I);


    {
        Timer<> timer("Simulation:");
        // Loop through timesteps
        const unsigned int timesteps = round(Parameters::durationMs / DT);
        const unsigned int tenPercentTimestep = timesteps / 10;
        for(unsigned int i = 0; i < timesteps; i++)
        {
            // Indicate every 10%
            if((i % tenPercentTimestep) == 0) {
                std::cout << i / 100 << "%" << std::endl;
            }

            // Simulate
#ifndef CPU_ONLY
            stepTimeGPU();

            pull23ECurrentSpikesFromDevice();
            pull23ICurrentSpikesFromDevice();
            pull4ECurrentSpikesFromDevice();
            pull4ICurrentSpikesFromDevice();
            pull5ECurrentSpikesFromDevice();
            pull5ICurrentSpikesFromDevice();
            pull6ECurrentSpikesFromDevice();
            pull6ICurrentSpikesFromDevice();
#else
            stepTimeCPU();
#endif

            // Record spikes
            for(auto &s : spikeRecorders) {
                s.record(t);
            }
        }
    }

    return 0;
}

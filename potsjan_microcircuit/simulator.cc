// Standard C++ includes
#include <iostream>
#include <memory>
#include <random>
#include <vector>

// GeNN user projects includes
#include "timer.h"
#include "spikeRecorder.h"

// Model parameters
#include "parameters.h"

// Auto-generated model code
#include "potjans_microcircuit_CODE/definitions.h"

namespace
{
void buildRowLengths(unsigned int numPre, unsigned int numPost, size_t numConnections, unsigned int *rowLengths, std::mt19937 &rng)
{
    // Calculate row lengths
    // **NOTE** we are FINISHING at second from last row because all remaining connections must go in last row
    size_t remainingConnections = numConnections;
    size_t matrixSize = (size_t)numPre * (size_t)numPost;
    std::generate_n(&rowLengths[0], numPre - 1,
                    [&remainingConnections, &matrixSize, numPost, &rng]()
                    {
                        const double probability = (double)numPost / (double)matrixSize;

                        // Create distribution to sample row length
                        std::binomial_distribution<size_t> rowLengthDist(remainingConnections, probability);

                        // Sample row length;
                        const size_t rowLength = rowLengthDist(rng);

                        // Update counters
                        remainingConnections -= rowLength;
                        matrixSize -= numPost;

                        return (unsigned int)rowLength;
                    });

    // Insert remaining connections into last row
    rowLengths[numPre - 1] = (unsigned int)remainingConnections;
}
}
// Macro to build a connection between a pair of populations
#define BUILD_PROJECTION(SRC_LAYER, SRC_POP, TRG_LAYER, TRG_POP)                                                                                                                \
    allocatepreCalcRowLength##SRC_LAYER##SRC_POP##_##TRG_LAYER##TRG_POP(Parameters::getScaledNumNeurons(Parameters::Layer##SRC_LAYER, Parameters::Population##SRC_POP));        \
    buildRowLengths(Parameters::getScaledNumNeurons(Parameters::Layer##SRC_LAYER, Parameters::Population##SRC_POP),                                                             \
                    Parameters::getScaledNumNeurons(Parameters::Layer##TRG_LAYER, Parameters::Population##TRG_POP),                                                             \
                    Parameters::getScaledNumConnections(Parameters::Layer##SRC_LAYER, Parameters::Population##SRC_POP,                                                          \
                                                        Parameters::Layer##TRG_LAYER, Parameters::Population##TRG_POP),                                                         \
                    preCalcRowLength##SRC_LAYER##SRC_POP##_##TRG_LAYER##TRG_POP, rng);                                                                                          \
    pushpreCalcRowLength##SRC_LAYER##SRC_POP##_##TRG_LAYER##TRG_POP##ToDevice(Parameters::getScaledNumNeurons(Parameters::Layer##SRC_LAYER, Parameters::Population##SRC_POP))   \

// Macro to record a population's output
#define ADD_SPIKE_RECORDER(LAYER, POPULATION)                                                                                                               \
    spikeRecorders.emplace_back(new SpikeRecorderCached(#LAYER#POPULATION".csv", glbSpkCnt##LAYER##POPULATION, glbSpk##LAYER##POPULATION, ",", true))

int main()
{
    allocateMem();

    {
        Timer timer("Building row lengths:");

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

    initialize();
    initializeSparse();

    // Create spike recorders
    // **HACK** would be nicer to have arrays of objects rather than pointers but ofstreams
    // aren't correctly moved in GCC 4.9.4 (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54316) -
    // the newest version that can be used with CUDA on Sussex HPC
    std::vector<std::unique_ptr<SpikeRecorderCached>> spikeRecorders;
    spikeRecorders.reserve(Parameters::LayerMax * Parameters::PopulationMax);
    ADD_SPIKE_RECORDER(23, E);
    ADD_SPIKE_RECORDER(23, I);
    ADD_SPIKE_RECORDER(4, E);
    ADD_SPIKE_RECORDER(4, I);
    ADD_SPIKE_RECORDER(5, E);
    ADD_SPIKE_RECORDER(5, I);
    ADD_SPIKE_RECORDER(6, E);
    ADD_SPIKE_RECORDER(6, I);

    double recordMs = 0.0;

    {
        Timer timer("Simulation:");
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
            stepTime();

            pull23ECurrentSpikesFromDevice();
            pull23ICurrentSpikesFromDevice();
            pull4ECurrentSpikesFromDevice();
            pull4ICurrentSpikesFromDevice();
            pull5ECurrentSpikesFromDevice();
            pull5ICurrentSpikesFromDevice();
            pull6ECurrentSpikesFromDevice();
            pull6ICurrentSpikesFromDevice();

            {
                TimerAccumulate timer(recordMs);

                // Record spikes
                for(auto &s : spikeRecorders) {
                    s->record(t);
                }
            }
        }
    }

    // Write spike recorder cache to disk
    {
        Timer timer("Writing spikes to disk:");
        for(auto &s : spikeRecorders) {
            s->writeCache();
        }
    }

    if(Parameters::measureTiming) {
        std::cout << "Timing:" << std::endl;
        std::cout << "\tInit:" << initTime * 1000.0 << std::endl;
        std::cout << "\tSparse init:" << initSparseTime * 1000.0 << std::endl;
        std::cout << "\tNeuron simulation:" << neuronUpdateTime * 1000.0 << std::endl;
        std::cout << "\tSynapse simulation:" << presynapticUpdateTime * 1000.0 << std::endl;
    }
    std::cout << "Record:" << recordMs << "ms" << std::endl;

    return 0;
}

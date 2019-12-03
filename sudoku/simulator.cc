// Standard C++ includes
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

// OpenCV includes
#include <opencv2/opencv.hpp>

// GeNN userproject includes
#include "timer.h"
#include "sharedLibraryModel.h"
#include "spikeRecorder.h"

// Model parameters
#include "parameters.h"
#include "puzzles.h"

//----------------------------------------------------------------------------
// LiveVisualiser
//----------------------------------------------------------------------------
template<size_t S>
class LiveVisualiser
{
public:
    LiveVisualiser(SharedLibraryModel<float> &model, const Puzzle<S> &puzzle, int squareSize)
    :   m_Model(model), m_Puzzle(puzzle), m_OutputImage((S * squareSize) + 10, S * squareSize, CV_8UC3), 
        m_SquareSize(squareSize), m_SubSize((size_t)std::sqrt(S))
    {
        // Loop through populations
        for(size_t x = 0; x < S; x++) {
            for(size_t y = 0; y < S; y++) {
                auto &popSpikes = m_PopulationSpikes[x][y];

                // Zero spike counts
                std::get<0>(popSpikes) = 0;
                
                // Get pointer to spike count array
                std::get<1>(popSpikes) = m_Model.getArray<unsigned int>("SpikeCount" + Parameters::getPopName(x, y));
            }
        }
    }

    //------------------------------------------------------------------------
    // Public API
    //------------------------------------------------------------------------
    void applySpikes()
    {
        // Loop through populations
        for(size_t x = 0; x < S; x++) {
            for(size_t y = 0; y < S; y++) {
                auto &popSpikes = m_PopulationSpikes[x][y];

                // Loop through domains
                std::get<0>(popSpikes) = 0;
                unsigned int maxSpikes = 0;
                for(size_t d = 0; d < 9; d++) {
                    // Get pointers to start and stop of this domains spike counts
                    unsigned int *domainStart = &std::get<1>(popSpikes)[d * Parameters::coreSize];
                    unsigned int *domainEnd = domainStart + Parameters::coreSize;

                    // Sum all spikes in domain
                    const unsigned int numSpikes = std::accumulate(domainStart, domainEnd, 0u);

                    // If this is an improvement on current best, update best domain
                    if(numSpikes > maxSpikes) {
                        std::get<0>(popSpikes) = d + 1;
                        maxSpikes = numSpikes;
                    }
                }

                // Zero spike counts
                std::fill_n(std::get<1>(popSpikes), 9 * Parameters::coreSize, 0);
            }
        }
    }

    void render(const char *windowName)
    {
        unsigned long long simTimestep = m_Model.getTimestep();
        auto realTime = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> realMs = realTime - m_LastRealTime;
        const double simMs = (double)(simTimestep - m_LastSimTimestep);

        m_LastRealTime = realTime;
        m_LastSimTimestep = simTimestep;

        // Clear background
        m_OutputImage.setTo(CV_RGB(0, 0, 0));
        
        // Render status text
        char status[255];
        sprintf(status, "Time:%.0lf, Speed:%.2fx realtime", m_Model.getTime(), simMs / realMs.count());
        
        const auto statusSize = cv::getTextSize(status, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, 1, nullptr);
        cv::putText(m_OutputImage, status, cv::Point(0, m_OutputImage.rows - statusSize.height),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(255, 255, 255));

        // Loop through cells
        for(size_t x = 0; x < S; x++) {
            for(size_t y = 0; y < S; y++) {
                auto &popSpikes = m_PopulationSpikes[x][y];

                // Determine size of text string and thus position to centre it in square
                const std::string bestNumberString = std::to_string(std::get<0>(popSpikes));
                const auto numberSize = cv::getTextSize(bestNumberString, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, 1, nullptr);
                const int xCentre = (m_SquareSize - numberSize.width) / 2;
                const int yCentre = (m_SquareSize - numberSize.height) / 2;

                // If there is a clue at this location, show number in while
                cv::Scalar colour;
                if(m_Puzzle.puzzle[y][x] != 0) {
                    assert(std::get<0>(popSpikes) == m_Puzzle.puzzle[y][x]);
                    colour = CV_RGB(255, 255, 255);
                }
                // Otherwise, if number matches solution, show number in green
                else if(m_Puzzle.solution[y][x] == std::get<0>(popSpikes)) {
                    colour = CV_RGB(0, 255, 0);
                }
                // Otherwise, show it in red
                else {
                    colour = CV_RGB(255, 0, 0);
                }
                
                // Render text
                cv::putText(m_OutputImage, bestNumberString,
                            cv::Point((x * m_SquareSize) + xCentre, (y * m_SquareSize) + yCentre),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, colour);
            }
        }

        // Draw horizontal and vertical lines
        for(size_t i = 0; i < m_SubSize; i++) {
            const int pos = i * m_SubSize * m_SquareSize;
            cv::line(m_OutputImage, cv::Point(pos, 0), cv::Point(pos, S * m_SquareSize), CV_RGB(255, 255, 255));
            cv::line(m_OutputImage, cv::Point(0, pos), cv::Point(S * m_SquareSize, pos), CV_RGB(255, 255, 255));
        }

        // Show image
        cv::imshow(windowName, m_OutputImage);
    }

private:
    //------------------------------------------------------------------------
    // Typedefines
    //------------------------------------------------------------------------
    typedef std::tuple<unsigned int, unsigned int*> PopulationSpikes;

    //------------------------------------------------------------------------
    // Members
    //------------------------------------------------------------------------
    SharedLibraryModel<float> &m_Model;
    const Puzzle<S> &m_Puzzle;

    const int m_SquareSize;

    const size_t m_SubSize;
    
    cv::Mat m_OutputImage;
    
    // Times used for tracking real vs simulated time
    std::chrono::time_point<std::chrono::high_resolution_clock> m_LastRealTime;
    unsigned long long m_LastSimTimestep;

    // Accumulated spike counts and functions to get current for each population
    PopulationSpikes m_PopulationSpikes[S][S];

};

template<size_t S>
void displayThreadHandler(LiveVisualiser<S> &visualiser, std::mutex &mutex, std::atomic<bool> &run)
{
    cv::namedWindow("Sudoku", cv::WINDOW_NORMAL);
    cv::resizeWindow("Sudoku", 50 * S, (50 * S) + 10);

    while(true) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            visualiser.render("Sudoku");
        }

        const auto key = cv::waitKey(33);
        if(key == 27) {
            break;
        }
    }
    
    // Clear run flag
    run = false;
}

int main()
{
    const auto &puzzle = Puzzles::easy;

    SharedLibraryModel<float> model("./", "sudoku");

    model.allocateMem();
    model.initialize();
    model.initializeSparse();

    std::atomic<bool> run{true};
    std::mutex mutex;
    LiveVisualiser<9> visualiser(model, puzzle, 50);
    std::thread displayThread(displayThreadHandler<9>, std::ref(visualiser), std::ref(mutex), std::ref(run));

    
    {
        Timer timer("Simulation:");
        
        // Loop through timesteps
        while(run)
        {
            // Simulate
            model.stepTime();

            // If 200 time steps have passed
            if((model.getTimestep() % 200) == 0) {
                // Loop through populations and get each neuron's spike count
                for(size_t y = 0; y < 9; y++) {
                    for(size_t x = 0; x < 9; x++) {
                        model.pullVarFromDevice(Parameters::getPopName(x, y), "SpikeCount");
                    }
                }

                // Apply spikes to visualizer
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    visualiser.applySpikes();
                }

                // Loop through populations and re-upload zeroed spike counts
                for(size_t y = 0; y < 9; y++) {
                    for(size_t x = 0; x < 9; x++) {
                        model.pushVarToDevice(Parameters::getPopName(x, y), "SpikeCount");
                    }
                }
            }

        }
    }

    // Join display thread - will wait for it to die
    displayThread.join();

    return 0;
}

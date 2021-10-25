#pragma once
#include <string>

/*
 * Namespace holding command line options. Default values are set here.
 */
namespace opt {
// Read options from command line
void ReadCmdLine(int argc, char** argv);

// General options
inline uint64_t seed = 1;             // Random seed
inline std::string instancePath; // Instance file
inline double timeLimit = 3600;       // Time limit, seconds

}; // namespace opt
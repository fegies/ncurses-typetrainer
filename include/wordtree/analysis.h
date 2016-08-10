#pragma once

#include <string>

#include "wordtree/Wordtree.hpp"

namespace wordtree
{

//Calls the Analyses with default values
//a kind of wrapper function
void doAllAnalyses();

//Fills A wordtree with only the letters that were different
Wordtree* characterAnalysis();

//exports a Gnuplot compatible 3d data file
void characterAnalysisToGnuplotFile(std::string& filename);

}
/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and
 * further review from Lawrence Livermore National Laboratory.
 */

/*!
 *******************************************************************************
 * \file lumberjack_speedTest.cpp
 * \author Chris White (white238@llnl.gov)
 *******************************************************************************
 */

#include "lumberjack/Lumberjack.hpp"
#include "lumberjack/BinaryTreeCommunicator.hpp"
#include "lumberjack/RootCommunicator.hpp"
#include "lumberjack/Message.hpp"

#include "common/CommonTypes.hpp"
#include "common/StringUtilities.hpp"

#include <mpi.h>

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    //Process command line options
    bool commandLineError = false;
    if (argc != 4) {
        std::cout << "Error: Wrong amount of command line arguments given. Usage:" << std::endl << 
                     "   " << argv[0] << " <b|r depending on binary or root communicator> <num messages before push once> <file to be read>" << std::endl;
        return 1;
    }
    std::string communicatorName = "";
    int cycleLimit = asctoolkit::utilities::string::stringToInt(argv[2]);
    char* fileName = argv[3];

    if (std::string(argv[1]) == "b") {
        communicatorName = "binary";
    } else if (std::string(argv[1]) == "r") {
        communicatorName = "root";
    } else {
        std::cout << "Error: First parameter must be either 'b' or 'r' for " << 
                     "BinaryTreeCommunicator or RootCommunicator respectively." << 
                     std::endl;
        commandLineError = true;
    }

    if (commandLineError) {
        return 1;
    }

    // Initialize MPI and get rank and comm size
    MPI_Init(&argc, &argv);

    int commRank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
    int commSize = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    // Determine how many ranks we want to individually track per message
    int ranksLimit = commSize/2;

    // Initialize which lumberjack communicator we want
    asctoolkit::lumberjack::Communicator* communicator = ATK_NULLPTR;
    if (communicatorName == "binary") {
        communicator = new asctoolkit::lumberjack::BinaryTreeCommunicator;
    } else if (communicatorName == "root") {
        communicator = new asctoolkit::lumberjack::RootCommunicator;
    }
    communicator->initialize(MPI_COMM_WORLD, ranksLimit);

    // Initialize lumberjack
    asctoolkit::lumberjack::Lumberjack lj;
    lj.initialize(communicator, ranksLimit);

    // Read lines from file
    std::string currMessage;
    std::vector<std::string> lines;
    std::ifstream file(fileName);
    while(std::getline(file, currMessage)){
        currMessage += '\n';
        lines.push_back(currMessage);
    }
    file.close();

    // Start clock
    std::clock_t begin = clock();

    // Queue messages into lumberjack
    int cycleCount = 0;
    int linesSize = (int)lines.size();
    for (int i = 0; i < linesSize; ++i){
        lj.queueMessage(lines[i]);
        ++cycleCount;
        if (cycleCount > cycleLimit) {
            lj.pushMessagesOnce();
            cycleCount = 0;
        }
    }

    // Push messages fully through lumberjack's communicator
    lj.pushMessagesFully();

    // End clock
    std::clock_t end = clock();

    // Get messages back out of lumberjack since they have been pushed.
    if (lj.isOutputNode()) {
        std::vector<asctoolkit::lumberjack::Message*> messages = lj.getMessages();

       std::ofstream outFile;
       outFile.open("speedTestOutput");
       for(int i=0; i<(int)(messages.size()); ++i){
           outFile << messages[i]->text();
       }
       lj.clearMessages();
       outFile.close();
   }

    // Finalize lumberjack 
    lj.finalize();
    // Finalize the lumberjack communicator
    communicator->finalize();
    delete communicator;

    // Output elapsed time
    if (commRank == 0) {
        std::cout << "Elapsed time: " << ((double)(end - begin)*1000)/CLOCKS_PER_SEC << std::endl;
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
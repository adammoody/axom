/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright (c) 2017, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * LLNL-CODE-xxxxxxx
 *
 * All rights reserved.
 *
 * This file is part of Axom.
 *
 * For details about use and distribution, please read axom/LICENSE.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include "lumberjack/Lumberjack.hpp"
#include "lumberjack/BinaryTreeCommunicator.hpp"
#include "lumberjack/Message.hpp"

#include <mpi.h>
#include <iostream>

//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // Initialize MPI and get rank and comm size
    MPI_Init(&argc, &argv);

    int commRank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
    int commSize = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    // Determine how many ranks we want to individually track per message
    int ranksLimit = commSize/2;

    // Initialize which lumberjack communicator we want
    axom::lumberjack::BinaryTreeCommunicator communicator;
    communicator.initialize(MPI_COMM_WORLD, ranksLimit);

    // Initialize lumberjack
    axom::lumberjack::Lumberjack lj;
    lj.initialize(&communicator, ranksLimit);

    // Queue messages into lumberjack
    if (commRank == 0){
        lj.queueMessage("This message will not be combined");
    }
    else {
        lj.queueMessage("This message will be combined");
        lj.queueMessage("This message will be combined");
        lj.queueMessage("This message will be combined");
    }
    // Push messages fully through lumberjack's communicator
    lj.pushMessagesFully();

    // Determine if this is an output node
    if (lj.isOutputNode()){
        // Get Messages from Lumberjack
        std::vector<axom::lumberjack::Message*> messages = lj.getMessages();
        for(int i=0; i<(int)(messages.size()); ++i){
            // Output a single Message at a time to screen
            std::cout << "(" << messages[i]->stringOfRanks() << ") " << messages[i]->ranksCount() <<
                         " '" << messages[i]->text() << "'" << std::endl;
        }
        // Clear already outputted Messages from Lumberjack
        lj.clearMessages();
    }

    // Finalize lumberjack
    lj.finalize();
    // Finalize the lumberjack communicator
    communicator.finalize();
    // Finalize MPI
    MPI_Finalize();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <fstream>
#include <math.h>

#include "models/IonModel.h"
#include "models/MultiPhysController.h"
#include "common/Utilities.h"

using namespace std;

//***************************************************************************
// Test lattice-Boltzmann Ion Model coupled with Poisson equation
//***************************************************************************

int main(int argc, char **argv)
{
    // Initialize MPI and error handlers
    Utilities::startup( argc, argv );

    { // Limit scope so variables that contain communicators will free before MPI_Finialize

        MPI_Comm comm;
        MPI_Comm_dup(MPI_COMM_WORLD,&comm);
        int rank = comm_rank(comm);
        int nprocs = comm_size(comm);

        if (rank == 0){
            printf("**************************************\n");
            printf("Running Test for Ion Transport \n");
            printf("**************************************\n");
        }
        // Initialize compute device
        ScaLBL_SetDevice(rank);
        ScaLBL_DeviceBarrier();
        MPI_Barrier(comm);

        PROFILE_ENABLE(1);
        //PROFILE_ENABLE_TRACE();
        //PROFILE_ENABLE_MEMORY();
        PROFILE_SYNCHRONIZE();
        PROFILE_START("Main");
        Utilities::setErrorHandlers();

        auto filename = argv[1];
        ScaLBL_IonModel IonModel(rank,nprocs,comm);
        ScaLBL_Multiphys_Controller Study(rank,nprocs,comm);//multiphysics controller coordinating multi-model coupling

        // Load controller information
        Study.ReadParams(filename);

        // Initialize LB-Ion model
        IonModel.ReadParams(filename,Study.num_iter_Ion);
        IonModel.SetDomain();    
        IonModel.ReadInput();    
        IonModel.Create();       
        IonModel.Initialize();   
        IonModel.DummyFluidVelocity();
        IonModel.DummyElectricField();


        int timestep=0;
        double error = 1.0;
        vector<double>ci_avg_previous{0.0,0.0};//assuming 1:1 solution
        while (timestep < Study.timestepMax && error > Study.tolerance){
            
            timestep++;
            IonModel.Run(IonModel.FluidVelocityDummy,IonModel.ElectricFieldDummy); //solve for ion transport and electric potential
            timestep++;//AA operations

            if (timestep%Study.analysis_interval==0){
                error = IonModel.CalIonDenConvergence(ci_avg_previous);
            }
        }
        IonModel.getIonConcentration_debug(timestep);

        if (rank==0) printf("Maximum timestep is reached and the simulation is completed\n");
        if (rank==0) printf("*************************************************************\n");

        PROFILE_STOP("Main");
        PROFILE_SAVE("TestIonModel",1);
        // ****************************************************
        
        MPI_Barrier(comm);
        MPI_Comm_free(&comm);

    } // Limit scope so variables that contain communicators will free before MPI_Finialize

    Utilities::shutdown();
}


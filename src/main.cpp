#include<fstream>
#include<iostream>
#include<cstring>
#include<omp.h>
#include<random>
#include<cassert>

#include"zxlb.hpp"
#include"config.hpp"
#include"sim.hpp"
#include"unit_tests.hpp"
#include"mnist_reader.hpp"
#include"network_template.hpp"
#include"builder.hpp"
#include"pattern_maker.hpp"
#include"runs.hpp"
#include"exp_eeg.hpp"
#include"loader.hpp"
#include"exp_move.hpp"

// #ifdef DEBUG
//     #define RUN_TEST 1
// #else
//     #define RUN_TEST 0
// #endif




int main(int argc, char**argv) {

    // ExpEEG exp_eeg;
    // exp_eeg.LoadData("./exp_eeg/train", "./exp_eeg/test");
    // return 0;

    // std::cout << std::sqrt(0) << std::endl;
    // VecS v1( -1.22428754752690549523208574101,2.83163092469335131440288932936,89.6130481824433360316106700338);
    // VecS v2( 1.22428754943845707892080554302,-0.309961738331212655062074645684,87.6133275637629225229829899035);
    // VecS v3(  0.0, 0.0, 1);
    // VecS v4(  -M_PI/4.0, 0.1, 1);
    // VecS v5(  M_PI/4.0, 0.1, 1);
    // for(int i = 0; i < 100; i++) {
    //     std::cout << "Iteration: " << i << std::endl;
    //     std::cout << "v3 heading to v4   " << v3.HeadingTo(v4) << std::endl;
    //     std::cout << "v3 heading to v5   " <<v3.HeadingTo(v5) << std::endl;
    //     std::cout << "v3 distance to v4  " << v3.Distance(v4) << std::endl;
    //     std::cout << "v3 distance to v5  " << v3.Distance(v5) << std::endl;
    //     std::cout << "v3: " << v3.to_string() << std::endl;
    //     std::cout << "v4: " << v4.to_string() << std::endl;
    //     std::cout << "v5: " << v5.to_string() << std::endl;
    //     v3.Orbit(v3.HeadingTo(v4), 0.01);
    //     v3.Orbit(v3.HeadingTo(v5), 0.01);
    // }

    // return 0;
    // std::cout << v3.to_string() << std::endl;
    // std::cout << v4.to_string() << std::endl;
    // std::cout << v3.HeadingTo(v4) << std::endl;
    // std::cout << v4.HeadingTo(v3) << std::endl;
    
    // v3.Orbit(v3.HeadingTo(v4),v3.Distance(v4)/10.0);
    // // v4.Orbit(v4.HeadingTo(v3),v4.Distance(v3)/10.0);
    // std::cout << v3.to_string() << std::endl;
    // std::cout << v4.to_string() << std::endl;
    // return 0;
    // v1.Orbit(v1.HeadingTo(v2),v1.Distance(v2));
    // std::cout << v1.Distance(v2) << std::endl;
    // return 0;

    // // std::cout << v2.VectorTo(v1).to_string() << std::endl;
    // // double head = v2.HeadingTo(v1);
    // // std::cout << Vec3(std::cos(head), std::sin(head)).to_string() << std::endl;

    // // std::cout << v1.VectorTo(v2).ToVecS().to_string() << std::endl;

    // // double hv1v2 = v1.HeadingTo(v2);
    // // double hv1v3 = v1.HeadingTo(v3);
    // // double hv1v4 = v1.HeadingTo(v4);
    // // double hv1v23 = std::abs( hv1v2-hv1v3);
    // // double hv2v1 = v2.HeadingTo(v1);
    // std::cout << "V1: " << v1.to_string() << std::endl;
    // std::cout << "V2: " << v2.to_string() << std::endl;
    // std::cout << "V3: " << v3.to_string() << std::endl;
    // std::cout << "V4: " << v4.to_string() << std::endl;
    // double dist = v2.DistanceWithRadius(v3);
    // double delta = 0.01;
    // double x= 0.0;
    // while( x < dist) {

    //     v2.Orbit(v2.HeadingTo(v3), -delta);
    //     std::cout << "V2: " << v2.to_string() << std::endl;
    //     x += delta;
    // }
    // std::cout << "V1: " << v1.to_string() << std::endl;
    // std::cout << "V2: " << v2.to_string() << std::endl;
    // std::cout << "V3: " << v3.to_string() << std::endl;
    // std::cout << "V4: " << v4.to_string() << std::endl;
    // // std::cout << "V1->V2: " << hv1v2 << std::endl;
    // // std::cout << "V1->V3: " << hv1v3 << std::endl;
    // // std::cout << "V1->V23: " << hv1v23 << std::endl;
    // // std::cout << "V1->V4: " << hv1v4 << std::endl;
    // return 0;

    // Vec3 v3a(-10,-10,-10);
    // VecS vSa = v3a.ToVecS();
    // Vec3 v3b = vSa.ToVec3();
    // VecS vSb = v3b.ToVecS();
    // std::cout << "V3A: " << v3a.to_string() << std::endl;
    // std::cout << "VSA: " << vSa.to_string() << std::endl;
    // std::cout << "V3B: " << v3b.to_string() << std::endl;
    // std::cout << "VSB: " << vSb.to_string() << std::endl;


    // ExpMove em(
    //     10,
    //     10,
    //     1.0,
    //     100.0,
    //     -200,
    //     1300,
    //     true,
    //     true
    // );
    // return 0;


    // return 0;
    //-------------------------------------------------------------------------
    // INIT STUFF
    omp_set_num_threads(7);

    zxlb::LoadConstants("ds_baseline_preset.json");

    // LOGS
    zxlog::Init(false);

    str OUTPUT_PATH = "/run/media/zax/a06347ed-42d6-48d5-a380-ddcfcb7fcf75/output/project_radius/output/";
    str PRESET_PATH = "/run/media/zax/a06347ed-42d6-48d5-a380-ddcfcb7fcf75/output/project_radius/DS_A_1000_POINT/";

    //-------------------------------------------------------------------------
    // RANDOM
    zxlog::Debug("MAIN: Random init.");
    std::random_device rd;
    RNG rng(rd());

    //-------------------------------------------------------------------------
    // WRITER
    zxlog::Debug("MAIN: Setting up writer.");
    Writer writer(OUTPUT_PATH);

    //-------------------------------------------------------------------------
    // Process command line args.
    str network_id = "move";
    bool load_preset = false;

    zxlog::Debug("MAIN: Processing cmd line args.");
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i],"-h")==0) {
            std::cout << "Help is on the way\n";
        } else if(strcmp(argv[i],"-n")==0) {
            network_id = argv[++i];
            std::cout << "RUNNING SIM: " << network_id << std::endl;
        } else if(strcmp(argv[i],"-d")==0) {
            UnitTests();
        } else if(strcmp(argv[i],"-p")==0) {
            load_preset = true;
        }
    }

    //-------------------------------------------------------------------------
    // Init the config data
    config::InitConfig();

    //-------------------------------------------------------------------------
    // Load the network templates
    zxlog::Debug("MAIN: Begin reading network templates.");
    tmps::ReadNetworkTemplate("example_template_file.json");
    zxlog::Debug("MAIN: End reading network templates.");

    

    //-------------------------------------------------------------------------
    // Network
    
    zxlog::Debug("MAIN: Begin building network " + network_id + ".");
    uptr<Network> network = BuildNetwork(network_id, rng);
    zxlog::Debug("MAIN: End building network " + network_id + ".");

    network->InitWriteData();

    //-------------------------------------------------------------------------
    // Load preset if preset.
    
    if(load_preset) {
        NetData netdata = LoadNetworkOutput(PRESET_PATH);
        network->LoadPresets(netdata);
    }

    //-------------------------------------------------------------------------
    // MNIST
    // RunMNIST(
    //     &writer,
    //     network.get(),
    //     rng
    // );

    //-------------------------------------------------------------------------
    // POISSON002
    // RunPoisson002(
    //     &writer,
    //     network.get(),
    //     rng
    // );

    //-------------------------------------------------------------------------
    // POISSON004B
    // RunPoisson004_B(
    //     &writer,
    //     network.get(),
    //     rng
    // );

    //----------------------------------------------------------------------------
    // Pattern 002
    // zxlog::Debug("MAIN: Loading Pattern data");
    // PatternMaker pattern_maker = BuildPattern001(rng);
    // RunPattern001(
    //     &writer,
    //     network.get(),
    //     &pattern_maker,
    //     rng
    // );


    //----------------------------------------------------------------------------
    // Poisson Spike Trains
    // RunExpBeacon(
    //     &writer,
    //     network.get(),
    //     rng
    // );

    //----------------------------------------------------------------------------
    // Lines Vert
    // RunLinesVert(
    //     &writer,
    //     network.get(),
    //     rng
    // );


    //----------------------------------------------------------------------------
    // XOR
    // RunXOR(
    //     &writer,
    //     network.get(),
    //     rng
    // );

    //----------------------------------------------------------------------------
    // EEG
    // RunEEG2(
    //     &writer,
    //     network.get(),
    //     rng
    // );

    //----------------------------------------------------------------------------
    // Move
    // RunMove(
    //     &writer,
    //     network.get(),
    //     rng
    // );

    RunMove_Hand(
        &writer,
        network.get(),
        rng
    );

    return 0;
}


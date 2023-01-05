#include"zxlb_constants.hpp"
namespace zxlb {
    void LoadConstants(str filename) {
        std::ifstream ifs("const/"+filename);
        nlohmann::json j;
        ifs >> j;

        NEURON_TYPE = j.at("NEURON_TYPE");
        MAX_RADIUS = j.at("MAX_RADIUS");
        PRE_LEARNING_RATE = j.at("PRE_LEARNING_RATE");
        POST_LEARNING_RATE = j.at("POST_LEARNING_RATE");
        SYN_STRENGTH_LEARNING_RATE = j.at("SYN_STRENGTH_LEARNING_RATE");
        MAX_ANG_TEMPORAL_DIFFERENCE = j.at("MAX_ANG_TEMPORAL_DIFFERENCE");
        MAX_RAD_TEMPORAL_DIFFERENCE = j.at("MAX_RAD_TEMPORAL_DIFFERENCE");
        MAX_STR_TEMPORAL_DIFFERENCE = j.at("MAX_STR_TEMPORAL_DIFFERENCE");
        STR_LEARNING_DECAY_PRE = j.at("STR_LEARNING_DECAY_PRE");
        STR_LEARNING_DECAY_POST = j.at("STR_LEARNING_DECAY_POST");
        STR_LEARNING_DECAY_PRE = j.at("STR_LEARNING_DECAY_PRE");
        STR_LEARNING_DECAY_POST = j.at("STR_LEARNING_DECAY_POST");
        BF = j.at("BF");
        MAX_COMPARTMENT_LENGTH = j.at("MAX_COMPARTMENT_LENGTH");
        DENDRITE_SIGNAL_WEIGHT = j.at("DENDRITE_SIGNAL_WEIGHT");
        if(j.contains("SYNAPSE_SIGNAL_WEIGHT")) {
            SYNAPSE_SIGNAL_WEIGHT = j.at("SYNAPSE_SIGNAL_WEIGHT");
        }
        TRAIN_ANG = j.at("TRAIN_ANG");
        TRAIN_RAD = j.at("TRAIN_RAD");
        TRAIN_STR = j.at("TRAIN_STR");
        CORRECT_EXPECTED = j.at("CORRECT_EXPECTED");
        INCORRECT_EXPECTED = j.at("INCORRECT_EXPECTED");
        TASK_DURATION = j.at("TASK_DURATION");
        NUM_ITERATIONS = j.at("NUM_ITERATIONS");
        B_SYN_SIG_MOD = j.at("B_SYN_SIG_MOD");
        C_SYN_SIG_MOD = j.at("C_SYN_SIG_MOD");
        B_SYN_SIG_SELF = j.at("B_SYN_SIG_SELF");
        C_SYN_SIG_SELF = j.at("C_SYN_SIG_SELF");
        B_SYN_STR = j.at("B_SYN_STR");
        C_SYN_STR = j.at("C_SYN_STR");
        B_SYN_STR_PRE = j.at("B_SYN_STR_PRE");
        C_SYN_STR_PRE = j.at("C_SYN_STR_PRE");
        B_SYN_STR_POST = j.at("B_SYN_STR_POST");
        C_SYN_STR_POST = j.at("C_SYN_STR_POST");
        B_NEU_DEN_OUT = j.at("B_NEU_DEN_OUT");
        C_NEU_DEN_OUT = j.at("C_NEU_DEN_OUT");

    }
}
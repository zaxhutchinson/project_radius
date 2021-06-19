#pragma once


#include"zxlb.hpp"
#include"vec_s.hpp"
#include"writer.hpp"
#include"data.hpp"
#include"connection.hpp"

struct Synapse {

    i64 id;
    i64 neuron_id;
    i64 layer_id;
    double signal[2];
    double bap[2];
    double max_strength;
    double abs_max_strength;
    double cur_strength;
    VecS location;
    double dendrite_path_length;
    double error;
    i64 time_error;
    i64 time_cur_spike;
    i64 time_pre_spike;
    i64 spikes;
    i64 parent;
    vec<i64> children;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<SynapseData> data;
    ConnectionAddress ca;

    // Helper variable.
    // Stores upstream signals during the
    // neural input process.
    double upstream_signal;
    bool upstream_eval;


    Synapse();
    Synapse(
        VecS _loc,
        double _max_strength,
        double _cur_strength
    );
    Synapse(const Synapse & s) = default;
    Synapse(Synapse && s) = default;
    Synapse& operator=(const Synapse & s) = default;
    Synapse& operator=(Synapse && s) = default;
    //------------------------------------------------------------------------

    void Reset();

    double GetError();

    void SetID(i64 _id);
    void SetLayerID(i64 _id);
    void SetNeuronID(i64 _id);

    void SetConnectionAddress(ConnectionAddress ca);
    void SetSignal(i64 time, double amt);
    void SetCurSpike(i64 time);

    /* SetPathLength:
        dendrite_path_length is a temporary var that stores how far this
        synapse is from the soma when distanse is measured by the dendrite.
    */
    void SetDendritePathLength(double path);

    /* GetStrength
        Returns the current strength of the synapse
        normalized by the max strength.
    */
    double GetStrength();
    double GetStrengthDelta();

    double GetSignal(i64 time);
    double GetSignalWithStrength(i64 time);
    double GetSignalFull(i64 time);

    double GetDendritePathLength();

    //------------------------------------------------------------------------
    // Dendrite connections
    i64 GetParent();
    vec<i64> * GetChildren();

    //------------------------------------------------------------------------
    // Upstream signal
    double GetUpstreamSignal();
    void ResetUpstreamSignal();
    void AddToUpstreamSignal(double sig);
    bool GetUpstreamEval();
    void SetUpstreamEval(bool b);

    //------------------------------------------------------------------------
    //
    bool Update(i64 time, Writer * writer, ConnectionMatrix & cm);
    void ResetWriteData();
    void SaveData(i64 time);
    void WriteData(Writer * writer);
    void CleanupData(Writer * writer);
    
};
#pragma once


#include"zxlb.hpp"
#include"vec_s.hpp"
#include"writer.hpp"
#include"data.hpp"
#include"loader.hpp"
#include"connection.hpp"
#include"neuron_type.hpp"

struct DenSig {
    double D;
    double Ts;
    double sig;
    DenSig()
        : D(0.0), Ts(0.0), sig(0.0) {}
    DenSig(double _D, double _Ts, double _sig) 
        : D(_D), Ts(_Ts), sig(_sig) {}
    DenSig(const DenSig & d) = default;
    DenSig(DenSig && d) = default;
    DenSig& operator=(const DenSig & d) = default;
    DenSig& operator=(DenSig && d) = default;
};

struct Synapse {

    i64 id;
    i64 neuron_id;
    i64 layer_id;
    double signal[2];
    i64 bap;
    double max_strength;
    double abs_max_strength;
    double cur_strength;
    VecS location;
    double dendrite_path_length;
    double straightline_path_length;
    double error;
    i64 time_error;
    i64 time_cur_spike;
    i64 time_pre_spike;
    lst<i64> spike_queue;
    lst<i64> comp_spike_queue;
    i64 spikes;
    i64 parent;
    vec<i64> children;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<SynapseData> data;
    ConnectionAddress ca;
    i64 compartment;
    double compartment_length;
    double polarity;

    // Helper variable.
    // Stores upstream signals during the
    // neural input process.
    double upstream_signal;
    bool upstream_eval;
    vec<DenSig> densigs;
    i64 compartment_size; // # of syns.
    double dist_to_parent;

    bool is_comp_root;
    double cur_comp_strength;
    double max_comp_strength;
    double abs_max_comp_strength;


    Synapse();
    Synapse(
        VecS _loc,
        double _max_strength,
        double _cur_strength,
        double _polarity
    );
    Synapse(const Synapse & s) = default;
    Synapse(Synapse && s) = default;
    Synapse& operator=(const Synapse & s) = default;
    Synapse& operator=(Synapse && s) = default;
    //------------------------------------------------------------------------
    void LoadPreset(SynData & syndata);
    void Reset();

    double GetError();

    void SetID(i64 _id);
    void SetLayerID(i64 _id);
    void SetNeuronID(i64 _id);

    void SetConnectionAddress(ConnectionAddress ca);
    void SetSignal(i64 time, double amt);
    void SetCurSpike(i64 time);
    i64 GetCurSpike();

    /* SetPathLength:
        dendrite_path_length is a temporary var that stores how far this
        synapse is from the soma when distanse is measured by the dendrite.
    */
    void SetDendritePathLength(double path);
    void SetStraightLinePathLength(double path);

    /* GetStrength
        Returns the current strength of the synapse
        normalized by the max strength.
    */
    double GetStrength();
    double GetStrengthDelta();
    void ChangeStrengthPre(
        i64 pre_spike_time,
        i64 post_spike_time,
        ConnectionMatrix & cm
    );
    void ChangeStrengthPost(
        i64 pre_spike_time,
        i64 post_spike_time_cur,
        i64 post_spike_time_prev,
        ConnectionMatrix & cm
    );
    void ChangeStrengthPre_AD(i64 time, i64 cur_neuron_spike, double _error, ConnectionMatrix & cm);
    void ChangeStrengthPost_AD(i64 time, double _error, ConnectionMatrix & cm, vec<i64> & comp_spikes);
    
    void ChangeStrengthCompartment(i64 time, double _error, vec<Synapse> & syns);
    void ChangeStrengthCompartment_Between(i64 time, double _error,lst<i64> & comp_spikes,double compdist);
    void ChangeStrengthCompartment_Within(i64 time, double _error, vec<Synapse> & syns,lst<i64> & comp_spikes);
    
    void ChangeStrengthPre_Simple(i64 time, double _error, ConnectionMatrix & cm);
    void ChangeStrengthPost_Simple(i64 time, double _error, ConnectionMatrix & cm);

    void SetBAP(i64 time);

    double psi(double sig);

    double GetInput(i64 time, vec<Synapse> & syns, i64 parent_id, double compdist);

    double GetInput_Within(
        i64 time, 
        vec<Synapse> & syns,
        vec<i64> & comp_spikes,
        vec<i64> & syns_per_comp,
        vec<pair<double,double>> & comp_izh,
        tmps::NeuronType & nt,
        i64 parent_id, 
        double compdist,
        double error,
        bool train_str
    );

    double GetInput_Between(
        i64 time, 
        vec<Synapse> & syns,
        vec<i64> & comp_spikes,
        vec<i64> & syns_per_comp,
        vec<pair<double,double>> & comp_izh,
        tmps::NeuronType & nt,
        i64 parent_id, 
        double compdist,
        double error,
        bool train_str
    );

    // THE GOOD ONE
    double GetSignal_Within(i64 time, double compdist);
    double GetSignal_Between(i64 time, i64 comp_spike_time, double compdist, i64 syns_in_comp);

    double GetSignal_Simple(i64 time);
    double GetSignal_In(i64 time);
    double GetSignal_Out(i64 time);
    double GetSignalWitch_Self(i64 time);
    double GetSignalWitchMod(i64 time, double dist, double spike_time_diff);
    // double GetSignalWitchMod_Out(i64 time, double dist, double spike_time_diff);

    double GetDendritePathLength();

    void SetCompartment(i64 comp_id);
    i64 GetCompartment();
    void SetCompartmentLength(double len);
    double GetCompartmentLength();
    //------------------------------------------------------------------------
    // Dendrite connections
    i64 GetParent();
    vec<i64> * GetChildren();

    //------------------------------------------------------------------------
    // Upstream signal
    double GetUpstreamSignal();
    void SetUpstreamSignal(double sig);
    void ResetUpstreamSignal();
    void AddToUpstreamSignal(double sig);
    bool GetUpstreamEval();
    void SetUpstreamEval(bool b);

    vec<DenSig> GetDendriticSignals();
    void AddDendriticSignal(DenSig sig);
    void AddDendriticSignals(vec<DenSig> sigs);
    void ResetDendriticSignals();

    int GetCompartmentSize();
    void AddToCompartmentSize(int amt);
    void ResetCompartmentSize();
    //------------------------------------------------------------------------
    //
    bool Update(i64 time, Writer * writer, ConnectionMatrix & cm);
    void ResetWriteData();
    void SaveData(i64 time);
    void WriteData(Writer * writer);
    void CleanupData(Writer * writer);
    
};
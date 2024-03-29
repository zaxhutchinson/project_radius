#pragma once

#include"zxlb.hpp"
#include"layer.hpp"
#include"connection.hpp"
#include"loader.hpp"

class Network {
private:
    vec<Layer> layers;
    i64 input_layer;
    i64 output_layer;
    ConnectionMatrix connection_matrix;
public:
    Network() = default;
    Network(const Network & n) = default;
    Network(Network && n) = default;
    Network& operator=(const Network & n) = default;
    Network& operator=(Network && n) = default;

    void LoadPresets(NetData & netdata);
    
    void Reset(bool purge_data = false);
    void RandomizeOrder(RNG & rng);
    //------------------------------------------------------------------------
    /* AddLayer:
        Adds a layer to the network. Sets its id based on its place in the
        vector. Returns the new layer's id.
    */
    i64 AddLayer(Layer layer);
    Layer * GetLayer(i64 index);
    i64 GetOutputLayerIndex();
    i64 GetInputLayerIndex();

    void SetInputs(vec<double> & inputs);
    void SetOutputs(vec<Connection*> & outputs);

    void Update(i64 time, Writer * writer, RNG & rng);

    void UpdateLayerErrorValues(
        vec<double> & rates,
        i64 layer_id
    );
    void UpdateLayerErrorValuesSeparate(
        vec<double> & pos_rates,
        vec<double> & weight_rates,
        i64 layer_id
    );

    vec<double> GetPosErrorRates(i64 layer_id);
    vec<double> GetWeightErrorRates(i64 layer_id);

    void InitDendrites();
    void RebuildDendrites();

    void InitWriteData();
    void CleanUpData(Writer * writer);
    void SaveData(i64 time);
    void WriteData(Writer * writer);
};


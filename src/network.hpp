#pragma once

#include"zxlb.hpp"
#include"layer.hpp"
#include"connection.hpp"


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
    
    void Reset();
    //------------------------------------------------------------------------
    /* AddLayer:
        Adds a layer to the network. Sets its id based on its place in the
        vector. Returns the new layer's id.
    */
    i64 AddLayer(Layer layer);
    Layer * GetLayer(i64 index);
    i64 GetOutputLayerIndex();

    void SetInputs(vec<double> & inputs);
    void SetOutputs(vec<Connection*> & outputs);

    void Update(i64 time, Writer * writer);

    void UpdateLayerErrorValues(
        vec<double> & rates,
        i64 layer_id
    );

    vec<double> GetErrorRates(i64 layer_id);

    void RebuildDendrites();

    void InitWriteData();
    void CleanUpData(Writer * writer);
    void SaveData(i64 time);
    void WriteData(Writer * writer);
};


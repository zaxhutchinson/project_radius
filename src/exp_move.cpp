#include"exp_move.hpp"

ExpMove::ExpMove(
    int _width,
    int _height,
    double _b,
    double _c,
    int _num_ts,
    bool _v,
    bool _h
) {

    field_width = _width;
    field_height = _height;
    time_steps = _num_ts;
    b = _b; c = _c;

    instances.emplace(MoveType::VERTICAL,vec<ExpMoveInstance>());
    instances.emplace(MoveType::HORIZONTAL,vec<ExpMoveInstance>());

    if(_v) {
        // Make vertical instances
        for(int w = 0; w < _width; w++) {

            ExpMoveInstance emi;
            emi.type = MoveType::VERTICAL;

            for(int ts = 0; ts < _num_ts; ts++) {

                vec<double> data;

                double x = 0.0;
                if(ts!=0)
                    x = static_cast<double>(ts) / 
                            (static_cast<double>(time_steps) / static_cast<double>(field_height));

                for(int i = 0; i < _width; i++) {
                    for(int j = 0; j < _height; j++) {
                        if(i==w) {
                            data.push_back(Witch(x, j));
                        } else {
                            data.push_back(0.0);
                        }
                        std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
                    }
                }

                emi.signals.emplace_back(std::move(data));          
            }

            instances[MoveType::VERTICAL].emplace_back(std::move(emi));

        }
    }

    if(_h) {
        // Make horizontal instances
        for(int h = 0; h < _height; h++) {

            ExpMoveInstance emi;
            emi.type = MoveType::HORIZONTAL;

            for(int ts = 0; ts < _num_ts; ts++) {

                vec<double> data;

                double x = 0.0;
                if(ts!=0)
                    x = static_cast<double>(ts) / 
                            (static_cast<double>(time_steps) / static_cast<double>(field_width));

                for(int i = 0; i < _width; i++) {
                    for(int j = 0; j < _height; j++) {
                        if(j==h) {
                            data.push_back(Witch(x, i));
                        } else {
                            data.push_back(0.0);
                        }
                        std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
                    }
                }

                emi.signals.emplace_back(std::move(data));          
            }

            instances[MoveType::HORIZONTAL].emplace_back(std::move(emi));

        }
    }

}

vec<ExpMoveInstance> & ExpMove::GetInstancesOfType(MoveType type) {
    return instances.at(type);
}

vec<ExpMoveInstance> ExpMove::GetAllInstances() {
    vec<ExpMoveInstance> v;

    for(
        umap<MoveType,vec<ExpMoveInstance>>::iterator it = instances.begin();
        it != instances.end();
        it++
    ) {
        for(
            vec<ExpMoveInstance>::iterator it2 = it->second.begin();
            it2 != it->second.end();
            it2++
        ) {
            v.push_back(*it2);
        }
    }
    return v;
}

double ExpMove::Witch(double x, double a) {
    return c / (std::pow( (x-a)/b, 2.0 )+1.0);
}
#include"exp_move.hpp"

ExpMove::ExpMove(
    int _width,
    int _height,
    double _b,
    double _c,
    int _time_start,
    int _time_end,
    vec<MoveType> types
) {

    field_width = _width;
    field_height = _height;
    time_start = _time_start;
    time_end = _time_end;
    b = _b; c = _c;

    // double time_steps = time_end-time_start;

    // instances.emplace(MoveType::LEFT,vec<ExpMoveInstance>());
    

    for(
        vec<MoveType>::iterator it = types.begin();
        it != types.end();
        it++
    ) {
        

        if(*it==MoveType::DOWN) {
            
            instances.emplace(MoveType::DOWN,vec<ExpMoveInstance>());

            for(int w = 0; w < _width; w++) {

                ExpMoveInstance emi;
                emi.type = MoveType::DOWN;
                emi.rc = w;

                for(int ts = time_start; ts < time_end; ts++) {

                    vec<double> data;

                    double x = static_cast<double>(ts) / 
                                (1000.0 / static_cast<double>(field_height));

                    for(int i = 0; i < _width; i++) {
                        for(int j = 0; j < _height; j++) {
                            if(i==w) {
                                data.push_back(Witch(x, j+1));
                            } else {
                                data.push_back(0.0);
                            }
                            // std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
                        }
                    }

                    emi.signals.emplace_back(std::move(data));          
                }

                instances[MoveType::DOWN].emplace_back(std::move(emi));

            }
        }

        else if(*it==MoveType::UP) {

            instances.emplace(MoveType::UP,vec<ExpMoveInstance>());

            for(int w = 0; w < _width; w++) {

                ExpMoveInstance emi;
                emi.type = MoveType::UP;
                emi.rc = w;

                for(int ts = time_end-1; ts >= time_start; ts--) {

                    vec<double> data;

                    double x = static_cast<double>(ts) / 
                                (1000.0 / static_cast<double>(field_height));

                    for(int i = 0; i < _width; i++) {
                        for(int j = 0; j < _height; j++) {
                            if(i==w) {
                                data.push_back(Witch(x, j+1));
                            } else {
                                data.push_back(0.0);
                            }
                            // std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
                        }
                    }

                    emi.signals.emplace_back(std::move(data));          
                }

                instances[MoveType::UP].emplace_back(std::move(emi));

            }
        }

        else if(*it==MoveType::RIGHT) {

            instances.emplace(MoveType::RIGHT,vec<ExpMoveInstance>());
            
            for(int h = 0; h < _height; h++) {


                ExpMoveInstance emi;
                emi.type = MoveType::RIGHT;
                emi.rc = h;

                for(int ts = time_start; ts < time_end; ts++) {

                    vec<double> data;

                    double x = static_cast<double>(ts) / 
                                (1000.0 / static_cast<double>(field_width));

                    for(int i = 0; i < _width; i++) {
                        for(int j = 0; j < _height; j++) {
                            if(j==h) {
                                data.push_back(Witch(x, i+1));
                            } else {
                                data.push_back(0.0);
                            }
                            // std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
                        }
                    }

                    emi.signals.emplace_back(std::move(data));          
                }

                instances[MoveType::RIGHT].emplace_back(std::move(emi));

            }
        }
        else if(*it==MoveType::LEFT) {

            instances.emplace(MoveType::LEFT,vec<ExpMoveInstance>());
            
            for(int h = 0; h < _height; h++) {

                ExpMoveInstance emi;
                emi.type = MoveType::LEFT;
                emi.rc = h;

                for(int ts = time_end-1; ts >= time_start; ts--) {

                    vec<double> data;

                    double x = static_cast<double>(ts) / 
                                (1000.0 / static_cast<double>(field_width));

                    for(int i = 0; i < _width; i++) {
                        for(int j = 0; j < _height; j++) {
                            if(j==h) {
                                data.push_back(Witch(x, i+1));
                            } else {
                                data.push_back(0.0);
                            }
                            // std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
                        }
                    }

                    emi.signals.emplace_back(std::move(data));          
                }

                instances[MoveType::LEFT].emplace_back(std::move(emi));

            }
        }
    }

}

// ExpMove::ExpMove(
//     int _width,
//     int _height,
//     double _sigma,
//     int _num_ts,
//     bool _v,
//     bool _h
// ) {

//     field_width = _width;
//     field_height = _height;
//     time_steps = _num_ts;
//     sigma = _sigma;

//     instances.emplace(MoveType::VERTICAL,vec<ExpMoveInstance>());
//     instances.emplace(MoveType::HORIZONTAL,vec<ExpMoveInstance>());

//     if(_v) {
//         // Make vertical instances
//         for(int w = 0; w < _width; w++) {

//             ExpMoveInstance emi;
//             emi.type = MoveType::VERTICAL;

//             for(int ts = 0; ts < _num_ts; ts++) {

//                 vec<double> data;

//                 double x = 0.0;
//                 if(ts!=0)
//                     x = static_cast<double>(ts) / 
//                             (static_cast<double>(time_steps) / static_cast<double>(field_height));

//                 for(int i = 0; i < _width; i++) {
//                     for(int j = 0; j < _height; j++) {
//                         if(i==w) {
//                             data.push_back(Gauss(x, j));
//                         } else {
//                             data.push_back(0.0);
//                         }
//                         // std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
//                     }
//                 }

//                 emi.signals.emplace_back(std::move(data));          
//             }

//             instances[MoveType::VERTICAL].emplace_back(std::move(emi));

//         }
//     }

//     if(_h) {
//         // Make horizontal instances
//         for(int h = 0; h < _height; h++) {

//             ExpMoveInstance emi;
//             emi.type = MoveType::HORIZONTAL;

//             for(int ts = 0; ts < _num_ts; ts++) {

//                 vec<double> data;

//                 double x = 0.0;
//                 if(ts!=0)
//                     x = static_cast<double>(ts) / 
//                             (static_cast<double>(time_steps) / static_cast<double>(field_width));

//                 for(int i = 0; i < _width; i++) {
//                     for(int j = 0; j < _height; j++) {
//                         if(j==h) {
//                             data.push_back(Gauss(x, i));
//                         } else {
//                             data.push_back(0.0);
//                         }
//                         // std::cout << x << "--" << i << " " << j << ": " << data.back() << std::endl;
//                     }
//                 }

//                 emi.signals.emplace_back(std::move(data));          
//             }

//             instances[MoveType::HORIZONTAL].emplace_back(std::move(emi));

//         }
//     }

// }

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

double ExpMove::Gauss(double x, double mu) {
    return (1.0/(sigma*std::sqrt(2.0*M_PI))) * std::exp(-0.5*std::pow((x-mu)/sigma,2.0));
}
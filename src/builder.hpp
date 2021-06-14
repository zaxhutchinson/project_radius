#pragma once

#include<random>

#include"zxlb.hpp"
#include"log.hpp"
#include"vec_s.hpp"
#include"network_template.hpp"
#include"network.hpp"

uptr<Network> BuildNetwork(str network_id, RNG & rng);
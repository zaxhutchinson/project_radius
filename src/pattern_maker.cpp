#include"pattern_maker.hpp"



PatternMaker::PatternMaker(sizet size) {
    pattern_size = size;
}

void PatternMaker::AddPatterns(vec<PatternTemplate> & templates, RNG & rng) {
    for(
        vec<PatternTemplate>::iterator it = templates.begin();
        it != templates.end();
        it++
    ) {
        assert(it->size==pattern_size);

        vec<std::normal_distribution<double>> dists;
        for(sizet i = 0; i < it->size; i++) {
            dists.push_back(
                std::normal_distribution<double>(it->mus[i], it->stdevs[i])
            );
        }

        vec<Pattern> pvec;
        for(sizet i = 0; i < it->amount; i++) {
            Pattern p;
            p.label = it->label;
            for(sizet i = 0; i < it->size; i++) {
                p.data.push_back(
                    dists[i](rng)
                );
            }
            pvec.push_back(std::move(p));
        }
        patterns[it->label] = std::move(pvec);
        patterns_per_iteration[it->label] = it->amount_per_iteration;

    }
}

vec<Pattern> & PatternMaker::GetAllWithLabel(str label) {
    return patterns[label];
}

vec<Pattern*> PatternMaker::GetRandomOneOfEachOrdered(RNG & rng) {
    vec<Pattern*> v;
    for(
        umap<str,vec<Pattern>>::iterator it = patterns.begin();
        it != patterns.end();
        it++
    ) {
        std::uniform_int_distribution<sizet> dist(0,it->second.size()-1);
        v.push_back(&(it->second[dist(rng)]));
    }
    return v;
}
vec<Pattern*> PatternMaker::GetRandomOneOfEachRandomized(RNG & rng) {
    vec<Pattern*> v;
    for(
        umap<str,vec<Pattern>>::iterator it = patterns.begin();
        it != patterns.end();
        it++
    ) {
        std::uniform_int_distribution<sizet> dist(0,it->second.size()-1);
        v.push_back(&(it->second[dist(rng)]));
    }
    std::shuffle(v.begin(), v.end(), rng);
    return v;
}
vec<str> PatternMaker::GetAllLabels() {
    vec<str> v;
    for(
        umap<str, vec<Pattern>>::iterator it = patterns.begin();
        it != patterns.end();
        it++
    ) {
        v.push_back(it->first);
    }
    return v;
}
sizet PatternMaker::GetIterationSize() {
    sizet s = 0;
    for(
        umap<str, sizet>::iterator it = patterns_per_iteration.begin();
        it != patterns_per_iteration.end();
        it++
    ) {
        s += it->second;
    }
    return s;
}
vec<vec<Pattern>> PatternMaker::GetDataAsIteration(
    vec<str> labels,
    sizet num_iterations,
    RNG & rng
) {
    vec<vec<Pattern>> data_by_label;
    for(sizet i = 0; i < labels.size(); i++) {
        vec<Pattern> data = GetAllWithLabel(labels[i]);
        std::shuffle(data.begin(), data.end(), rng);
        data.resize(num_iterations * patterns_per_iteration[labels[i]]);
        data_by_label.push_back(std::move(data));
    }

    vec<vec<Pattern>> data_by_iteration;
    for(sizet n = 0; n < num_iterations; n++) {
        vec<Pattern> iteration;
        for(sizet l = 0; l < data_by_label.size(); l++) {
            str label = data_by_label[l].back().label;
            for(sizet i = 0; i < patterns_per_iteration[label]; i++) {
                iteration.push_back(data_by_label[l].back());
                data_by_label[l].pop_back();
            }
        }
        std::shuffle(iteration.begin(), iteration.end(), rng);
        data_by_iteration.push_back(std::move(iteration));
    }
    return data_by_iteration;
}







PatternMaker BuildPattern001(RNG & rng) {
    PatternMaker pattern_maker(16);

    double mu_good = 500.0;
    double std_good = 1.0;
    double mu_bad = 50.0;
    double std_bad = 10.0;
    sizet base_amount = 10000;

    PatternTemplate pt0;
    pt0.label = "0";
    pt0.size = 16;
    pt0.amount_per_iteration = 1;
    pt0.amount = base_amount * pt0.amount_per_iteration;
    
    pt0.mus = {
        mu_good, mu_good, mu_good, mu_good,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad
    };
    pt0.stdevs = {
        std_good, std_good, std_good, std_good,
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad
    };

    PatternTemplate pt1;
    pt1.label = "1";
    pt1.size = 16;
    pt1.amount_per_iteration = 1;
    pt1.amount = base_amount * pt1.amount_per_iteration;
    
    pt1.mus = {
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_good, mu_good, mu_good, mu_good,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad
    };
    pt1.stdevs = {
        std_bad, std_bad, std_bad, std_bad,
        std_good, std_good, std_good, std_good,
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad
    };

    PatternTemplate pt2;
    pt2.label = "2";
    pt2.size = 16;
    pt2.amount_per_iteration = 1;
    pt2.amount = base_amount * pt2.amount_per_iteration;
    
    pt2.mus = {
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_good, mu_good, mu_good, mu_good,
        mu_bad, mu_bad, mu_bad, mu_bad
    };
    pt2.stdevs = {
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad,
        std_good, std_good, std_good, std_good,
        std_bad, std_bad, std_bad, std_bad
    };

    PatternTemplate pt3;
    pt3.label = "3";
    pt3.size = 16;
    pt3.amount_per_iteration = 1;
    pt3.amount = base_amount * pt3.amount_per_iteration;
    
    pt3.mus = {
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_good, mu_good, mu_good, mu_good
    };
    pt3.stdevs = {
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad,
        std_good, std_good, std_good, std_good
    };

    vec<PatternTemplate> pts = {
        pt0, pt1, pt2, pt3
    };

    pattern_maker.AddPatterns(pts, rng);

    return pattern_maker;

}

PatternMaker BuildPattern002(RNG & rng) {
    PatternMaker pattern_maker(16);

    double mu_good = 400.0;
    double std_good = 1.0;
    double mu_bad = 100.0;
    double std_bad = 50.0;
    sizet base_amount = 10000;

    PatternTemplate pt0;
    pt0.label = "0";
    pt0.size = 16;
    pt0.amount_per_iteration = 1;
    pt0.amount = base_amount * pt0.amount_per_iteration;
    
    pt0.mus = {
        mu_good, mu_good, mu_good, mu_good,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0
    };
    pt0.stdevs = {
        std_good, std_good, std_good, std_good,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0
    };

    PatternTemplate pt1;
    pt1.label = "1";
    pt1.size = 16;
    pt1.amount_per_iteration = 1;
    pt1.amount = base_amount * pt1.amount_per_iteration;
    
    pt1.mus = {
        0.0, 0.0, 0.0, 0.0,
        mu_good, mu_good, mu_good, mu_good,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0
    };
    pt1.stdevs = {
        0.0, 0.0, 0.0, 0.0,
        std_good, std_good, std_good, std_good,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0
    };

    PatternTemplate pt2;
    pt2.label = "2";
    pt2.size = 16;
    pt2.amount_per_iteration = 1;
    pt2.amount = base_amount * pt2.amount_per_iteration;
    
    pt2.mus = {
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        mu_good, mu_good, mu_good, mu_good,
        0.0, 0.0, 0.0, 0.0
    };
    pt2.stdevs = {
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        std_good, std_good, std_good, std_good,
        0.0, 0.0, 0.0, 0.0
    };

    PatternTemplate pt3;
    pt3.label = "3";
    pt3.size = 16;
    pt3.amount_per_iteration = 1;
    pt3.amount = base_amount * pt3.amount_per_iteration;
    
    pt3.mus = {
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        mu_good, mu_good, mu_good, mu_good
    };
    pt3.stdevs = {
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        std_good, std_good, std_good, std_good
    };

    PatternTemplate pt4;
    pt4.label = "4";
    pt4.size = 16;
    pt4.amount_per_iteration = 4;
    pt4.amount = base_amount * pt4.amount_per_iteration;
    
    pt4.mus = {
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad,
        mu_bad, mu_bad, mu_bad, mu_bad
    };
    pt4.stdevs = {
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad,
        std_bad, std_bad, std_bad, std_bad
    };

    vec<PatternTemplate> pts = {
        pt0, pt1, pt2, pt3, pt4
    };

    pattern_maker.AddPatterns(pts, rng);

    return pattern_maker;

}


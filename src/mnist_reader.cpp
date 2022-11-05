#include"mnist_reader.hpp"


void MNISTData::NormalizeByTotalStrength() {
    double total = 0.0;
    for(sizet i = 0; i < image.size(); i++) {
        total += image[i];
    }
    for(sizet i = 0; i < image.size(); i++) {
        image[i] = (image[i] / total) * 1000.0;
    }

    // for(sizet i = 0; i < image.size(); i++) {
    //     image[i] = (image[i] / 255.0) * 100.0;
    // }
}




MNISTReader::MNISTReader() {

}

void MNISTReader::LoadData(std::string & label_filename, std::string & images_filename, bool normalize) {


    std::ifstream ifs(label_filename, std::ios::binary);
    if(ifs) {
        ifs.read(reinterpret_cast<char*>(&labels_magic_number), 4);
        ifs.read(reinterpret_cast<char*>(&labels_size), 4);

        labels_magic_number = reverseInt(labels_magic_number);
        labels_size = reverseInt(labels_size);

        char label;
        while(!ifs.eof()) {
            ifs.read(&label,1);
            if(!ifs.fail()) {
                MNISTData md;
                md.label = static_cast<unsigned>(label);
                data.push_back(md);
            }
        }
    }
    ifs.close();

    ifs.open(images_filename, std::ios::binary);
    if(ifs) {

        ifs.read(reinterpret_cast<char*>(&images_magic_number), 4);
        ifs.read(reinterpret_cast<char*>(&images_size), 4);
        ifs.read(reinterpret_cast<char*>(&images_rows), 4);
        ifs.read(reinterpret_cast<char*>(&images_cols), 4);

        images_magic_number = reverseInt(images_magic_number);
        images_size = reverseInt(images_size);
        images_rows = reverseInt(images_rows);
        images_cols = reverseInt(images_cols);


        int i = 0;
        unsigned char pixel[1];
        while(!ifs.eof()) {
            vec<double> image;
            for(int i = 0; i < images_rows*images_cols; i++) {
                ifs.read(reinterpret_cast<char*>(pixel),1);
                image.push_back(static_cast<double>(*pixel));
            }
            if(!ifs.fail()) {
                data[i].image=std::move(image);
                if(normalize) {
                    data[i].NormalizeByTotalStrength();
                }
                i++;
            }
                
        }
    }
    ifs.close();

}

vec<MNISTData> MNISTReader::GetDataWithLabel(unsigned label, MNISTDIR trans) {
    vec<MNISTData> d;
    for(unsigned i = 0; i < data.size(); i++) {
        if(label==data[i].label) {
            d.push_back(FlipDataAt(i,trans));
        }
    }
    return d;
}

vec<MNISTData> & MNISTReader::GetData() {
    return data;
}

MNISTData MNISTReader::GetDataAt(unsigned i) {
    return data.at(i);
}

uptr<InputGenerator_Poisson> MNISTReader::GetDataAsPoissonInputGenerator(unsigned i) {
    MNISTData d = data[i];
    uptr<InputGenerator_Poisson> ig = std::make_unique<InputGenerator_Poisson>();
    ig->id = std::to_string(d.label);
    for(sizet i = 0; i < d.image.size(); i++) {
        ig->decay.push_back(std::exp(-1.0/10));
        ig->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig->strength.push_back(500.0);
        ig->signal.push_back(0.0);
        ig->rate.push_back(d.image[i]/255.0);
    }
    return ig;
}

uptr<InputGenerator_Poisson> MNISTReader::GetDataAsPoissonInputGenerator(MNISTData & d) {
    uptr<InputGenerator_Poisson> ig = std::make_unique<InputGenerator_Poisson>();
    ig->id = std::to_string(d.label);
    for(sizet i = 0; i < d.image.size(); i++) {
        ig->decay.push_back(std::exp(-1.0/10));
        ig->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig->strength.push_back(500.0);
        ig->signal.push_back(0.0);
        ig->rate.push_back(d.image[i]/255.0);
    }
    return ig;
}

vec<vec<MNISTData>> MNISTReader::GetDataAsIteration(vec<unsigned> & labels, sizet num_iterations, sizet examples_per_iteration, RNG & rng) {
    zxlog::Debug("MNISTReader: GetDataAsIteration");
    vec<vec<MNISTData>> data_by_label;
    for(sizet i = 0; i < labels.size(); i++) {
        vec<MNISTData> data = GetDataWithLabel(labels[i]);
        std::shuffle(data.begin(), data.end(), rng);
        data.resize(num_iterations*examples_per_iteration);

        zxlog::Debug(
            "Label "+std::to_string(labels[i]) +
            ", Num examples "+std::to_string(data.size())
        );

        data_by_label.push_back(std::move(data));
    }

    vec<vec<MNISTData>> data_by_iteration;
    for(sizet n = 0; n < num_iterations; n++) {
        vec<MNISTData> iteration;
        for(sizet l = 0; l < data_by_label.size(); l++) {
            for(sizet i = 0; i < examples_per_iteration; i++) {

                // zxlog::Debug(
                //     "Data by label size " + std::to_string(data_by_label[l].size())
                // );

                iteration.push_back(data_by_label[l].back());
                data_by_label[l].pop_back();
            }
        }
        std::shuffle(iteration.begin(), iteration.end(), rng);
        data_by_iteration.push_back(iteration);
    }

    return data_by_iteration;
}

void MNISTReader::ToPPM(unsigned i) {
    MNISTData md = GetDataAt(i);

    std::ofstream ofs("image_"+std::to_string(i)+".ppm");
    ofs << "P2" << std::endl;
    ofs << images_rows << " " << images_cols << std::endl;
    ofs << 255;

    for(unsigned i = 0; i < md.image.size(); i++) {
        if(i%images_cols==0 && i < md.image.size()-1) ofs << std::endl;
        else ofs << " ";
        ofs << md.image[i];
    }
    ofs.close();
}

void MNISTReader::ToPPM(MNISTData & md) {
    std::ofstream ofs("image.ppm");
    ofs << "P2" << std::endl;
    ofs << images_rows << " " << images_cols << std::endl;
    ofs << 255;

    for(unsigned i = 0; i < md.image.size(); i++) {
        if(i%images_cols==0 && i < md.image.size()-1) ofs << std::endl;
        else ofs << " ";
        ofs << md.image[i];
    }
    ofs.close();
}

int32_t MNISTReader::reverseInt (int32_t i) 
{
    uint8_t c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int32_t)c1 << 24) + ((int32_t)c2 << 16) + ((int32_t)c3 << 8) + c4;
}

void MNISTReader::Shuffle() {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::shuffle(data.begin(), data.end(), rng);
}

void MNISTReader::PrintStats() {
    std::cout << "The next three values should be identical.\n";
    std::cout << "# Labels: " << labels_size << std::endl;
    std::cout << "# Images: " << images_size << std::endl;
    std::cout << "# Data:   " << data.size() << std::endl;

    std::map<unsigned,unsigned> count;
    
    for(sizet i = 0; i < data.size(); i++) {
        unsigned cur = 0;
        try {
            cur = count.at(data[i].label);
        } catch(std::out_of_range &e) {}
        count.insert_or_assign(data[i].label,cur+1);
    }


    unsigned total = 0;
    for(std::map<unsigned,unsigned>::iterator it = count.begin(); 
        it != count.end(); it++) {

        std::cout << it->first << ": " << it->second << std::endl;
        total+= it->second;    
    }
    std::cout << "TOTAL: " << total << std::endl;
    
    if(
        labels_size==images_size && 
        images_size==static_cast<i32>(data.size()) && 
        data.size()==total
    ) {
        std::cout << "Sizes match.\n";
    } else {
        std::cout << "Size mismatch.\n";
    }

}

void MNISTReader::NormalizeImage(double max_value) {
    for(std::size_t i = 0; i < data.size(); i++) {
        for(i32 k = 0; k < images_rows*images_cols; k++) {
            data[i].image[k] /= max_value;
        }
    }
}

MNISTData MNISTReader::FlipDataAt(unsigned i, MNISTDIR dir) {
    MNISTData d = GetDataAt(i);

    if(dir==MNISTDIR::HOR) {
        for(i32 x = 0; x < images_rows; x++) {
            for(i32 ys = 0, ye=images_cols-1; ys<ye ;ys++,ye--) {
                double temp = d.image[ys+x*images_cols];
                d.image[ys+x*images_cols]=d.image[ye+x*images_cols];
                d.image[ye+x*images_cols]=temp;
            }
        }
    } else if(dir==MNISTDIR::VER) {
        for(i32 y = 0; y < images_cols; y++) {
            for(i32 xs = 0, xe=images_rows-1; xs<xe ;xs++,xe--) {
                double temp = d.image[y+xs*images_cols];
                d.image[y+xs*images_cols]=d.image[y+xe*images_cols];
                d.image[y+xe*images_cols]=temp;
            }
        }
    } else {

    }

    return d;
}
#include"exp_beacon.hpp"

void ExpBeacon::Init(i64 seed) {
    rng = RNG(seed);
    LoadBeaconEntries();
}
void ExpBeacon::LoadBeaconEntries() {
    std::ifstream ifs("exp_beacon/iBeacon_RSSI_Labeled.csv");
    if(!ifs.good()) {
        zxlog::Error("Unable to load iBeacon labeled data.");
        return;
    }

    vec<int> num_heard_by_index = {
        0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    for(str line; std::getline(ifs, line); ) {
        std::istringstream iss;
        iss.str(line);
        vec<str> elements;
        for(str e; std::getline(iss, e, ','); ) {
            elements.push_back(e);
        }

        // This is the first line. Skip.
        if(elements[0]=="location") continue;

        ExpBeaconEntry entry;
        entry.beacons.resize(13);

        // Row and Col
        int col = elements[0][0]-65;    // Cap letter - Cap A (ascii conversion)
        int row = std::stoi(elements[0].substr(1,2))-1;

        entry.col[col] = BASE*CELL_MULT;
        entry.row[row] = BASE*CELL_MULT;

        // Skip date/time for now.

        // Beacon data
        entry.beacons[0] = (std::stoi(elements[2])+BASE)*BEAC_MULT;
        entry.beacons[1] = (std::stoi(elements[3])+BASE)*BEAC_MULT;
        entry.beacons[2] = (std::stoi(elements[4])+BASE)*BEAC_MULT;
        entry.beacons[3] = (std::stoi(elements[5])+BASE)*BEAC_MULT;
        entry.beacons[4] = (std::stoi(elements[6])+BASE)*BEAC_MULT;
        entry.beacons[5] = (std::stoi(elements[7])+BASE)*BEAC_MULT;
        entry.beacons[6] = (std::stoi(elements[8])+BASE)*BEAC_MULT;
        entry.beacons[7] = (std::stoi(elements[9])+BASE)*BEAC_MULT;
        entry.beacons[8] = (std::stoi(elements[10])+BASE)*BEAC_MULT;
        entry.beacons[9] = (std::stoi(elements[11])+BASE)*BEAC_MULT;
        entry.beacons[10] = (std::stoi(elements[12])+BASE)*BEAC_MULT;
        entry.beacons[11] = (std::stoi(elements[13])+BASE)*BEAC_MULT;
        entry.beacons[12] = (std::stoi(elements[14])+BASE)*BEAC_MULT;

        int num_heard = 0;
        for(sizet i = 0; i < 13; i++) {
            if(entry.beacons[i] > 0) {
                num_heard++;
            }
        }

        if(num_heard>1) {
            for(sizet i = 0; i < 13; i++) {
                if(entry.beacons[i] > 0) {
                    num_heard_by_index[i]++;
                }
            }
            
            labeled_entries.emplace_back(entry);
        }

    }
    ifs.close();

    ifs = std::ifstream("exp_beacon/iBeacon_RSSI_Unlabeled.csv");
    if(!ifs.good()) {
        zxlog::Error("Unable to load iBeacon unlabeled data.");
        return;
    }
    for(str line; std::getline(ifs, line); ) {
        std::istringstream iss;
        iss.str(line);
        vec<str> elements;
        for(str e; std::getline(iss, e, ','); ) {
            elements.push_back(e);
        }

        // This is the first line. Skip.
        if(elements[0]=="location") continue;

        ExpBeaconEntry entry;
        entry.beacons.resize(13);

        // Unlabeled entries just have ?. Skip.

        // Skip date/time for now.

        // Beacon data
        entry.beacons[0] = (std::stoi(elements[2])+BASE)*BEAC_MULT;
        entry.beacons[1] = (std::stoi(elements[3])+BASE)*BEAC_MULT;
        entry.beacons[2] = (std::stoi(elements[4])+BASE)*BEAC_MULT;
        entry.beacons[3] = (std::stoi(elements[5])+BASE)*BEAC_MULT;
        entry.beacons[4] = (std::stoi(elements[6])+BASE)*BEAC_MULT;
        entry.beacons[5] = (std::stoi(elements[7])+BASE)*BEAC_MULT;
        entry.beacons[6] = (std::stoi(elements[8])+BASE)*BEAC_MULT;
        entry.beacons[7] = (std::stoi(elements[9])+BASE)*BEAC_MULT;
        entry.beacons[8] = (std::stoi(elements[10])+BASE)*BEAC_MULT;
        entry.beacons[9] = (std::stoi(elements[11])+BASE)*BEAC_MULT;
        entry.beacons[10] = (std::stoi(elements[12])+BASE)*BEAC_MULT;
        entry.beacons[11] = (std::stoi(elements[13])+BASE)*BEAC_MULT;
        entry.beacons[12] = (std::stoi(elements[14])+BASE)*BEAC_MULT;

        int num_heard = 0;
        for(sizet i = 0; i < 13; i++) {
            if(entry.beacons[i] > 0) {
                num_heard++;
            }
        }

        if(num_heard>1) {
            for(sizet i = 0; i < 13; i++) {
                if(entry.beacons[i] > 0) {
                    num_heard_by_index[i]++;
                }
            }
            
            labeled_entries.emplace_back(entry);
        }
        

    }
    // std::cout << "NUM LABELED ENTRIES: " << labeled_entries.size() << std::endl;
    // for(int i = 0; i < 13; i++) {
    //     std::cout << "B" << i+1 << ": " << num_heard_by_index[i] << std::endl;
    // }
    ifs.close();
}

void ExpBeacon::ShuffleLabeledEntries() {
    std::shuffle(labeled_entries.begin(), labeled_entries.end(), rng);
}
void ExpBeacon::ShuffleUnlabeledEntries() {
    std::shuffle(unlabeled_entries.begin(), unlabeled_entries.end(), rng);
}
sizet ExpBeacon::GetSizeLabeledEntries() {
    return labeled_entries.size();
}
sizet ExpBeacon::GetSizeUnlabeledEntries() {
    return unlabeled_entries.size();
}
ExpBeaconEntry * ExpBeacon::GetLabeledEntry(sizet index) {
    return &labeled_entries[index];
}
ExpBeaconEntry * ExpBeacon::GetUnlabeledEntry(sizet index) {
    return &unlabeled_entries[index];
}
ExpBeaconEntry * ExpBeacon::GetRandomLabeledEntry() {
    std::uniform_int_distribution<sizet> dist(0,labeled_entries.size()-1);
    return &(labeled_entries[dist(rng)]);
}
ExpBeaconEntry * ExpBeacon::GetRandomUnlabeledEntry() {
    std::uniform_int_distribution<sizet> dist(0,unlabeled_entries.size()-1);
    return &(unlabeled_entries[dist(rng)]);
}
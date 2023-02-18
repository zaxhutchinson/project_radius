

#include"loader.hpp"

std::string SynData::to_string() {
    return std::to_string(id) + " " + std::to_string(lat) + " " + std::to_string(lon) + " " + std::to_string(rad) + " " + std::to_string(str) + "\n";
}


std::string NeuData::to_string() {
    std::string s(std::to_string(id) + "\n");
    for(umap<i64,SynData>::iterator it = syndata.begin(); it != syndata.end(); it++) {
        s += "      " + it->second.to_string();
    }
    return s;
}

std::string LayData::to_string() {
    std::string s(std::to_string(id) + "\n");
    for(umap<i64,NeuData>::iterator it = neudata.begin(); it != neudata.end(); it++) {
        s += "    " + it->second.to_string();
    }
    return s;
}

std::string NetData::to_string() {
    std::string s("NETWORK\n");
    for(umap<i64,LayData>::iterator it = laydata.begin(); it != laydata.end(); it++) {
        s += "  " + it->second.to_string();
    }
    return s;
}







// int main(int argc, char** argv) {
//     NetData nd = ProcessNetworkOutput(PATH);
//     return 0;
// }

NetData LoadNetworkOutput(std::string path) {
    NetData netdata;


    for(auto& file : std::filesystem::directory_iterator{path}) {
        str filename = file.path().filename();
        if(filename[0]!='S') {
            continue;
        }

        vec<str> fn_split = SplitString(filename,'_');

        i64 lid = std::stoull(fn_split[1]);
        i64 nid = std::stoull(fn_split[2]);
        i64 sid = std::stoull(fn_split[3]);

        if(lid!=LAYERID) continue;
        
        SynData sdata;
        sdata.id = sid;
        


        std::ifstream ifs{file.path()};
        str last_lon;
        str last_str;
        str last_rad;
        str last_lat;
        str last_pid;
        str line;
        while(std::getline(ifs,line) ){
            if(!line.empty()) {
                if(line.substr(0,3)=="STR") last_str = line;
                if(line.substr(0,3)=="LAT") last_lat = line;
                if(line.substr(0,3)=="LON") last_lon = line;
                if(line.substr(0,3)=="RAD") last_rad = line;
                if(line.substr(0,3)=="PID") last_pid = line;
            }
        }

        str slon = SplitString(last_lon,' ')[1];
        str slat = SplitString(last_lat,' ')[1];
        str srad = SplitString(last_rad,' ')[1];
        str sstr = SplitString(last_str,' ')[1];
        str spid = SplitString(last_pid,' ')[1];

        sdata.lat = std::stod(slat);
        sdata.lon = std::stod(slon);
        sdata.rad = std::stod(srad);
        sdata.str = std::stod(sstr);
        sdata.pid = std::stol(spid);

        umap<i64,LayData>::iterator lit = netdata.laydata.find(lid);

        if(lit == netdata.laydata.end()) {
            LayData ldata;
            NeuData ndata;
            ndata.id = nid;
            ndata.syndata.emplace(sid,sdata);
            ldata.neudata.emplace(nid,ndata);
            netdata.laydata.emplace(lid,ldata);
        } else {

            umap<i64,NeuData>::iterator nit = lit->second.neudata.find(nid);
            if(nit != lit->second.neudata.end()) {
                nit->second.syndata.emplace(sid,sdata);
            } else {
                NeuData ndata;
                ndata.id = nid;
                ndata.syndata.emplace(sid,sdata);
                lit->second.neudata.emplace(nid,ndata);
            }
        }
    }

    return netdata;
}

vec<str> SplitString(str line, char delim) {

    vec<str> v;

    str x;
    size_t p = 0;
    for(size_t i = 0; i < line.size(); i++) {
        if(line[i]==delim) {
            str chunk = line.substr(p,i-p);
            v.push_back(chunk);
            p=i+1;
        }
    }
    str chunk = line.substr(p,line.size()-p);
    v.push_back(chunk);

    return v;
}
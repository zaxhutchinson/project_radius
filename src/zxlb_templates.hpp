#pragma once

#include<cstdint>
#include<vector>
#include<queue>
#include<memory>
#include<optional>
#include<unordered_map>
#include<random>
#include<mutex>
#include<list>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using str = std::string;
using mtx = std::mutex;
using RNG = std::mt19937_64;

template<typename T>
using vec = std::vector<T>;
template<typename T>
using que = std::queue<T>;
template<typename T>
using lst = std::list<T>;
template<typename T, typename U>
using umap = std::unordered_map<T,U>;
template<typename T>
using opt = std::optional<T>;
template<typename T>
using uptr = std::unique_ptr<T>;
template<typename T>
using sptr = std::shared_ptr<T>;
template<typename T>
using vuptr = vec<uptr<T>>;
template<typename T>
using vsptr = vec<sptr<T>>;

template<typename T>
using quptr = que<uptr<T>>;

using UTRTN = std::pair<bool, str>;
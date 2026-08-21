#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstring>

struct binaryReader{
    std::ifstream file;
    size_t size;
    binaryReader(const std::filesystem::path& path){
        file.open(path, std::ios::binary | std::ios::ate);
        if(!file){throw std::runtime_error("Error opening binary file!");}
        size = file.tellg();
        file.seekg(0);
    } 

    void check()
    {
        if (!file){
            auto pos = file.tellg();
            throw std::runtime_error("Unexpected end of file at " + std::to_string(pos));
        }
            
    }
    void skip(size_t count){
        file.seekg(count, std::ios::cur);
        check();
    }

    void seek(size_t pos){
        file.seekg(pos);
        check();
    }
    size_t tell(){
        return file.tellg();
    }
    size_t remaining(){
        return size - tell();
    }

    uint8_t readU8(){
        uint8_t data;
        file.read(reinterpret_cast<char*>(&data), 1);
        check();
        return data;
    }
    uint32_t readU32LE(){
        uint8_t data[4];
        file.read(reinterpret_cast<char*>(&data), 4);
        check();
        return (uint32_t(data[3]) << 24 |
                uint32_t(data[2]) << 16 |
                uint32_t(data[1]) << 8 |
                uint32_t(data[0]));   
    }
    uint32_t readU32BE(){
        uint8_t data[4];
        file.read(reinterpret_cast<char*>(&data), 4);
        check();
        return (uint32_t(data[0]) << 24 |
                uint32_t(data[1]) << 16 |
                uint32_t(data[2]) << 8 |
                uint32_t(data[3]));   
    }
    uint16_t readU16LE(){
        uint8_t data[2];
        file.read(reinterpret_cast<char*>(&data), 2);
        check();
        return (uint32_t(data[1]) << 8 |
                uint32_t(data[0]));   
    }
    uint16_t readU16BE(){
        uint8_t data[2];
        file.read(reinterpret_cast<char*>(&data), 2);
        check();
        return (uint32_t(data[0]) << 8 |
                uint32_t(data[1]));   
    }
    float readFloat(){
        // assuemes LE
        uint8_t data[4];
        file.read(reinterpret_cast<char*>(&data), 4);
        check();
        uint32_t bits = (uint32_t(data[3]) << 24 |
                uint32_t(data[2]) << 16 |
                uint32_t(data[1]) << 8 |
                uint32_t(data[0])); 
        float val;
        std::memcpy(&val, &bits, sizeof(float));
        return val;
    }
    double readDouble(){
        // assuemes LE
        uint8_t data[8];
        file.read(reinterpret_cast<char*>(&data), 8);
        check();
        uint64_t bits = (uint64_t(data[7]) << 56 |
                uint64_t(data[6]) << 48 |
                uint64_t(data[5]) << 40 |
                uint64_t(data[4]) << 32 |
                uint64_t(data[3]) << 24 |
                uint64_t(data[2]) << 16 |
                uint64_t(data[1]) << 8 |
                uint64_t(data[0])); 
        double val;
        std::memcpy(&val, &bits, sizeof(double));
        return val;
    }
    std::vector<std::byte> readBytes(size_t count){
        std::vector<std::byte> data(count);
        file.read(reinterpret_cast<char*>(data.data()), count);
        check();
        return data;
    }
    void readBytes(void* buf, size_t count){
        file.read(reinterpret_cast<char*>(buf), count);
        check();
    }
    std::string readString(size_t length)
    {   
        std::string str(length, '\0');
        file.read(str.data(), length);
        check();
        return str;
    }
};
#pragma once
#include "../utils/units.h"
#include "../debugger/Debugger.h"
namespace Iris {
    namespace Serializer {
        enum class MetaType {
            I8,
            U8,
            I16,
            U16,
            I32,
            U32,
            I64,
            U64,
            F32,
            F64,
            STRING,
            COUNT
        };
        size_t metaTypeSize[(size_t)MetaType::COUNT] = { sizeof(i8), sizeof(u8), sizeof(i16), sizeof(u16), sizeof(i32), sizeof(u32), sizeof(i64), sizeof(u64), sizeof(f32), sizeof(f64), sizeof(char*) };
        struct MetaInfo {
            MetaType* types;
            size_t size;
        };
        struct MetaData {
            char* data;
            size_t size;
        };
        std::unordered_map<const char*, MetaInfo> s_metaInfo;
        std::unordered_map<std::string, MetaData> s_metaData;
        template <typename T, typename... Args>
        void registerType(Args... args) {
            if (s_metaInfo.contains(typeid(T).name())) {
                WARN("Type '{}' is already registered!", typeid(T).name());
                return;
            }
            for (const auto type : { args... }) {
                if (typeid(type) != typeid(MetaType)) {
                    ERROR("Failed to register '{}' type! Invalid argument!", typeid(T).name());
                    return;
                }
            }
            std::vector<MetaType> temp = { args... };
            MetaType* types = new MetaType[temp.size()];
            memcpy(types, temp.data(), temp.size() * sizeof(MetaType));
            s_metaInfo.emplace(typeid(T).name(), MetaInfo{ types, temp.size() });
            INFO("Successfully registered '{}' type", typeid(T).name());
        }
        template <typename T>
        MetaData createMetaData(T val, const char* name) {
            const char* typeName = typeid(T).name();
            size_t typeLength = strlen(typeName) + 1; // +1 to include the \0 char
            if (!s_metaInfo.contains(typeName)) {
                ERROR("Type '{}' isn't registered!", typeName);
                return {};
            }
            const char* variableName = name;
            size_t variableLength = strlen(variableName) + 1;
            MetaInfo& info = s_metaInfo.at(typeName);
            size_t dataOffset = 0;
            size_t* offsets = new size_t[info.size];
            for (size_t i = 0; i < info.size; i++) {
                size_t byteSize = fmax(metaTypeSize[(size_t)info.types[i]], 2);
                if (i < info.size - 1) {
                    size_t spaceLeft = (dataOffset + byteSize) % 4;
                    size_t nextSize = fmax(metaTypeSize[(size_t)info.types[i + 1]], 2);
                    if (nextSize > spaceLeft) {
                        byteSize += spaceLeft;
                    }
                }
                offsets[i] = dataOffset;
                dataOffset += byteSize;
            }
            size_t outputSize = typeLength + variableLength + info.size + sizeof(outputSize);
            for (size_t i = 0; i < info.size; i++) {
                if (info.types[i] == MetaType::STRING) {
                    char* ptr = (char*)(&val) + offsets[i];
                    char* str = (char*)*(size_t*)ptr;
                    outputSize += strlen(str) + 1;
                }
                else {
                    size_t byteSize = metaTypeSize[(size_t)info.types[i]];
                    outputSize += byteSize;
                }
            }
            char* data = new char[outputSize];
            std::string fullName = std::string(typeName) + " " + std::string(variableName);
            size_t index = typeLength + variableLength + sizeof(outputSize);
            memcpy(data, &outputSize, sizeof(outputSize));
            memcpy(data + sizeof(outputSize), fullName.c_str(), fullName.length() + 1);
            for (size_t i = 0; i < info.size; i++) {
                data[index++] = (char)info.types[i];
                if (info.types[i] == MetaType::STRING) {
                    char* ptr = (char*)(&val) + offsets[i];
                    char* str = (char*)*(size_t*)ptr;
                    memcpy(data + index, str, strlen(str) + 1);
                    index += strlen(str) + 1;
                }
                else {
                    size_t byteSize = metaTypeSize[(size_t)info.types[i]];
                    memcpy(data + index, ((char*)&val) + offsets[i], byteSize);
                    index += byteSize;
                }
            }
            if (s_metaData.contains(fullName))
                s_metaData.at(fullName) = MetaData{ data, outputSize };
            else
                s_metaData.emplace(fullName, MetaData{ data, outputSize });
            INFO("Created data for '{}' of type '{}'!", name, typeName);
            return s_metaData.at(fullName);
        }
        void saveMetaData(const char* output) {
            FILE* f;
            fopen_s(&f, output, "wb");
            if (f == nullptr) {
                ERROR("Failed to save data!");
                return;
            }
            size_t amount = s_metaData.size();
            for (size_t i = 0; i < sizeof(size_t); i++) {
                fprintf(f, "%c", ((char*)&amount)[i]);
            }
            for (auto data : s_metaData) {
                for (size_t i = 0; i < data.second.size; i++) {
                    fprintf(f, "%c", data.second.data[i]);
                }
            }
            INFO("Saved data to {}!", output);
            fclose(f);
        }
        void loadMetaData(const char* input) {
            FILE* f;
            fopen_s(&f, input, "rb");
            if (f == nullptr) {
                ERROR("File '{}' doesn't exist!", input);
                return;
            }
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            fseek(f, 0, SEEK_SET);
            char* buf = new char[size];
            fread_s(buf, size, size, 1, f);
            size_t amount = *(size_t*)(buf);
            size_t index = sizeof(size_t);
            for (size_t i = 0; i < amount; i++) {
                size_t size = *(size_t*)(buf + index);
                std::string fullName = std::string((const char*)(buf + index + sizeof(size_t)));
                char* data = new char[size];
                memcpy(data, &buf[index], size);
                if (s_metaData.contains(fullName)) {
                    WARN("File contains multiple datas with same identificator ('{}')!", fullName);
                }
                s_metaData.emplace(fullName, MetaData{ data, size });
                index += size;
            }
            fclose(f);
            INFO("Successfully loaded saved data!");
        }
        template <typename T>
        void loadFromMetaData(T* val, const char* name) {
            const char* typeName = typeid(T).name();
            std::string fullName = std::string(typeName) + " " + std::string(name);
            if (!s_metaData.contains(fullName)) {
                ERROR("File doesn't contain data for {} variable!", name);
                return;
            }
            MetaData& data = s_metaData.at(fullName);
            MetaInfo& info = s_metaInfo.at(typeName);
            size_t dataOffset = 0;
            size_t* sizes = new size_t[info.size];
            for (size_t i = 0; i < info.size; i++) {
                size_t byteSize = fmax(metaTypeSize[(size_t)info.types[i]], 2);
                if (i < info.size - 1) {
                    size_t spaceLeft = (dataOffset + byteSize) % 4;
                    size_t nextSize = fmax(metaTypeSize[(size_t)info.types[i + 1]], 2);
                    if (nextSize > spaceLeft) {
                        byteSize += spaceLeft;
                    }
                }
                sizes[i] = byteSize;
                dataOffset += byteSize;
            }
            char* buf = new char[sizeof(T)];
            memset(buf, 0, sizeof(T));
            size_t index = 0;
            size_t element = 0;
            for (size_t i = sizeof(size_t) + fullName.length() + 1; i < data.size; i++) {
                MetaType type = (MetaType)data.data[i];
                size_t byteSize = metaTypeSize[(size_t)type];
                i64 padding = sizes[element] - byteSize;
                if (type == MetaType::STRING) {
                    char* ptr = (char*)(data.data + i + 1);
                    memcpy(buf + index, &ptr, sizeof(char*));
                    index += byteSize;
                    byteSize = strlen(data.data + i);
                }
                else {
                    memcpy(buf + index, data.data + i + 1, byteSize);
                    if (padding > 0) index += padding;
                    index += byteSize;
                }
                element++;
                i += byteSize;
            }
            memcpy(val, buf, sizeof(T));
            INFO("Loaded saved data into {}!", name);
        }
    }
}
#pragma once

#include "core/hash/FnvHash.h"

#include <cstdlib>
#include <cstdint>
#include <functional>

class VersionedSerializedReader {
public:
    typedef std::function<void(void *, size_t)> Reader;

    VersionedSerializedReader(Reader reader, uint32_t readerVersion) :
        _reader(reader),
        _readerVersion(readerVersion)
    {
        _reader(&_dataVersion, sizeof(_dataVersion));
    }

    uint32_t readerVersion() const { return _readerVersion; }
    uint32_t dataVersion() const { return _dataVersion; }

    template<typename T>
    void read(T &value, uint32_t addedInVersion = 0) {
        read(&value, sizeof(value), addedInVersion);
    }

    template<typename ReadT, typename T>
    void readAs(T &value, uint32_t addedInVersion = 0) {
        ReadT tmp;
        read(tmp, addedInVersion);
        value = tmp;
    }

    void read(void *data, size_t len, uint32_t addedInVersion) {
        if (_dataVersion >= addedInVersion) {
            _reader(data, len);
            _hash(data, len);
        }
    }

    template<typename T>
    void skip(uint32_t addedInVersion, uint32_t removedInVersion) {
        skip(sizeof(T), addedInVersion, removedInVersion);
    }

    void skip(size_t len, uint32_t addedInVersion, uint32_t removedInVersion) {
        if (_dataVersion >= addedInVersion && _dataVersion < removedInVersion) {
            uint8_t dummy[len];
            _reader(dummy, len);
            _hash(dummy, len);
        }
    }

    bool checkHash() {
        uint32_t hash;
        _reader(&hash, sizeof(hash));
        return _hash.result() == hash;
    }

private:
    Reader _reader;
    uint32_t _readerVersion;
    uint32_t _dataVersion;
    FnvHash _hash;
};

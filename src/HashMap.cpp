//
// Created by Connor Ragland on 4/8/24.
//
#include "HashMap.h"


int HashMap::hash(const string& key) {
    std::hash<string> hash;
    size_t hashValue = hash(key);
    size_t reducedHashValue = hashValue % _maxCapacity;

    return (int)reducedHashValue;  // Reduce and return
}


void HashMap::updateLoadFactor() {
    _loadFactor = (float)_currentCapacity / (float)_maxCapacity;

    // Re-hash
    if (_loadFactor > MAX_LOAD_FACTOR) {
        int tempCapacity = _maxCapacity;  // Store old capacity
        _maxCapacity *= 2;

        vector<vector<pair<string, AttributeData>>> newHashMap;
        newHashMap.resize(_maxCapacity);

        for (int i = 0; i < tempCapacity; i++) {
            for (auto pair : hashMap[i]) {
                // Push each pair into their respective row
                int idx = hash(pair.first);
                newHashMap[idx].push_back(pair);
            }
        }

        // Update load factor
        _loadFactor = (float)_currentCapacity / (float)_maxCapacity;

        hashMap.clear();
        hashMap.resize(_maxCapacity);
        hashMap = std::move(newHashMap);  // std::move to avoid copies
        // Reference: https://stackoverflow.com/questions/3413470/what-is-stdmove-and-when-should-it-be-used
    }
}


// Initializer
HashMap::HashMap() {
    hashMap.resize(INITIAL_CAPACITY);
    _maxCapacity = INITIAL_CAPACITY;
    _currentCapacity = 0;
    _loadFactor = 0;
    _maxCrashes = 0;
}


void HashMap::insert(string key, const DataNode& dataObject) {
    int hashKey = hash(key);

    if (!hashMap[hashKey].empty()) {
        // If the key is already present in the hash map
        for (auto &item : hashMap[hashKey]) {
            if (item.first == key) {
                item.second.numCrashes += 1;
                item.second.totalSeverity += dataObject.severity;
                int numCrashes = item.second.numCrashes;
                int totalSeverity = item.second.totalSeverity;
                item.second.averageSeverity = (double)totalSeverity / (double)numCrashes;
                item.second.numCrashes > _maxCrashes ? _maxCrashes = item.second.numCrashes : _maxCrashes;

                return;
            }
        }
    }

    // Existing object not found
    AttributeData attributeObject;
    attributeObject.attributeName = key;
    attributeObject.numCrashes = 1;
    attributeObject.totalSeverity = dataObject.severity;
    attributeObject.averageSeverity = (float)dataObject.severity;
    pair<string, AttributeData> pair = make_pair(key, attributeObject);

    hashMap[hashKey].push_back(pair);

    // Update the load factor only if a new object is being added
    _currentCapacity++;
    updateLoadFactor();
}


void HashMap::find(string key) {
    int hashKey = hash(key);

    // Iterate through all until found, then print
    for (int i = 0; i < hashMap[hashKey].size(); i++) {
        if (hashMap[hashKey][i].first == key) {
            AttributeData attribute = hashMap[hashKey][i].second;
            cout << key << ':' << endl;
            cout << attribute.numCrashes << " total crashes" << endl;
            cout << attribute.averageSeverity << " average severity" << endl;
            cout << endl;

            return;
        }
    }

    cout << "Not found\n\n";
}


vector<pair<string, AttributeData>> HashMap::getTopK(int k) {
    vector<vector<pair<string, AttributeData>>> buckets(_maxCrashes+1);
    for (auto &bucket : hashMap) {
        for (auto &pair : bucket) {
            buckets.at(pair.second.numCrashes).push_back(pair);
        }
    }

    vector<pair<string, AttributeData>> dataVector;

    int i = _maxCrashes;
    while (dataVector.size() < k && i >= 0) {
        for (auto &pair : buckets.at(i)) {
            dataVector.push_back(pair);
            if (dataVector.size() >= k) {
                break;
            }
        }
        i--;
    }

    return dataVector;
}


void HashMap::transferToHeap(Heap<less<int>> &heap) {
    for (auto &bucket : hashMap) {
        for (auto &pair : bucket) {
            heap.push(pair.second);
        }
    }
}


// Debugging functions
// -------------------
//
//void HashMap::printAll() {
//    for (auto row : hashMap) {
//        for (auto col : row) {
//            cout << col.first << ": " << endl;
//            cout << "Num crashes: " << col.second.numCrashes << endl;
//            cout << "Average severity: " << col.second.averageSeverity << endl;
//            cout << endl;
//        }
//    }
//}
//
//
//void HashMap::getBucketCount() {
//    cout << this->_currentCapacity << endl;
//}
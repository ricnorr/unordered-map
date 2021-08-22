#pragma once
#include <vector>
#include <list>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
  friend class ConstIterator;
 public:
  explicit HashMap(Hash hash = std::hash<KeyType>()) : hash(hash) {}
  template<class Iterator>
  HashMap(Iterator begin, Iterator end, Hash hash = std::hash<KeyType>()) {
    this->hash = hash;
    while (begin != end) {
      insert(*(begin++));
    }
  }
  HashMap(std::initializer_list<std::pair<KeyType, ValueType>> initializer_list) : HashMap(initializer_list.begin(),
                                                                                           initializer_list.end()) {};

  void Insert(const std::pair<KeyType, ValueType> &pair) {
    auto &bucket = data[hash(pair.first) % bucket_cnt];
    const auto &it = find_element_by_key(pair.first);
    if (it == bucket.end()) {
      data[hash(pair.first) % bucket_cnt].push_back(pair);
      size++;
    }
  }

  size_t Size() const {
    return size;
  }

  bool Empty() const {
    return size == 0;
  }

  Hash &hash_function() const {
    return hash;
  }

  void Erase(const KeyType &key) {
    auto &bucket = data[hash(key) % bucket_cnt];
    auto &it = find_element_by_key();
    if (it != bucket.end()) {
      bucket.erase(it);
      size--;
    }
  }

 private:
  const static size_t bucket_cnt = 1000;
  std::vector<std::list<std::pair<KeyType, ValueType>>>
      data = std::vector(bucket_cnt, std::list<std::pair<KeyType, ValueType>>(0));
  Hash hash;
  size_t size = 0;

  typename std::list<std::pair<KeyType, ValueType>>::iterator find_element_iterator_in_bucket(const KeyType &key) {
    auto &bucket = data[hash(key) % bucket_cnt];
    for (auto &it = bucket.begin(); it != bucket.end(); it++) {
      if (it.first == key) {
        return it;
      }
    }
    return bucket.end();
  }



  class ConstIterator {

    ConstIterator(const ConstIterator &that,
                  std::vector<std::list<std::pair<const int, int>>>::const_iterator iterator,
                  std::_List_const_iterator<std::pair<const int, int>> const_iterator) {
      map = that.map;
      bucket_it = that.bucket_it;
      vector_it = that.vector_it;
    }

    ConstIterator &operator++() {
      if (bucket_it == (*vector_it).end()) {
        vector_it++;
        if (vector_it != map.data.end()) {
          bucket_it = (*vector_it).begin();
        }
      } else {
        bucket_it++;
      }
      return *this;
    }

    ConstIterator operator++(int) {
      auto ans = *this;
      if (bucket_it == (*vector_it).end()) {
        vector_it++;
        if (vector_it != map.data.end()) {
          bucket_it = (*vector_it).begin();
        }
      } else {
        bucket_it++;
      }
      return ans;
    }

    bool operator==(const ConstIterator &that) {
      return (this->vector_it == that.vector_it && this->bucket_it == that.bucket_it);
    }

    bool operator!=(const ConstIterator &that) {
      return !(*this == that);
    }

    const std::pair<const KeyType, const ValueType> *operator->() {
      return &(*bucket_it);
    }

    const std::pair<const KeyType, const ValueType> &operator*() {
      return *bucket_it;
    }

    HashMap<KeyType, ValueType, Hash> &map;
    typename std::vector<std::list<std::pair<KeyType, ValueType>>>::const_iterator vector_it;
    typename std::list<std::pair<KeyType, ValueType>>::const_iterator bucket_it;
  };

  class Iterator {

  };

};
#include <iostream>
//#include "unordered_map.h"
#pragma once
#include <vector>
#include <list>
#include <cassert>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 private:
  class ConstIterator;
  class Iterator;
 public:
  explicit HashMap(Hash hash = std::hash<KeyType>()) : hash(hash) {}
  template<class Iterator>
  HashMap(Iterator begin, Iterator end, Hash hash = std::hash<KeyType>()) {
    this->hash = hash;
    while (begin != end) {
      Insert(*(begin++));
    }
  }
  HashMap(std::initializer_list<std::pair<KeyType, ValueType>> initializer_list) : HashMap(initializer_list.begin(),
                                                                                           initializer_list.end()) {};

  void Insert(const std::pair<KeyType, ValueType> &pair) {
    auto &bucket = find_bucket_reference(pair.first);
    auto it = find_element_iterator_in_bucket(pair.first);
    if (it == bucket.end()) {
      find_bucket_reference(pair.first).push_back(pair);
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
    auto &bucket = find_bucket_reference(key);
    auto it = find_element_iterator_in_bucket(key);
    if (it != bucket.end()) {
      bucket.erase(it);
      size--;
    }
  }

  Iterator End() {
    return Iterator(*this, data.end(), data[bucket_cnt - 1].end());
  }

  Iterator Find(const KeyType &key) {
    auto bucket = find_bucket_iterator(key);
    auto it = find_element_iterator_in_bucket(key);
    if (it == bucket->end()) {
      return End();
    } else {
      return Iterator(*this, bucket, it);
    }
  }

  ConstIterator End() const {
    return ConstIterator(*this, data.end(), data[bucket_cnt - 1].end());
  }

  ConstIterator Find(const KeyType &key) const {
    auto bucket = find_bucket_iterator(key);
    auto it = find_element_iterator_in_bucket(key);
    if (it == bucket->end()) {
      return End();
    } else {
      return ConstIterator(*this, bucket, it);
    }
  }

  Iterator Begin() {
    for (auto vector_it = data.begin(); vector_it != data.end(); vector_it++) {
      for (auto bucket_it = (*vector_it).begin(); bucket_it != (*vector_it).end(); bucket_it++) {
        return Iterator(*this, vector_it, bucket_it);
      }
    }
    return Iterator(*this, data.end(), data[0].end());
  }

  ConstIterator Begin() const {
    for (auto vector_it = data.begin(); vector_it != data.end(); vector_it++) {
      for (auto bucket_it = (*vector_it).begin(); bucket_it != (*vector_it).end(); bucket_it++) {
        return ConstIterator(*this, vector_it, bucket_it);
      }
    }
    return ConstIterator(*this, data.end(), data[0].end());
  }

  std::pair<const KeyType, ValueType> &operator[](const KeyType &key) {
    auto it = Find(key);
    if (it == End()) {
      return Insert(key, ValueType());
    } else {
      return *it;
    }
  };

  const std::pair<const KeyType, const ValueType> &operator[](const KeyType &key) const {
    auto it = Find(key);
    if (it == End()) {
      return Insert(key, ValueType());
    } else {
      return *it;
    }
  };

  const ValueType &At(const KeyType &key) const {
    auto it = Find(key);
    if (it == End()) {
      throw std::out_of_range("such key doesn't exist");
    } else {
      return it->second;
    }
  };

  void Clear() {
    data = std::vector(bucket_cnt, std::list<std::pair<const KeyType, ValueType>>(0));
    size = 0;
  };
 public:
  const static size_t bucket_cnt = 1000;
  std::vector<std::list<std::pair<const KeyType, ValueType>>>
      data = std::vector(bucket_cnt, std::list<std::pair<const KeyType, ValueType>>(0));
  Hash hash;
  size_t size = 0;

  typename std::list<std::pair<const KeyType,
                               ValueType>>::iterator find_element_iterator_in_bucket(const KeyType &key) {
    auto &bucket = data[hash(key) % bucket_cnt];
    for (auto it = bucket.begin(); it != bucket.end(); it++) {
      if (it->first == key) {
        return it;
      }
    }
    return bucket.end();
  }

  typename std::list<std::pair<const KeyType,
                               ValueType>>::const_iterator find_element_iterator_in_bucket(const KeyType &key) const {
    auto &bucket = data[hash(key) % bucket_cnt];
    for (auto it = bucket.begin(); it != bucket.end(); it++) {
      if (it->first == key) {
        return it;
      }
    }
    return bucket.end();
  }

  typename std::vector<typename std::list<std::pair<const KeyType,
                                                    ValueType>>>::iterator find_bucket_iterator(const KeyType &key) {
    return data.begin() + (hash(key) % bucket_cnt);
  }

  typename std::vector<typename std::list<std::pair<const KeyType, ValueType>>>::const_iterator find_bucket_iterator(
      const KeyType &key) const {
    return data.begin() + (hash(key) % bucket_cnt);
  }

  const typename std::list<std::pair<const KeyType, ValueType>> &find_bucket_reference(const KeyType &key) const {
    return *(data.begin() + (hash(key) % bucket_cnt));
  }

  typename std::list<std::pair<const KeyType, ValueType>> &find_bucket_reference(const KeyType &key) {
    return *(data.begin() + (hash(key) % bucket_cnt));
  }

  template<class VectorIterator, class BucketIterator, class HashMapType>
  class AbstractIterator {
   public:
    AbstractIterator(HashMapType &map,
                     const VectorIterator &vector_it,
                     const BucketIterator &bucket_it)
        : map(map), vector_it(vector_it), bucket_it(bucket_it) {}

    AbstractIterator &preInc() {
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

    void Inc() {
      bucket_it++;
      if (bucket_it == (*vector_it).end()) {
        for (++vector_it; vector_it != map.data.end() && (*vector_it).size() == 0; vector_it++);
        if (vector_it == map.data.end()) {
          bucket_it = map.data[bucket_cnt - 1].end();
        } else {
          bucket_it = vector_it->begin();
        }
      }
    }

    bool equal(const AbstractIterator &that) const {
      return (this->vector_it == that.vector_it && this->bucket_it == that.bucket_it);
    }

    HashMapType &map;
    VectorIterator vector_it;
    BucketIterator bucket_it;
  };
 private:
  class ConstIterator :
      public AbstractIterator<
          typename std::vector<typename std::list<std::pair<const KeyType, ValueType>>>::const_iterator,
          typename std::list<typename std::pair<const KeyType, ValueType>>::const_iterator,
          const HashMap<KeyType, ValueType, Hash>
      > {

    using VectorIterator = typename std::vector<typename std::list<std::pair<const KeyType,
                                                                             ValueType>>>::const_iterator;
    using BucketIterator = typename std::list<typename std::pair<const KeyType, ValueType>>::const_iterator;
    using HashMapType = const HashMap<KeyType, ValueType, Hash>;

   public:
    ConstIterator(const ConstIterator &that) : AbstractIterator<VectorIterator, BucketIterator, HashMapType>(that) {}

    ConstIterator(const HashMap<KeyType, ValueType, Hash> &map,
                  VectorIterator vector_iterator,
                  BucketIterator bucket_iterator
    ) : AbstractIterator<VectorIterator, BucketIterator, HashMapType>(map, vector_iterator, bucket_iterator) {}

    ConstIterator &operator++() {
      return this->preInc();
    }

    ConstIterator operator++(int) {
      auto ans = *this;
      this->Inc();
      return ans;
    }

    bool operator==(const ConstIterator &that) const {
      return this->equal(that);
    }

    bool operator!=(const ConstIterator &that) const {
      return !this->equal(that);
    }

    const std::pair<const KeyType, const ValueType> *operator->() {
      return reinterpret_cast<const std::pair<const KeyType, const ValueType> *>(&(*this->bucket_it));
    }

    const std::pair<const KeyType, const ValueType> &operator*() {
      return *(this->bucket_it);
    }
  };

  class Iterator : public AbstractIterator<
      typename std::vector<typename std::list<std::pair<const KeyType, ValueType>>>::iterator,
      typename std::list<typename std::pair<const KeyType, ValueType>>::iterator,
      HashMap<KeyType, ValueType, Hash>> {

    using VectorIterator = typename std::vector<typename std::list<std::pair<const KeyType, ValueType>>>::iterator;
    using BucketIterator = typename std::list<typename std::pair<const KeyType, ValueType>>::iterator;
    using HashMapType = HashMap<KeyType, ValueType, Hash>;
   public:

    Iterator(const Iterator &that) : AbstractIterator<VectorIterator, BucketIterator, HashMapType>(that) {}

    Iterator(HashMap<KeyType, ValueType> &map, VectorIterator vector_iterator, BucketIterator bucket_iterator)
        : AbstractIterator<VectorIterator, BucketIterator, HashMapType>(map, vector_iterator, bucket_iterator) {}

    Iterator &operator++() {
      return preInc(*this);
    }

    Iterator operator++(int) {
      return postInc(*this);
    }

    bool operator==(const Iterator &that) {
      return this->equal(that);
    }

    bool operator!=(const Iterator &that) {
      return !this->equal(that);
    }

    std::pair<const KeyType, ValueType> *operator->() {
      return &(*this->bucket_it);
    }

    std::pair<const KeyType, ValueType> &operator*() {
      return *this->bucket_it;
    }
  };

};

void test1() {
  HashMap<int, int> map = {{0, 0}};
  auto pair1 = std::pair<int, int>(1, 1);
  auto pair2 = std::pair<int, int>(2, 2);
  auto pair3 = std::pair<int, int>(3, 3);
  map.Insert(pair1);
  map.Insert(pair2);
  map.Insert(pair3);
  std::cout << map.Size();
  map.Clear();
  std::cout << map.Size();
  assert(map.Find(1) == map.End());
}

void test2() {
  const HashMap<int, int> map = {{0, 0}, {999, 999}};
  for (auto it = map.Begin(); it != map.End(); it++) {
    std::cout << it->second << std::endl;
  }
}

int main() {
  test2();

}

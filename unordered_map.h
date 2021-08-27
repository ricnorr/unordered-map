#pragma once
#include <iostream>
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
      Insert(std::pair<KeyType, ValueType>(begin->first, begin->second));
      begin++;
    }
  }
  HashMap(std::initializer_list<std::pair<KeyType, ValueType>> initializer_list) : HashMap(initializer_list.begin(),
                                                                                           initializer_list.end()) {};
  /**
   * Inserts value, or overwrite, if key exists
   * Makes copy of pair!
   */
  void Insert(const std::pair<KeyType, ValueType> &pair) {
    auto bucket = find_bucket_iterator(pair.first);
    auto it = find_element_iterator_in_bucket(pair.first);
    if (it == bucket->end()) {
      find_bucket_iterator(pair.first)->push_back(pair);
      size++;
    } else {
      it->second = pair.second;
    }
  }

  size_t Size() const {
    return size;
  }

  bool Empty() const {
    return size == 0;
  }

  const Hash &hash_function() const {
    return hash;
  }

  /**
   * Do nothing, if key does not exist, or erase value
   */
  void Erase(const KeyType &key) {
    auto bucket = find_bucket_iterator(key);
    auto it = find_element_iterator_in_bucket(key);
    if (it != bucket->end()) {
      bucket->erase(it);
      size--;
    }
  }

  /**
   * End of map
   * @return
   */
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
    return End();
  }

  ConstIterator Begin() const {
    for (auto vector_it = data.begin(); vector_it != data.end(); vector_it++) {
      for (auto bucket_it = vector_it->begin(); bucket_it != vector_it->end(); bucket_it++) {
        return ConstIterator(*this, vector_it, bucket_it);
      }
    }
    return End();
  }

  /**
   * Returns found pair, or create it and return created
   */
  ValueType &operator[](const KeyType &key) {
    auto it = Find(key);
    if (it == End()) {
      Insert({key, ValueType()});
    }
    return Find(key)->second;
  };

  /**
   * analogue of [] for const map
   */
  const ValueType &At(const KeyType &key) const {
    auto it = Find(key);
    if (it == End()) {
      throw std::out_of_range("such key doesn't exist");
    } else {
      return it->second;
    }
  };

  /**
   * Clears map
   */
  void Clear() {
    data = std::vector(bucket_cnt, std::list<std::pair<const KeyType, ValueType>>(0));
    size = 0;
  };
 private:
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

  template<class VectorIterator, class BucketIterator, class HashMapType>
  class AbstractIterator {
   public:
    AbstractIterator(HashMapType &map,
                     const VectorIterator &vector_it,
                     const BucketIterator &bucket_it)
        : map(map), vector_it(vector_it), bucket_it(bucket_it) {}

    void Inc() {
      bucket_it++;
      if (bucket_it == (*vector_it).end()) {
        for (vector_it++; vector_it != map.data.end() && vector_it->size() == 0; vector_it++);
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
      this->Inc();
      return *this;
    }

    const ConstIterator operator++(int) {
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

    const std::pair<const KeyType, ValueType> *operator->() {
      return &(*this->bucket_it);
    }

    const std::pair<const KeyType, ValueType> &operator*() {
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
      this->Inc();
      return *this;
    }

    const Iterator operator++(int) {
      auto ans = *this;
      this->Inc();
      return ans;
    }

    bool operator==(const Iterator &that) const {
      return this->equal(that);
    }

    bool operator!=(const Iterator &that) const {
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


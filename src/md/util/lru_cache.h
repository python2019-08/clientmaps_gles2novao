#ifndef LRU_CACHE_HPP_INCLUDED
#define	LRU_CACHE_HPP_INCLUDED

#include <map>
#include <list>
#include <cstddef>
#include <stdexcept>
#include <functional>

namespace clientmap {
    
    template<typename KEY, 
            typename VALUE, 
            typename _Compare = std::less<KEY>
    >
    class LruCache {
    public:
        using TCacheListItem = typename std::pair<KEY, VALUE> ; 
        using CacheList = std::list<TCacheListItem>;
        using CacheMap = std::map<KEY, typename CacheList::iterator, _Compare>; 
        //using CacheMap = std::unordered_map<KEY, typename CacheList::iterator>;

    public:
        LruCache() = default;
        
        LruCache(size_t max_size) {
            m_maxSize = max_size;
        }
        
    public:
        void put(const KEY& key, const VALUE& value) 
        {
            if (m_maxSize <= 0){ 
                return;
            }

            m_list.push_front(TCacheListItem(key, value));
            
            auto it = m_map.find(key);
            if (it != m_map.end()) {
                m_list.erase(it->second);
                m_map.erase(it);
            }
            m_map[key] = m_list.begin();
            
            if (m_map.size() > m_maxSize) {
                auto last = --m_list.end();
                m_map.erase(last->first);
                m_list.pop_back();
            }
        }
        
        VALUE get(const KEY& key) 
        {
            if (m_maxSize <= 0) {
                return VALUE();
            }

            auto it = m_map.find(key);
            if (it == m_map.end()) {
                return VALUE();    // return default value
            } else {
                m_list.splice(m_list.begin(), m_list, it->second);
                return it->second->second;
            }
        }
        
        void setMaxSize(size_t size) {
            if (size >= m_maxSize) {
                m_maxSize = size;
                return;
            }
            
            auto diff = m_maxSize - size;
            for (auto i=0; i<diff; ++i) {
                auto last = --m_list.end();
                m_map.erase(last->first);
                m_list.pop_back();
            }
            
            m_maxSize = size;
        }
        
        bool contains(const KEY& key) const {
            return m_map.find(key) != m_map.end();
        }
        
        size_t size() const {
            return m_map.size();
        }
        
        void for_each(std::function<void (const TCacheListItem &)> f) {
            for (auto &item : m_list) {
                f(item);
            }
        }
        
        void clear() {
            m_map.clear();
            m_list.clear();
        }
        
    private:
        CacheList m_list;
        CacheMap  m_map;        
        size_t m_maxSize = 256;
    };
    
} // namespace framework

#endif	/* LRU_CACHE_HPP_INCLUDED */

#pragma once

namespace caches
{
template <typename T>
struct MinHeap {
    static int parent(int x)
    {
        // return parent index
        return (x - 1) / 2;
    }

    static int child(int x)
    {
        // return child index
        return (2 * x + 1);
    }

    static void swap(T &a, T &b)
    {
        // swap two value (call by ref)
        T temp = a;
        a = b;
        b = temp;
    }

    static void percolate_up(T *arr, int index)
    {
        // percolate_up algorithm -  It is an algorithm that sorts according to minheap rules
        // from index to index 0. Starting comparision between parent and child value from input index, if the
        // value at child index is smaller than his parent value, the swap is excuted because it violates
        // minheap rules. This is repeated until index is replaced with parent index and root (index=0). If
        // the minheap rule is followed during iteration(the child's value is lager than the parent's value),
        // the loop will escape and be terminated early.
        while (index > 0) {
            int par_index = parent(index);
            if (arr[index] >= arr[par_index]) {
                return;
            }

            else {
                swap(arr[index], arr[par_index]);
                index = par_index;
            }
        }
    }

    static void percolate_down(T *arr, int size, int index)
    {
        // percolate down algorithm - It is an algorithm that sorts the input index to size-1
        // index according to minheap rules. Find the smallest value after index and swap the value of index
        // and the smallest value, and the child value of index will repeat it until all values follow
        // minheap's rules. If the index reaches the last index, or if the values are correctly ordered, loop
        // ends.
        int child_index = child(index);
        T cur_value = arr[index];

        while (child_index < size) {
            T min = cur_value;
            int min_index = -1;
            for (int i = 0; i < 2 && i + child_index < size; i++) { // repeat two child
                if (arr[i + child_index] < min) {                   // to find minimum value
                    min = arr[i + child_index];
                    min_index = i + child_index;
                }
            }

            if (min == cur_value) { // terminate condition
                return;
            } else { // repeat until find the minimum value
                swap(arr[index], arr[min_index]);
                index = min_index;
                child_index = child(index);
            }
        }
    }

    static void heapify(T *arr, int size)
    {
        // heapify array - Compare the values from the first index to size-1 index with
        // values in the parent index. If there is a relationship that violates the minheap rules,reorder it
        // through percolate up.
        for (int i = 1; i < size; i++) {
            int cur_index = i;
            int par_index = parent(cur_index);

            if (arr[cur_index] < arr[par_index]) {
                percolate_up(arr, cur_index);
            }
        }
    }

    static void make_heap(T *arr, int size, int hint = -1)
    {
        // Build heap - It is an algorithm heapify the entered array. If the hint exists in range(0~size-1),
        // only the value in the hint index needs to be heapfy. So i can sort it using percolate algorithm. If
        // hint is not given, use the heapify function to heapify.
        if (hint >= 0 && hint < size) {
            percolate_down(arr, size, hint);
            percolate_up(arr, hint);
        } else {
            heapify(arr, size);
        }
    }

    static void pop(T *arr, int size)
    {
        // Delete minimum - Delete the smallest value in the heap(here is the value at index 0). It is
        // difficult to apply concept of delete in an array already declared in size, so here is implemented
        // in a way that is exchanged for the last value(the value at index size-1) and does not approach the
        // size-1 index. Also, since the size of the value(at index 0) has changed, the minheap is reordered
        // again from index 0 through percolate down.
        swap(arr[0], arr[size - 1]);
        percolate_down(arr, size - 1, 0);
    }

    static void push(T *arr, int size, T item)
    { // Push item - Push item to arr[size] Then, to consider the rules of minheap, run the percolate up(at
      // that time index is size).(Also, I didn't consider the situation that push is excuted when the array
      // is full.)
        arr[size] = item;
        percolate_up(arr, size);
    }
};

template <typename T>
class LFU {
    struct LFUItem {
        T item;
        int usedCount; // The number of accesses after inserted to the cache.

        explicit LFUItem(T _item = T(), int _usedCount = 0)
        {
            usedCount = _usedCount;
            item = _item;
        }

        bool operator==(const LFUItem &other) const
        {
            return item == other.item;
        }
        bool operator<(const LFUItem &other) const
        {
            return usedCount < other.usedCount;
        }
        bool operator>(const LFUItem &other) const
        {
            return usedCount > other.usedCount;
        }
    };

    LFUItem **cache;
    const int size;
    int misses;
    int cur_size;
    int hint_index;
    // I add two variable `int cur_size`, `int hint_index` as class member variable. `cur_size` means the
    // number of data in cache. Also, `hint_index` is used at `touch()` as hint index of `make_heap()`.

  public:
    LFU(int _size) : size(_size), misses(0), cur_size(0), hint_index(-1)
    {
        cache = new LFUItem *[size];

        for (int i = 0; i < size; i++) {
            cache[i] = nullptr;
        }
        // Constructor - initialization member variable and pointer.
    }

    ~LFU()
    {
        for (int i = 0; i < size; i++) {
            if (cache[i] != nullptr) {
                delete cache[i];
            }
        }
        delete[] cache;
        // Destructor - delete the pointer dynamic allocated.
    }
    LFUItem *find(T _item)
    { // my implementation : if find item in cache, return cache ptr
        for (int i = 0; i < this->size; i++) {
            if (cache[i] != nullptr) {
                if (cache[i]->item == _item) {
                    this->hint_index = i;
                    return this->cache[i];
                }
            }
        }
        return nullptr;
    }

    bool exists(T item)
    {
        return find(item) != nullptr;
        // Return true if data exists in the cache
        // Return false otherwise
    }

    int status() const
    {
        return this->cur_size;
        // Return number of elements in cache
    }

    bool touch(T item)
    {
        // Return true on hit, false on miss. This function uses the `exists()` to check if the item exists in
        // cache. If the item exists then check cache is full or not, and if the cache is full, delete the
        // value that used to have a small number of access through `pop()` and insert a new value through
        // `push`. (This is implemented through MinHeap.) If the cache is not full, just add a value through
        // `push()`. If there exists an item in the cache already, add 1 more `used_count` of the
        // corresponding `LFUItem` and reorder the heap.
        if (exists(item)) {
            find(item)->usedCount++;
            MinHeap<LFUItem *>::make_heap(cache, cur_size, hint_index);
            return true;
        } else {
            if (cur_size < size) {
                LFUItem *dataPtr = new LFUItem;
                dataPtr->item = item;
                dataPtr->usedCount = 0;

                MinHeap<LFUItem *>::push(cache, cur_size, dataPtr);

                this->misses++;
                this->cur_size++;

            } else { // cur_size == size
                MinHeap<LFUItem *>::pop(cache, cur_size);

                LFUItem *temp = cache[cur_size - 1];
                delete temp;
                cache[cur_size - 1] = nullptr;
                this->cur_size--;

                LFUItem *dataPtr = new LFUItem;
                dataPtr->item = item;
                dataPtr->usedCount = 0;

                MinHeap<LFUItem *>::push(cache, cur_size, dataPtr);

                this->misses++;
                this->cur_size++;
            }
            return false;
        }

        // The data is being accessed
        // Return true on hit, false on miss
        // i.e. return true if it existed, and false when it was not
    }

    int getMisses()
    {
        return this->misses;
        // Return the number of cache misses until now
    }
};

} // namespace caches

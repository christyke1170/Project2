#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include "Eecs281PQ.h"

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            this->compare = comp;
    } // BinaryPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }{
            this->compare = comp;
            for (auto it = start; it != end; ++it){
                data.push_back(*it);
            }
            //fix down bottom to top (bottomup)
            updatePriorities();
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()

    
    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
//    it's just size/2 not minus one, and it's done by one index
        for (int i = (int)(data.size()/2); i >= 1; --i ){
            fixDown((int)data.size(), i);
        }
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
//        std::cout << "size before " << data.size() << std::endl;
//        std::cout << "val before " << val << std::endl;
//        if(data.empty()){
//            data.push_back(val);
//            std::cout << "size after " << data.size() << std::endl;
//            return;
//        }
//      since you have the get element function you don't need to resize
        data.push_back(val);
        fixUp((int)data.size());
//        std::cout << "size after " << data.size() << std::endl;
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        getElement(1) = getElement(data.size());
//      once you switch the root to the end of the heap, you need to delete the end
        data.pop_back();
        fixDown((int)data.size(), 1);
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        return getElement(1);
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return data.size();
    } // size()


    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return data.size() == 0;
    } // empty()


private:
    // Note: This vector *must* be used your heap implementation.
    std::vector<TYPE> data;

    // TODO: Add any additional member functions or data you require here.
    // For instance, you might add fixUp() and fixDown().
    
    //FixDown
    void fixDown(int size, int k){
        while((2*k) <= size){
            int j = 2*k; //start with left child
//            condition needs to be both smaller in size and data[j] < data[j+1]
            if(j < size && this->compare(getElement((size_t)j),getElement((size_t)j+1))){
//                if(getElement((size_t)j) < getElement((size_t)j+1)){
                   ++j;
                }

            if(!(this->compare(getElement((size_t)k),getElement((size_t)j)))){
                break;
                
                
            }
            std::swap((getElement((size_t)j)), getElement((size_t)k));
            k=j; //move down
        }
    }
    
    //FixUp
    void fixUp(int k){
        while(k>1 && (this->compare(getElement((size_t)k/2),getElement((size_t)k)))){
            std::swap(getElement((size_t)k), getElement((size_t)k/2));
            k/=2; //move to parent
        }
    }
    
    // Translate 1-based indexing into a 0-based vector
    TYPE &getElement(std::size_t i) {
        return data[i - 1];
    } // getElement()
    
    const TYPE &getElement(std::size_t i) const {
        return data[i - 1];
    } // getElement()
    

}; // BinaryPQ


#endif // BINARYPQ_H

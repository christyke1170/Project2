#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>


// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        explicit Node(const TYPE &val)
            : elt{ val }, child{ nullptr }, sibling{ nullptr }, prev{ nullptr }
        {}

        // Description: Allows access to the element at that Node's position.
        // There are two versions, getElt() and a dereference operator, use
        // whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        //const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data members of this
        // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
        // statement in PairingPQ's add_node() function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child;
        Node *sibling;
        // TODO: Add one extra pointer (parent or previous) as desired.
        Node *prev;
    }; // Node


       // Description: Construct an empty priority_queue with an optional comparison functor.
       // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
            this->compare = comp;
    } // PairingPQ()


      // Description: Construct a priority_queue out of an iterator range with an optional
      //              comparison functor.
      // Runtime: O(n) where n is number of elements in range.
      // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
        this->compare = comp;
        pairing = 0;
        for (auto i = start; i < end; ++i) {
            addNode(*i);
        }
    } // PairingPQ()


      // Description: Copy constructor.
      // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare } {
        // TODO: Implement this function.
        if (other.empty()) {
            return;
        }
        else {
            std::deque<Node*> holder;
            Node *temp = other.heap;
            holder.push_back(temp);
            while (!holder.empty()) {
                if (temp->sibling != nullptr) {
                    holder.push_back(temp->sibling);
                }
                if (temp->child != nullptr) {
                    holder.push_back(temp->child);
                }
                push(temp->elt);
                holder.pop_front();
                temp = holder.front();
            }
        }
    } // PairingPQ()


      // Description: Copy assignment operator.
      // Runtime: O(n)
      // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) {
        // TODO: Implement this function.
        PairingPQ pq(rhs);
        swap(pq);
        return *this;
    } // operator=()


      // Description: Destructor
      // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        if (!empty()) {
            pairing = 0;
            std::deque<Node*> boom;
            Node *temp = heap;
            boom.push_back(temp);
            while (!boom.empty()) {
                temp = boom.front();
                if (temp->child != nullptr) {
                    boom.push_back(temp->child);
                }
                if (temp->sibling != nullptr) {
                    boom.push_back(temp->sibling);
                }
                delete temp;
                boom.pop_front();
            }
            temp = nullptr;
            pairing = 0;
        }
        pairing = 0;
    } // ~PairingPQ()


      // Description: Assumes that all elements inside the priority_queue are out of order and
      //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
      // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        if (pairing < 2) {
            return;
        }
        std::deque<Node*> update;
        Node* temp = heap;
        update.push_back(heap);
        while (!update.empty()) {
            temp = update.front();
            if (temp->child != nullptr) {
                update.push_back(temp->child);
                temp->child->prev = nullptr;
                temp->child = nullptr;
            }
            if (temp->sibling != nullptr) {
                update.push_back(temp->sibling);
                temp->sibling->prev = nullptr;
                temp->sibling = nullptr;
            }
            if (temp->prev != nullptr) {
                if (temp->prev->child == temp) {
                    temp->prev->child = nullptr;
                }
                else {
                    temp->prev->sibling = nullptr;
                }
                temp->prev = nullptr;
            }
            heap = meld(heap, update.front());
            update.pop_front();
        }
    } // updatePriorities()


      // Description: Add a new element to the priority_queue. This has been provided for you,
      //              in that you should implement push functionality in the
      //              addNode function.
      // Runtime: Amortized O(1)
      // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
        // TODO: Implement this function.
        addNode(val);
    } // push()


      // Description: Remove the most extreme (defined by 'compare') element from
      //              the priority_queue.
      // Note: We will not run tests on your code that would require it to pop an
      // element when the priority_queue is empty. Though you are welcome to if you are
      // familiar with them, you do not need to use exceptions in this project.
      // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        if (heap == nullptr) {
            return;
        }
        else if (heap->child == nullptr) {
            delete heap;
            pairing = 0;
            return;
        }
        else {
            std::deque<Node*> pop;
            Node* temp = heap->child;
            heap->child = nullptr;
            while (temp != nullptr) {
                pop.push_back(temp);
                if (temp->prev != heap && temp->prev != nullptr) {
                    temp->prev->sibling = nullptr;
                }
                temp->prev = nullptr;
                temp = temp->sibling;
            }
            delete heap;
            pairing -= 1;

            while (pop.size() > 1) {
                Node* fiddle = pop.front();
                pop.pop_front();
                Node* sticks = pop.front();
                pop.pop_front();
                pop.push_back(meld(fiddle, sticks));
            }
            heap = pop.front();
        }
    } // pop()


      // Description: Return the most extreme (defined by 'compare') element of
      //              the heap.  This should be a reference for speed.  It MUST be
      //              const because we cannot allow it to be modified, as that
      //              might make it no longer be the most extreme element.
      // Runtime: O(1)
    virtual const TYPE & top() const {
        // TODO: Implement this function
        return heap->elt;
    } // top()


      // Description: Get the number of elements in the priority_queue.
      // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return pairing;
    } // size()

      // Description: Return true if the priority_queue is empty.
      // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        return (pairing == 0);
    } // empty()


      // Description: Updates the priority of an element already in the priority_queue by
      //              replacing the element refered to by the Node with new_value.
      //              Must maintain priority_queue invariants.
      //
      // PRECONDITION: The new priority, given by 'new_value' must be more extreme
      //               (as defined by comp) than the old priority.
      //
      // Runtime: As discussed in reading material.
      // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
        // TODO: Implement this function
        node->elt = new_value;
        if (node != heap) {
            if (node->sibling != nullptr) {
                node->sibling->prev = node->prev;
                if (node->prev != nullptr) {
                    if (node->prev->child == node) {
                        node->prev->child = node->sibling;
                    }
                    else if (node->prev->sibling == node) {
                        node->prev->sibling = node->sibling;
                    }
                }
            }
            else if (node->prev != nullptr) {
                if (node->prev->child == node) {
                    node->prev->child = nullptr;
                }
                else if (node->prev->sibling == node) {
                    node->prev->sibling = nullptr;
                }
            }
            node->sibling = nullptr;
            node->prev = nullptr;
            heap = meld(heap, node);
        }
    } // updateElt()


      // Description: Add a new element to the priority_queue. Returns a Node* corresponding
      //              to the newly added element.
      // Runtime: Amortized O(1)
      // TODO: when you implement this function, uncomment the parameter names.
    Node* addNode(const TYPE & val) {
        // TODO: Implement this function
        pairing += 1;
        Node *temp = new Node(val);
        if (heap == nullptr) {
            heap = temp;
        }
        else {
            heap = meld(heap, temp);
        }
        return temp;
    } // addNode()


private:
    // TODO: Add any additional member functions or data you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    size_t pairing = 0;
    Node *heap = nullptr;

    void swap(PairingPQ &other) {
        std::swap(heap, other.heap);
        std::swap(pairing, other.pairing);
    }

    Node* meld(Node* first, Node* second) {
        if (first == nullptr) {
            return second;
        }
        if (second == nullptr) {
            return first;
        }
        if (first == second) {
            return second;
        }
        Node *left = first;
        Node *right = second;
        if (this->compare(right->elt, left->elt)) {
            right->prev = left;
            if (left->child == nullptr) {
                left->child = right;
            }
            else {
                right->sibling = left->child;
                left->child = right;
                right->sibling->prev = right;
            }
            return left;
        }
        else {
            left->prev = right;
            if (right->child == nullptr) {
                right->child = left;
            }
            else {
                left->sibling = right->child;
                right->child = left;
                left->sibling->prev = left;
            }
            return right;
        }
    }
};


#endif // PAIRINGPQ_H
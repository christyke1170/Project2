#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority_queue' ADT implemented as a pairing priority_queue.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, prev(nullptr)
            {}

            // Description: Allows access to the element at that Node's position.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }

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
        psize = 0;
        for (auto it = start; it < end; ++it) {
            addNode(*it);
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare } {

            if (other.empty()) {
                return;
            }
            else {
                std::deque<Node*> deck;
                Node *copy = other.root;
                deck.push_back(copy);
                
                // TODO
                
                while (!deck.empty()) {
                    if (copy->sibling != nullptr) {
                        deck.push_back(copy->sibling);
                    }
                    if (copy->child != nullptr) {
                        deck.push_back(copy->child);
                    }
                    push(copy->elt);
                    
                    deck.pop_front();
                    copy = deck.front();
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
        if (!empty()) {
            psize = 0;
            std::deque<Node*> destruct;
            Node *curr = root;
            destruct.push_back(curr);
            while (!destruct.empty()) {
                curr = destruct.front();
                if (curr->child != nullptr) {
                    destruct.push_back(curr->child);
                }
                if (curr->sibling != nullptr) {
                    destruct.push_back(curr->sibling);
                }
                delete curr;
                destruct.pop_front();
            }
            curr = nullptr;
            psize = 0;
        }
        psize = 0;
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        if (psize < 2) return;
        
        std::deque<Node*> deck1;
        Node* ah = root;
        deck1.push_back(root);
        while (!deck1.empty()) {
            //
            ah = deck1.front();
            if (ah->child != nullptr) {
                deck1.push_back(ah->child);
                ah->child->prev = nullptr;
                ah->child = nullptr;
            }
            if (ah->sibling != nullptr) {
                deck1.push_back(ah->sibling);
                ah->sibling->prev = nullptr;
                ah->sibling = nullptr;
            }
            if (ah->prev != nullptr) {
                if (ah->prev->child == ah) {
                    ah->prev->child = nullptr;
                }
                else {
                    ah->prev->sibling = nullptr;
                }
                ah->prev = nullptr;
            }
            root = meld(root, deck1.front());
            deck1.pop_front();
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
        if (root == nullptr) return;
        else if (root->child == nullptr) {
            delete root;
            //root = nullptr;
            psize = 0;
            return;
        }
        else {
            std::deque<Node*> children;
            Node* t = root->child;
            root->child = nullptr;
            while (t != nullptr) {
                children.push_back(t);
                if (t->prev != root && t->prev != nullptr){
                    t->prev->sibling = nullptr;
                }
                t->prev = nullptr;
                t = t->sibling;
            }
            
            delete root;
            psize -= 1;
            
            
            while (children.size() > 1) {
                Node* temp1 = children.front();
                children.pop_front();
                Node* temp2 = children.front();
                children.pop_front();
                children.push_back(meld(temp1, temp2));
            }
            root = children.front();
        }

    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        // TODO: Implement this function
        return root->elt;
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return psize;
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return (psize == 0);
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
        node->elt = new_value;
        if (node != root) {
            if (node->sibling != nullptr) {
                node->sibling->prev = node->prev;
                
                if (node->prev != nullptr) {
                    if (node->prev->child == node){
                        node->prev->child = node->sibling;
                    }
                    
                    else if (node->prev->sibling == node){
                        node->prev->sibling = node->sibling;
                    }
                    
                }
            }
            else if (node->prev != nullptr) {
                
                if (node->prev->child == node){
                    node->prev->child = nullptr;
                }
                
                else if (node->prev->sibling == node){
                    node->prev->sibling = nullptr;
                }
            }
            node->prev = nullptr;
            node->sibling = nullptr;
            root = meld(root, node);
        }

    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: Amortized O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    
    
    
    Node* addNode(const TYPE & val) {
        psize += 1;
        Node *newNode = new Node(val);
        if (root == nullptr){
            root = newNode;
        }
        else root = meld(root, newNode);
        return newNode;
    } // addNode()


private:
    // TODO: Add any additional member functions or data you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    
    Node* meld(Node* left, Node* right) {
        
        if (left == nullptr){
            return right;
        }
        if (right == nullptr){
            return left;
        }
        if (left == right){
            return right;
        }
        Node *lhs = left;
        Node *rhs = right;
        
        if (this->compare(rhs->elt, lhs->elt)) { //if LHS higher than RHS
            //std::cout << "rhs,lhs\n";
            rhs->prev = lhs;
            if (lhs->child == nullptr) {
                lhs->child = rhs;
            }
            else {
                rhs->sibling = lhs->child;
                lhs->child = rhs;
                rhs->sibling->prev = rhs;
            }
            return lhs;
        }
        else {
            //std::cout << "lhs,rhs\n";
            lhs->prev = rhs;
            if (rhs->child == nullptr) {
                rhs->child = lhs;
            }
            else {
                //rhs->child->prev = lhs;
                lhs->sibling = rhs->child;
                rhs->child = lhs;
                //lhs->prev = rhs;
                lhs->sibling->prev = lhs;
            }
            return rhs;
        }
    }
    
    void swap(PairingPQ &other) {
        std::swap(root, other.root);
        std::swap(psize, other.psize);
    }
    
    size_t psize = 0;
    Node *root = nullptr;

};


#endif // PAIRINGPQ_H

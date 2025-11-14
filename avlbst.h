#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key,Value>* n, int diff);
    void rotateLeft(AVLNode<Key,Value>* rotate);
    void rotateRight(AVLNode<Key,Value>* rotate);
    int getHeight(AVLNode<Key, Value>* node) const;

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // TODO
    // if empty, create new root
    if (this->root_ == nullptr) {
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      return;
    }

    // otherwise, do bst search (cries)
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    
    while (curr != nullptr) {
      parent = curr;
      if (new_item.first < curr->getKey()) {
        curr = curr->getLeft();
      }
      else if (new_item.first > curr->getKey()) {
        curr = curr->getRight();
      }
      else {
        curr->setValue(new_item.second);
        return;
      }
    }

    AVLNode<Key, Value>* temp = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (temp->getKey() < parent->getKey()) {
      parent->setLeft(temp);
    }
    else {
      parent->setRight(temp);
    }

    // if (parent->getLeft() == temp) {
    //   parent->updateBalance(1);
    // }
    // else {
    //   parent->updateBalance(-1);
    // } 

    // std::cout << "DEBUG: parent = " << parent->getKey() << " balance BEFORE updating = " << (int)parent->getBalance() << std::endl;

    AVLNode<Key, Value>* n = temp;
    AVLNode<Key, Value>* p = parent;

    while (p != nullptr) { // left
      if (n->getKey() < p->getKey()) {
        p->updateBalance(1);
      }
      else { // right
        p->updateBalance(-1);
      }

      int b = p->getBalance();

      // balance = 0

      if (b == 0) return;

      if (b == 2 || b == -2) {
        insertFix(p, n); // rotate
        return;
      }

      n = p;
      p = p->getParent();
    }


    // insertFix(parent, temp);

    // std::cout << "DEBUG: parent = " << parent->getKey() << " balance AFTER updating = " << (int)parent->getBalance() << std::endl;

}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (temp == nullptr) return;

    // if node has two children, swap w predecessor
    if (temp->getLeft() != nullptr && temp->getRight() != nullptr) {
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(temp));

      nodeSwap(temp, pred); 
    }
    // aftwer swap: node will have at most one child

    AVLNode<Key, Value>* parent = temp->getParent();
    // which side shrinks...
    int d = 0;
    if(parent != nullptr) {
      if (parent->getLeft() == temp) d = 1;
      else d = -1;
    }

    // remove node
    AVLNode<Key, Value>* child = nullptr;

    if (temp->getLeft() != nullptr) child = temp->getLeft();
    else if (temp->getRight() != nullptr) child = temp->getRight();

    // attach child to node's parent
    if (child != nullptr) child->setParent(parent);

    // remove
    if (parent == nullptr) this->root_ = child;
    else if (parent->getLeft() == temp) parent->setLeft(child);
    else parent->setRight(child);

    delete temp;
    removeFix(parent, d);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* rotate) {
  AVLNode<Key, Value>* new_parent = rotate->getRight();
  AVLNode<Key, Value>* og_parent = rotate->getParent();

  bool r = (og_parent == nullptr);
  bool lc = (!r && og_parent->getLeft() == rotate);

  rotate->setRight(new_parent->getLeft()); // move the new parents subtree to b rotate's subtree
  if (new_parent->getLeft() != nullptr) {
    new_parent->getLeft()->setParent(rotate); // update parent ptr
  }
  // rotate becomes child of the new parent
  new_parent->setLeft(rotate); 
  rotate->setParent(new_parent);

  // reconnect the new parent to the tree where the rotated node was.
  // if the rotated node was the root, then y becomes the new root
  new_parent->setParent(og_parent);

  if (r) this->root_ = new_parent;
  else if (lc) og_parent->setLeft(new_parent);
  else og_parent->setRight(new_parent);

  // new_parent->setParent(og_parent);

  // rotate->updateBalance(1);
  // new_parent->updateBalance(-1);  
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* rotate) {
  AVLNode<Key, Value>* new_parent = rotate->getLeft();
  AVLNode<Key, Value>* og_parent = rotate->getParent();

  bool r = (og_parent == nullptr);
  bool rc = (!r && og_parent->getRight() == rotate);

  rotate->setLeft(new_parent->getRight()); // move the new parents subtree to b rotate's subtree
  if (new_parent->getRight() != nullptr) {
    new_parent->getRight()->setParent(rotate); // update parent ptr
  }
  // rotate becomes child of the new parent
  new_parent->setRight(rotate); 
  rotate->setParent(new_parent);

  // reconnect the new parent to the tree where the rotated node was.
  // if the rotated node was the root, then y becomes the new root
  new_parent->setParent(og_parent);

  if (r) this->root_ = new_parent;
  else if (rc) og_parent->setRight(new_parent);
  else og_parent->setLeft(new_parent);

  // new_parent->setParent(og_parent);

  // rotate->updateBalance(-1);
  // new_parent->updateBalance(1);  
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key, Value>* n) {
  
  // std::cout << "DEBUG: insertFix p = " << p->getKey() 

  if (p == nullptr) return; 

  if (p->getBalance() == 0) return;

  // left heavy 

  if (p->getBalance() == 2) {
    // if child is left heavy -> LL rotation
    if (n->getBalance() == 1) {
      rotateRight(p);
      p->setBalance(0); 
      n->setBalance(0);
    }

    // if child is right-heavy -> LR rotation
    // req. more test cases 
    else {
      AVLNode<Key, Value>* c = n->getRight();
      rotateLeft(n);
      rotateRight(p);

      if (c->getBalance() == 1) {
        p->setBalance(-1);
        n->setBalance(0);
      }
      else if (c->getBalance() == -1) {
        p->setBalance(0);
        n->setBalance(1);
      }
      else {
        p->setBalance(0);
        n->setBalance(0);
      }

      c->setBalance(0);
    }
  }

  // right heavy 
  else if (p->getBalance() == -2) {
  // RR 
    if (n->getBalance() == -1) {
      rotateLeft(p);
      p->setBalance(0); 
      n->setBalance(0);
    }
    // RL
    else {
      AVLNode<Key, Value>* c = n->getLeft();
      rotateRight(n);
      rotateLeft(p);
      if (c->getBalance() == 1) {
        p->setBalance(0);
        n->setBalance(-1);
      }
      else if (c->getBalance() == -1) {
        p->setBalance(1);
        n->setBalance(0);
      }
      else {
        p->setBalance(0);
        n->setBalance(0);
      }

      c->setBalance(0);
    }

  }
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value>* node) const {
  if (node == nullptr) return 0;

  int lh = getHeight(node->getLeft());
  int rh = getHeight(node->getRight());

  if (lh > rh) return lh + 1;
  else return rh + 1;
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff) {

  // if (n == nullptr) return;
  AVLNode<Key, Value>* curr = n;
  // int nd = 0; // left or right child

  while (curr != nullptr) {
    int lh = getHeight(curr->getLeft());
    int rh = getHeight(curr->getRight());
    int bf = lh - rh;


    // balance
    curr->setBalance(bf);

    // case 1: left heavy
    if (bf > 1) {
      AVLNode<Key, Value>* c = curr->getLeft();
      int clh = getHeight(c->getLeft());
      int crh = getHeight(c->getRight());

      // LL
      if (clh >= crh) rotateRight(curr);
      else {
        // LR
        rotateLeft(c);
        rotateRight(curr);
      }
    }

    // case 2 : right heavy
    else if (bf < -1) {
      AVLNode<Key, Value>* c = curr->getRight();
      int crh = getHeight(c->getRight());
      int clh = getHeight(c->getLeft());

      // RR
      if (crh >= clh) rotateLeft(curr);
      else {
        // RL
        rotateRight(c);
        rotateLeft(curr);
      }
    }
    curr = curr->getParent();
  }
}

#endif

/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <cstddef>
#include <ctime>
#include <functional>
#include "exceptions.hpp"
#include "utility.hpp"

namespace sjtu {
template<
   class Key,
   class T,
   class Compare = std::less <Key>
   > class map {
  public:
   /**
  * the internal type of data.
  * it should have a default constructor, a copy constructor.
  * You can use sjtu::map as value_type by typedef.
    */
   typedef pair<const Key, T> value_type;
   /**
  * see BidirectionalIterator at CppReference for help.
  *
  * if there is anything wrong throw invalid_iterator.
  *     like it = map.begin(); --it;
  *       or it = map.end(); ++end();
    */
private:
    enum colourT{RED, BLACK};
    struct RedBlackNode {
        value_type data;
        RedBlackNode* l;
        RedBlackNode* r;
        RedBlackNode* parent;
        colourT colour;

        RedBlackNode(const colourT c, const value_type& v, RedBlackNode* left = nullptr, RedBlackNode* right = nullptr, RedBlackNode* p = nullptr)
            :data(v), l(left), r(right), parent(p), colour(c){};
    };
    RedBlackNode* root;
    size_t _size;
    Compare comp;
    friend class iterator;
    void transplant(RedBlackNode* u, RedBlackNode* v) {
        if (u->parent == nullptr) root = v;
        else if (u == u->parent->l) u->parent->l = v;
        else u->parent->r = v;

        if (v) v->parent = u->parent;
    }
    bool isBlack(RedBlackNode* x) {
    return x == nullptr || x->colour == BLACK;
}
    void insertFix(RedBlackNode* node) {
        RedBlackNode* parent = nullptr;
        RedBlackNode* grand = nullptr;

        while (node != root && node->parent->colour == RED) {
            parent = node->parent;
            grand = parent->parent;

            if (parent == grand->l) {
                RedBlackNode* uncle = grand->r;
                if (uncle && uncle->colour == RED) {
                    parent->colour = BLACK;
                    uncle->colour = BLACK;
                    grand->colour = RED;
                    node = grand;
                } else {
                    if (node == parent->r) {
                        rotateLeft(parent);
                        node = parent;
                        parent = node->parent;
                    }
                    parent->colour = BLACK;
                    grand->colour = RED;
                    rotateRight(grand);
                }
            } else {
                RedBlackNode* uncle = grand->l;
                if (uncle && uncle->colour == RED) {
                    parent->colour = BLACK;
                    uncle->colour = BLACK;
                    grand->colour = RED;
                    node = grand;
                } else {
                    if (node == parent->l) {
                        rotateRight(parent);
                        node = parent;
                        parent = node->parent;
                    }
                    parent->colour = BLACK;
                    grand->colour = RED;
                    rotateLeft(grand);
                }
            }
        }

        root->colour = BLACK;
    }
    void deleteFix(RedBlackNode* x, RedBlackNode* parent) {
        while ((x != root) && isBlack(x)) {

        if (x == (parent ? parent->l : nullptr)) {

            RedBlackNode* w = parent ? parent->r : nullptr;

            if (w && w->colour == RED) {
                w->colour = BLACK;
                parent->colour = RED;
                rotateLeft(parent);
                w = parent->r;
            }

            if (isBlack(w ? w->l : nullptr) && isBlack(w ? w->r : nullptr)) {
                if (w) w->colour = RED;
                x = parent;
                parent = x ? x->parent : nullptr;
            } else {

                if (isBlack(w ? w->r : nullptr)) {
                    if (w && w->l) w->l->colour = BLACK;
                    if (w) {
                        w->colour = RED;
                        rotateRight(w);
                    }
                    w = parent ? parent->r : nullptr;
                }

                if (w) w->colour = parent->colour;
                parent->colour = BLACK;
                if (w && w->r) w->r->colour = BLACK;
                rotateLeft(parent);

                x = root;
                break;
            }

        } else {
            RedBlackNode* w = parent ? parent->l : nullptr;

            if (w && w->colour == RED) {
                w->colour = BLACK;
                parent->colour = RED;
                rotateRight(parent);
                w = parent->l;
            }

            if (isBlack(w ? w->l : nullptr) && isBlack(w ? w->r : nullptr)) {
                if (w) w->colour = RED;
                x = parent;
                parent = x ? x->parent : nullptr;
            } else {

                if (isBlack(w ? w->l : nullptr)) {
                    if (w && w->r) w->r->colour = BLACK;
                    if (w) {
                        w->colour = RED;
                        rotateLeft(w);
                    }
                    w = parent ? parent->l : nullptr;
                }

                if (w) w->colour = parent->colour;
                parent->colour = BLACK;
                if (w && w->l) w->l->colour = BLACK;
                rotateRight(parent);

                x = root;
                break;
            }
        }
    }

    if (x) x->colour = BLACK;
}
    void rotateLeft(RedBlackNode* x) {
        RedBlackNode* y = x->r;
        if (y == nullptr) return;

        x->r = y->l;
        if (y->l)
            y->l->parent = x;

        y->parent = x->parent;

        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->l)
            x->parent->l = y;
        else
            x->parent->r = y;

        y->l = x;
        x->parent = y;
    }
    void rotateRight(RedBlackNode* x) {
        RedBlackNode* y = x->l;
        if (y == nullptr) return;

        x->l = y->r;
        if (y->r)
            y->r->parent = x;

        y->parent = x->parent;

        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->r)
            x->parent->r = y;
        else
            x->parent->l = y;

        y->r = x;
        x->parent = y;
    }
    void LL(RedBlackNode* x) {
        rotateRight(x);
    }
    void LR(RedBlackNode* gp) {
        rotateLeft(gp->l);
        rotateRight(gp);
    }
    void RL(RedBlackNode* gp) {
        rotateRight(gp->r);
        rotateLeft(gp);
    }
    void RR(RedBlackNode* x) {
        rotateLeft(x);
    }
    void insertAdjust(RedBlackNode* gp, RedBlackNode* p, RedBlackNode* t) {
        RedBlackNode* uncle = (gp->l == p ? gp->r : gp->l);

        if (uncle && uncle->colour == RED) {
            p->colour = BLACK;
            uncle->colour = BLACK;
            gp->colour = RED;
            return;
        }

        if (p == root) {
            p->colour = BLACK;
            return;
        }

        if (gp->l == p) {
            if (p->l == t)
                LL(gp);
            else
                LR(gp);
        }
        else {
            if (p->r == t)
                RR(gp);
            else
                RL(gp);
        }
    }

    void removeAdjust(RedBlackNode* &p, RedBlackNode* &c, RedBlackNode* &t, Key del) {
        if (c->colour == RED) return;
        if (c == root) {
            if (c->l && c->r && c->r->colour == c->l->colour) {
                c->colour = RED;
                c->l->colour = c->r->colour = BLACK;
                return;
            }
        }
        if ((c->l && c->l->colour || c->l == nullptr) && (c->r && c->r->colour || c->r == nullptr)) {
            if ((t->l && t->l->colour || t->l == nullptr) && (t->r && t->r->colour || t->r == nullptr)) {
                p->colour = BLACK;
                t->colour = c->colour = RED;
            }
            else {
                if (p->l == t) {
                    if (t->l && t->l->colour == RED) {
                        t->l->colour = BLACK;
                        LL(p);
                        p = t;
                    }
                    else {
                        LR(p);
                        p = p->r;
                        p->colour = BLACK;
                    }
                }
                else if (t->r && t->r->colour == RED) {
                    t->r->colour = BLACK;
                    RR(p);
                    p = t;
                }
                else {
                    RL(p);
                    p = p->l;
                    p->colour = BLACK;
                }
                c->colour = RED;
            }
        }
        else {
            if (!comp(c->data.first, del) && !comp(del, c->data.first)) {
                if (c->l && c->r) {
                    if (c->r->colour == BLACK) {
                        LL(c);
                        c = c->r;
                    }
                    return;
                }
                if (c->l) {
                    LL(c);
                    p = c;
                    c = c->r;
                }
                else {
                    RR(c);
                    p = c;
                    c = c->l;
                }
            }
            else {
                p = c;
                c = (comp(del, p->data.first) ? p->l : p->r);
                t = (c == p->l ? p->r:p->l);
                if (c->colour == BLACK) {
                    if (t == p->r) {
                        RR(p);
                    }
                    else {
                        LL(p);
                    }
                    p = t;
                    t = (c == p->l? p->r:p->l);
                    removeAdjust(p, c, t, del);
                }
            }
        }
    }
    void makeEmpty(RedBlackNode* node) {
        if (node != nullptr) {
            makeEmpty(node->l);
            makeEmpty(node->r);
            node->~RedBlackNode();
            ::operator delete(node);
        }
        node = nullptr;
    }

    RedBlackNode* copy(RedBlackNode* parent, RedBlackNode* other) {
        if (other == nullptr) return nullptr;

        auto* node = static_cast<RedBlackNode*>(::operator new (sizeof(RedBlackNode)));
        new (node) RedBlackNode(other->colour, other->data, nullptr, nullptr, parent);

        node->l = copy(node, other->l);
        node->r = copy(node, other->r);
        return node;
    }

    RedBlackNode* findMin(RedBlackNode* node) const{
        if (!node) return nullptr;
        while (node->l) node = node->l;
        return node;
    }

    RedBlackNode* findMax(RedBlackNode* node) const{
        if (!node) return nullptr;
        while (node->r) node = node->r;
        return node;
    }

public:
   class const_iterator;
   class iterator {
       friend class map<Key,T,Compare>;
      private:
       /**
    * TODO add data members
    *   just add whatever you want.
        */
       RedBlackNode* ptr;
       map* _map;
      public:
       iterator(map* m = nullptr, RedBlackNode* p = nullptr):ptr(p), _map(m) {
           // TODO
       }

       iterator(const iterator &other):ptr(other.ptr), _map(other._map) {
           // TODO
       }

       /**
    * TODO iter++
        */
       iterator operator++(int) {
           iterator tmp = *this;
           ++(*this);
           return tmp;
       }

       /**
    * TODO ++iter
        */
       iterator &operator++() {
           if (ptr == nullptr)
               throw invalid_iterator();

           if (ptr->r != nullptr) {
               ptr = ptr->r;
               while (ptr->l) ptr = ptr->l;
               return *this;
           }

           RedBlackNode* p = ptr->parent;
           while (p && ptr == p->r) {
               ptr = p;
               p = p->parent;
           }

           ptr = p;
           return *this;
       }

       /**
    * TODO iter--
        */
       iterator operator--(int) {
           iterator tmp = *this;
           --(*this);
           return tmp;
       }

       /**
    * TODO --iter
        */
       iterator &operator--() {
           if (_map == nullptr)
               throw invalid_iterator();

           if (ptr == nullptr) {
               ptr = _map->findMax(_map->root);
               if (!ptr) throw invalid_iterator();
               return *this;
           }

           if (ptr->l) {
               ptr = ptr->l;
               while (ptr->r) ptr = ptr->r;
               return *this;
           }

           RedBlackNode* p = ptr->parent;
           while (p && ptr == p->l) {
               ptr = p;
               p = p->parent;
           }

           if (p == nullptr) throw invalid_iterator();

           ptr = p;
           return *this;
       }

       /**
    * a operator to check whether two iterators are same (pointing to the same memory).
        */
       value_type &operator*() const {
           return ptr->data;
       }

       bool operator==(const iterator &rhs) const {
           return (ptr == rhs.ptr && _map == rhs._map);
       }

       bool operator==(const const_iterator &rhs) const {
           return (ptr == rhs.ptr && _map == rhs._map);
       }

       /**
    * some other operator for iterator.
        */
       bool operator!=(const iterator &rhs) const {
           return !(*this == rhs);
       }

       bool operator!=(const const_iterator &rhs) const {
           return !(*this == rhs);
       }

       /**
    * for the support of it->first.
    * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
        */
       value_type *operator->() const noexcept {
           return &(ptr->data);
       }
   };
   class const_iterator {
       friend class map<Key,T,Compare>;
       // it should has similar member method as iterator.
       //  and it should be able to construct from an iterator.
      private:
       // data members.
       RedBlackNode* ptr;
       const map* _map;
      public:
       const_iterator(const map* m = nullptr, RedBlackNode* p = nullptr):ptr(p), _map(m) {
           // TODO
       }

       const_iterator(const iterator &other):ptr(other.ptr), _map(other._map) {
           // TODO
       }

       // And other methods in iterator.
       // And other methods in iterator.
       // And other methods in iterator.
        /**
    * TODO iter++
        */
       const_iterator operator++(int) {
           const_iterator tmp = *this;
           ++(*this);
           return tmp;
       }

       /**
    * TODO ++iter
        */
       const_iterator &operator++() {
           if (ptr == nullptr)
               throw invalid_iterator();

           if (ptr->r != nullptr) {
               ptr = ptr->r;
               while (ptr->l) ptr = ptr->l;
               return *this;
           }

           RedBlackNode* p = ptr->parent;
           while (p && ptr == p->r) {
               ptr = p;
               p = p->parent;
           }

           ptr = p;
           return *this;
       }

       /**
    * TODO iter--
        */
       const_iterator operator--(int) {
           const_iterator tmp = *this;
           --(*this);
           return tmp;
       }

       /**
    * TODO --iter
        */
       const_iterator &operator--() {
           if (_map == nullptr)
               throw invalid_iterator();

           if (ptr == nullptr) {
               ptr = _map->findMax(_map->root);
               if (!ptr) throw invalid_iterator();
               return *this;
           }

           if (ptr->l) {
               ptr = ptr->l;
               while (ptr->r) ptr = ptr->r;
               return *this;
           }

           RedBlackNode* p = ptr->parent;
           while (p && ptr == p->l) {
               ptr = p;
               p = p->parent;
           }

           if (p == nullptr) throw invalid_iterator();

           ptr = p;
           return *this;
       }

       /**
    * a operator to check whether two iterators are same (pointing to the same memory).
        */
       value_type &operator*() const {
           return ptr->data;
       }

       bool operator==(const iterator &rhs) const {
           return (ptr == rhs.ptr && _map == rhs._map);
       }

       bool operator==(const const_iterator &rhs) const {
           return (ptr == rhs.ptr && _map == rhs._map);
       }

       /**
    * some other operator for iterator.
        */
       bool operator!=(const iterator &rhs) const {
           return !(*this == rhs);
       }

       bool operator!=(const const_iterator &rhs) const {
           return !(*this == rhs);
       }

       const value_type* operator->() const {
           return &(ptr->data);
       }
   };

   /**
  * TODO two constructors
    */
   map():root(nullptr), _size(0) {};

   map(const map &other) {
       _size = other._size;
       comp = other.comp;
       root = copy(nullptr, other.root);
   }

   /**
  * TODO assignment operator
    */
   map &operator=(const map &other) {
       if (this == &other) return *this;
       clear();
       root = copy(nullptr, other.root);
       _size = other._size;
       comp = other.comp;
       return *this;
   }

   /**
  * TODO Destructors
    */
   ~map() {
       makeEmpty(root);
       root = nullptr;
   }

   /**
  * TODO
  * access specified element with bounds checking
  * Returns a reference to the mapped value of the element with key equivalent to key.
  * If no such element exists, an exception of type `index_out_of_bound'
    */
   T &at(const Key &key) {
       iterator it = find(key);
       if (it == end()){
           throw index_out_of_bound();
       }
       return it->second;
   }

   const T &at(const Key &key) const {
       const_iterator it = find(key);
       if (it == end()){
           throw index_out_of_bound();
       }
       return it->second;
   }

   /**
  * TODO
  * access specified element
  * Returns a reference to the value that is mapped to a key equivalent to key,
  *   performing an insertion if such key does not already exist.
    */
   T &operator[](const Key &key) {
       RedBlackNode* t=root; RedBlackNode* parent=nullptr;
       while(t){
           if(!(comp(key,t->data.first) || comp(t->data.first,key)))
               return t->data.second;
           parent=t;
           t=comp(key,t->data.first)? t->l : t->r;
       }
       RedBlackNode* node=new RedBlackNode(RED,value_type(key,T()),nullptr,nullptr,parent);
       if(!parent) root=node;
       else if(comp(key,parent->data.first)) parent->l=node;
       else parent->r=node;
       insertFix(node);
       _size++;
       return node->data.second;
   }

   /**
  * behave like at() throw index_out_of_bound if such key does not exist.
    */
   const T &operator[](const Key &key) const {
       const_iterator it = find(key);

       if (it == end()) {
           throw index_out_of_bound();
       }

       return it->second;
   }

   /**
  * return a iterator to the beginning
    */
   iterator begin() {
       if (root == nullptr) return end();
       RedBlackNode* tmp = root;
       while (tmp->l) {
           tmp = tmp->l;
       }
       return iterator(this, tmp);
   }

   const_iterator cbegin() const {
       if (root == nullptr) return end();
       RedBlackNode* tmp = root;
       while (tmp->l) {
           tmp = tmp->l;
       }
       return const_iterator(this, tmp);
   }

   /**
  * return a iterator to the end
  * in fact, it returns past-the-end.
    */
   iterator end() {
       return iterator(this, nullptr);
   }

    const_iterator end() const {
       return const_iterator(this, nullptr);
   }

   const_iterator cend() const {
       return const_iterator(this, nullptr);
   }

   /**
  * checks whether the container is empty
  * return true if empty, otherwise false.
    */
   bool empty() const {
       return _size == 0;
   }

   /**
  * returns the number of elements.
    */
   size_t size() const {
       return _size;
   }

   /**
  * clears the contents
    */
   void clear() {
       makeEmpty(root);
       root = nullptr;
       _size = 0;
   }

   /**
  * insert an element.
  * return a pair, the first of the pair is
  *   the iterator to the new element (or the element that prevented the insertion),
  *   the second one is true if insert successfully, or false.
    */
    pair<iterator, bool> insert(const value_type &v) {
       RedBlackNode* t=root; RedBlackNode* parent=nullptr;
       while(t){
           if(!(comp(v.first,t->data.first) || comp(t->data.first,v.first)))
               return {iterator(this,t),false};
           parent=t;
           t=comp(v.first,t->data.first)? t->l : t->r;
       }
       RedBlackNode* node=new RedBlackNode(RED,v,nullptr,nullptr,parent);
       if(!parent) root=node;
       else if(comp(v.first,parent->data.first)) parent->l=node;
       else parent->r=node;
       insertFix(node);
       _size++;
       return {iterator(this,node),true};
   }

   /**
  * erase the element at pos.
  *
  * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
    */
    void erase(iterator pos) {
       if (pos._map != this || pos.ptr == nullptr)
           throw invalid_iterator();

       RedBlackNode* z = pos.ptr;
       RedBlackNode* y = z;
       RedBlackNode* x = nullptr;
       RedBlackNode* x_parent = nullptr;

       colourT y_original_color = y->colour;

       if (z->l == nullptr) {
           x = z->r;
           x_parent = z->parent;
           transplant(z, z->r);
       } else if (z->r == nullptr) {
           x = z->l;
           x_parent = z->parent;
           transplant(z, z->l);
       } else {
           y = findMin(z->r); // successor
           y_original_color = y->colour;
           x = y->r;

           if (y->parent == z) {
               if (x) x->parent = y;
               x_parent = y;
           } else {
               transplant(y, y->r);
               y->r = z->r;
               y->r->parent = y;
               x_parent = y->parent;
           }

           transplant(z, y);
           y->l = z->l;
           y->l->parent = y;
           y->colour = z->colour;
       }

       z->~RedBlackNode();
       ::operator delete(z);
       _size--;

       if (y_original_color == BLACK) {
           deleteFix(x, x_parent);
       }
   }

   /**
  * Returns the number of elements with key
  *   that compares equivalent to the specified argument,
  *   which is either 1 or 0
  *     since this container does not allow duplicates.
  * The default method of check the equivalence is !(a < b || b > a)
    */
   size_t count(const Key &key) const {
       if (find(key) != end()) {
           return 1;
       }
       return 0;
   }

   /**
  * Finds an element with key equivalent to key.
  * key value of the element to search for.
  * Iterator to an element with key equivalent to key.
  *   If no such element is found, past-the-end (see end()) iterator is returned.
    */
   iterator find(const Key &key) {
       RedBlackNode* t = root;
       while (t != nullptr && (comp(t->data.first, key) || comp(key, t->data.first))) {
           if (comp(key, t->data.first)) t = t->l;
           else t = t->r;
       }
       if (t == nullptr) return end();
       return iterator(this, t);
   }

   const_iterator find(const Key &key) const {
       RedBlackNode* t = root;
       while (t != nullptr && (comp(t->data.first, key) || comp(key, t->data.first))) {
           if (comp(key, t->data.first)) t = t->l;
           else t = t->r;
       }
       if (t == nullptr) return end();
       return const_iterator(this, t);
   }

};

}

#endif
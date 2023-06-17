#include <iostream>
#ifndef ARRAYLIST_HPP
    #include "ArrayList.hpp"
#endif
using namespace std;

#define FQT_HPP
#define InnerList ArrayList<FQT::InnerNode *>
#define LeafList ArrayList<FQT::LeafNode *>
#define DataList ArrayList<FQT::Data *>

#ifndef abs
    #define abs(a) ((a) > 0 ? (a) : -1 * (a))
#endif
#ifndef min
    #define min(a, b) ((a) < (b) ? (a) : (b))
#endif

class FQT {

    public:

    struct Data {

        double data;

        Data() {
            data = 0;
        }

        Data(double r) {
            data = r;
        }
    };

    static inline long dist(Data &d1, Data &d2) {
        double dist = abs(d1.data - d2.data);
        if (dist - (long) dist < 0.5) {
            return (long) dist;
        }
        return (long) dist + 1;
    }

    private:

    struct LeafNode {

        long dist;
        DataList data;

        LeafNode(long d) {
            dist = d;
        }
    };

    struct InnerNode {

        long dist;
        InnerList children;
        LeafList leaves;

        InnerNode() {
            dist = 0;
            leaves.~ArrayList();
        }

        InnerNode(long d, bool hasLeaf) {
            dist = d;
            if (hasLeaf) {
                children.~ArrayList();
            } else {
                leaves.~ArrayList();
            }
        }

        ~InnerNode() {
            for (long i = 0; i < min(children.size(), leaves.size()); i++) {
                delete(children[i]);
                delete(leaves[i]);
            }
            for (long i = 0; i < children.size(); i++) {
                delete(children[i]);
            }
            for (long i = 0; i < leaves.size(); i++) {
                delete(leaves[i]);
            }
        }
    
        void add(Data &data ,Data *pivots, long len) {
            long i = 0, d = FQT::dist(data, pivots[0]);

            if (len == 1) {
                for (; i < leaves.size() && d < leaves[i]->dist; i++);
                if (i == leaves.size() || d != leaves[i]->dist) {
                    leaves.add(new LeafNode(d), i);
                }
                leaves[i]->data.add(&data);
                return;
            }

            for (; i < children.size() && children[i]->dist < d; i++);
            if (i == children.size() || d != children[i]->dist) {
                children.add(new InnerNode(d, (len == 1 ? true : false)), i);
            }
            children[i]->add(data, pivots + 1, len - 1);
        }
    
        DataList &srch(Data &q, long r, Data *pivots, long len) {
            long d = FQT::dist(q, pivots[0]);
            DataList *list = new DataList;

            if (len == 1) {
                long s = 0, e = leaves.size(), m;
                while (s < e) {
                    m = s + (e - s) / 2;
                    long d2 = leaves[m]->dist;
                    if (d - r > d2) {
                        s = m + 1;
                    } else if (d + r < d2) {
                        e = m;
                    } else {
                        list->addAll(leaves[m]->data);
                        for (long i = m - 1; i > -1 && (d2 = leaves[i]->dist) <= d + r && d2 >= d - r; i--) {
                            list->addAll(leaves[i]->data);
                        }
                        for (long i = m + 1; i < leaves.size() && (d2 = leaves[i]->dist) <= d + r && d2 >= d - r; i++) {
                            list->addAll(leaves[i]->data);
                        }
                        return *list;
                    }
                }
            }

            long s = 0, e = children.size(), m;
            while(s < e) {
                m = s + (e - s) / 2;
                long d2 = children[m]->dist;
                if (d - r > d2) {
                    s = m + 1;
                } else if (d + r < d2) {
                    e = m;
                } else {
                    DataList &temp = children[m]->srch(q, r, pivots + 1, len - 1);
                    list->addAll(temp);
                    delete(&temp);
                    for (long i = m - 1; i > -1 && (d2 = children[i]->dist) <= d + r && d2 >= d - r; i--) {
                        temp = children[i]->srch(q, r, pivots + 1, len - 1);
                        list->addAll(temp);
                        delete(&temp);
                    }
                    for (long i = m + 1; i < children.size() && (d2 = children[i]->dist) <= d + r && d2 >= d - r; i++) {
                        temp = children[i]->srch(q, r, pivots + 1, len - 1);
                        list->addAll(temp);
                        delete(&temp);
                    }
                    return *list;
                }
            }
            return *list;
        }
    
        DataList &NN(Data &q, Data *pivots, long len) {
            for (long r = 1;; r++) {
                DataList &list = srch(q, r, pivots, len);
                if (list.size() != 0) {
                    return list;
                }
                delete(&list);
            }
        }

        void print() {
            for (long i = 0; i < leaves.size(); i++) {
                for (long j = 0; j < leaves[i]->data.size(); j++) {
                    cout << leaves[i]->data[j]->data << endl;
                }
            }
            for (long i = 0; i < children.size(); i++) {
                children[i]->print();
            }
        }
    };

    InnerNode root;
    Data *pivots;
    long height;

    public:

    FQT(Data *p, long len) {
        pivots = new Data[height = len];
        for (long i = 0; i < len; i++) {
            pivots[i] = p[i];
        }
    }

    ~FQT() {
        delete[](pivots);
    }

    inline void add(Data &d) {
        root.add(d, pivots, height);
    }

    inline DataList &search(Data &query, long radius) {
        return root.srch(query, radius, pivots, height);
    }

    inline DataList &nrstNghb(Data &query) {
        return root.NN(query, pivots, height);
    }

    inline void print() {
        root.print();
    }
};

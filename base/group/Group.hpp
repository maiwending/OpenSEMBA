#include "Group.h"

template<typename T>
Group<T>* Group<T>::clone() const {
    return new Group<T>(this->cloneElems());
}

template<typename T>
Group<T>& Group<T>::operator=(VectorPtr<T>& rhs) {
    if (this == &rhs) {
        return *this;
    }
    VectorPtr<T>::operator=(rhs);
    return *this;
}

template<typename T>
Group<T>& Group<T>::operator=(VectorPtr<T>&& rhs) {
    if (this == &rhs) {
        return *this;
    }
    VectorPtr<T>::operator=(std::move(rhs));
    return *this;
}

template<typename T> template<typename T2>
bool Group<T>::emptyOf() const {
    return (sizeOf<T2>() == 0);
}

template<typename T> template<typename T2>
UInt Group<T>::sizeOf() const {
    UInt res = 0;
    for (UInt i = 0; i < this->size(); i++) {
        if(this->get(i)->template is<T2>()) {
            res++;
        }
    }
    return res;
}

template<typename T> template<typename T2>
Group<typename std::conditional<std::is_const<T>::value, const T2, T2>::type>
        Group<T>::getOf() {
    return
        Group<
            typename std::conditional<
                std::is_const<T>::value,
                const T2, T2>::type>(this->get(getElemsOf_<T2>()));
}

template<typename T> template<typename T2>
Group<const T2> Group<T>::getOf() const {
    return Group<const T2>(this->get(getElemsOf_<T2>()));
}

template<typename T> template<class T2>
Group<typename std::conditional<std::is_const<T>::value, const T2, T2>::type>
        Group<T>::getOfOnly() {
    return
        Group<
            typename std::conditional<
                std::is_const<T>::value,
                const T2, T2>::type>(this->get(getElemsOfOnly_<T2>()));
}

template<typename T> template<class T2>
Group<const T2> Group<T>::getOfOnly() const {
    return Group<const T2>(this->get(getElemsOfOnly_<T2>()));
}

template<typename T>
void Group<T>::printInfo() const {
    std::cout << "Number of elements in group: " << this->size() << std::endl;
    for (UInt i = 0; i < this->size(); i++) {
        this->get(i)->printInfo();
    }
}

template<typename T> template<typename T2>
std::vector<UInt> Group<T>::getElemsOf_() const {
    std::vector<UInt> res;
    res.reserve(this->size());
    for (UInt i = 0; i < this->size(); i++) {
        if(this->get(i)->template is<T2>()) {
            res.push_back(i);
        }
    }
    return res;
}

template<typename T> template<typename T2>
std::vector<UInt> Group<T>::getElemsNotOf_() const {
    std::vector<UInt> res;
    res.reserve(this->size());
    for (UInt i = 0; i < this->size(); i++) {
        if(!this->get(i)->template is<T2>()) {
            res.push_back(i);
        }
    }
    return res;
}

template<typename T> template<typename T2>
std::vector<UInt> Group<T>::getElemsOfOnly_() const {
    std::vector<UInt> res;
    res.reserve(this->size());
    for (UInt i = 0; i < this->size(); i++) {
        if(typeid(*this->get(i)) == typeid(T2)) {
            res.push_back(i);
        }
    }
    return res;
}

template<typename T> template<typename T2>
std::vector<UInt> Group<T>::getElemsNotOfOnly_() const {
    std::vector<UInt> res;
    res.reserve(this->size());
    for (UInt i = 0; i < this->size(); i++) {
        if(typeid(*this->get(i)) != typeid(T2)) {
            res.push_back(i);
        }
    }
    return res;
}

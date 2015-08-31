/*
 * Hex8.h
 *
 *  Created on: Sep 24, 2013
 *      Author: luis
 */

#ifndef HEX8_H_
#define HEX8_H_

#include "base/error/ErrorNotImplemented.h"

#include "Volume.h"

class Hexahedron8Base : public virtual VolumeBase {
public:
    Hexahedron8Base() {}
    virtual ~Hexahedron8Base() {}

    inline bool isQuadratic() const { return false; }

    inline UInt numberOfFaces      () const { return 6; }
    inline UInt numberOfVertices   () const { return 8; }
    inline UInt numberOfCoordinates() const { return 8; }

    inline UInt numberOfSideVertices   (const UInt f = 0) const { return 4; }
    inline UInt numberOfSideCoordinates(const UInt f = 0) const { return 4; }
};

template<class T>
class Hexahedron8 : public virtual Volume<T>,
              public virtual Hexahedron8Base {
public:
    Hexahedron8();
    Hexahedron8(const GroupCoordinates<Coordinate<T,3> >&,
          const ElementId id,
          const CoordinateId vId[8],
          const LayerId layerId = LayerId(0),
          const MatId   matId   = MatId(0));
    Hexahedron8(GroupCoordinates<Coordinate<T,3> >&,
          const ElementId id,
          const Box<T,3>& box,
          const LayerId layerId = LayerId(0),
          const MatId   matId   = MatId(0));
    Hexahedron8(const Hexahedron8<T>& rhs);
    virtual ~Hexahedron8();

    DEFINE_CLONE(Hexahedron8<T>);

    bool isStructured(const Grid3&, const Real = Grid3::tolerance) const;

    bool isRegular() const;
    inline bool isCurvedFace(const UInt f) const { return false; }

    const Coordinate<T,3>* getV    (const UInt i) const { return v_[i]; }
    const Coordinate<T,3>* getSideV(const UInt f, const UInt i) const;

    const Coordinate<T,3>* getVertex    (const UInt i) const { return v_[i]; }
    const Coordinate<T,3>* getSideVertex(const UInt f, const UInt i) const;

    vector<const Coordinate<T,3>*> getVertices() const;
    vector<const Coordinate<T,3>*> getSideVertices(const UInt face) const;

    Real getAreaOfFace(const UInt face) const;
    Real getVolume() const;

    void setV(const UInt i, const Coordinate<T,3>*);

    ElemI* toStructured(const GroupCoordinates<CoordI3>&,
                        const Grid3&, const Real = Grid3::tolerance) const;
    ElemR* toUnstructured(const GroupCoordinates<CoordR3>&, const Grid3&) const;

    void printInfo() const;

private:
    const Coordinate<T,3>* v_[8];

    const static Real tolerance;
};

typedef Hexahedron8Base   Hex8;
typedef Hexahedron8<Real> HexR8;
typedef Hexahedron8<Int > HexI8;

#endif /* HEX8_H_ */

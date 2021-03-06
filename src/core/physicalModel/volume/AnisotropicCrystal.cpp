// OpenSEMBA
// Copyright (C) 2015 Salvador Gonzalez Garcia        (salva@ugr.es)
//                    Luis Manuel Diaz Angulo         (lmdiazangulo@semba.guru)
//                    Miguel David Ruiz-Cabello Nuñez (miguel@semba.guru)
//                    Daniel Mateos Romero            (damarro@semba.guru)
//
// This file is part of OpenSEMBA.
//
// OpenSEMBA is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// OpenSEMBA is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenSEMBA. If not, see <http://www.gnu.org/licenses/>.

#include "physicalModel/volume/AnisotropicCrystal.h"

namespace SEMBA {
namespace PhysicalModel {
namespace Volume {

AnisotropicCrystal::AnisotropicCrystal(
        const Id matId,
        const std::string& name,
        const Math::Axis::Local& local,
        const Math::CVecR3& principalAxesRelativePermittivity,
        const Math::Real relativePermeability)
:   Identifiable<Id>(matId),
    PhysicalModel(name),
    Anisotropic(local) {
    principalAxesRelativePermittivity_ = principalAxesRelativePermittivity;
    relativePermeability_ = relativePermeability;
}

AnisotropicCrystal::AnisotropicCrystal(
    const AnisotropicCrystal& rhs)
:   Identifiable<Id>(rhs),
    PhysicalModel(rhs),
    Anisotropic(rhs) {
    principalAxesRelativePermittivity_ =
        rhs.principalAxesRelativePermittivity_;
    relativePermeability_ = rhs.relativePermeability_;
}

AnisotropicCrystal::~AnisotropicCrystal() {

}

const Math::CVecR3
    AnisotropicCrystal::getPrincipalAxesRelativePermittivity() const {
    return principalAxesRelativePermittivity_;
}

Math::Real AnisotropicCrystal::getRelativePermeability() const {
    return relativePermeability_;
}


Math::MatR33 AnisotropicCrystal::getRelPermittivityMatR() const {
    Math::MatR33 local;
    local.setInDiagonal(principalAxesRelativePermittivity_);
    return getLocalAxe().convertToGlobal(local);
}

Math::MatR33 AnisotropicCrystal::getRelPermeabilityMatR() const {
    Math::MatR33 local;
    local.setInDiagonal(Math::CVecR3(relativePermeability_));
    return getLocalAxe().convertToGlobal(local);
}

Math::MatR33 AnisotropicCrystal::getElectricConductivityMat() const {
    return Math::MatR33();
}

Math::MatR33 AnisotropicCrystal::getMagneticConductivityMat() const {
    return Math::MatR33();
}

} /* namespace Volume */
} /* namespace PhysicalModel */
} /* namespace SEMBA */

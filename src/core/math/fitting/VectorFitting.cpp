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

#include "VectorFitting.h"

#ifdef EIGEN_SUPPORT

namespace SEMBA {
namespace Math {
namespace Fitting {

VectorFitting::VectorFitting(
        const std::vector<Sample>& samples) {
}

void VectorFitting::fit() {
}

std::vector<VectorFitting::Sample> VectorFitting::getFittedSamples(
        const std::vector<std::complex<Real> >& frequencies) const {

}

std::vector<std::complex<Real> > VectorFitting::getPoles() {

}

std::vector<std::complex<Real> > VectorFitting::getResidues() {

}

Real VectorFitting::getRMS() {

}

} /* namespace Fitting */
} /* namespace Math */
} /* namespace SEMBA */

#endif


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
#include "gtest/gtest.h"
#include "math/simplex/Line.h"
#include <list>

using namespace SEMBA;
using namespace Math;

template<size_t N>
class MathSimplexLineTest : public ::testing::TestWithParam<size_t> {

};

TEST_P(MathSimplexLineTest, BasicOperations) {
    Simplex::Line<TypeParam> lin;

    Real sum = 0.0;
    std::vector<Real> weights = lin.getWeights();
    for (size_t i = 0; i < weights.size(); ++i) {
        sum += weights[i];
    }
    EXPECT_NEAR(1.0, sum, 1e-8);
}


INSTANTIATE_TEST_CASE_P(Prueba, MathSimplexLineTest, ::testing::Values(1, 2));

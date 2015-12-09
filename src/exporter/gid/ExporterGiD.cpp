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
#include "Exporter.h"

namespace SEMBA {
namespace Exporter {
namespace GiD {

Math::Int numberOfOutputGiD_ = 0;
Math::Int coordCounter_ = 0;
Math::Int elemCounter_ = 0;

const Math::CVecR3 pecColor(255, 0, 0);
const Math::CVecR3 pmcColor(0, 255, 0);
const Math::CVecR3 smaColor(0, 0, 255);
const Math::CVecR3 pmlColor(0, 0, 255);
const Math::CVecR3 sibcColor(100, 0, 100);
const Math::CVecR3 emSourceColor(100, 100, 0);

void Exporter::init_(
        const Data* smb,
        GiD_PostMode mode,
        const std::string& fn) {
    // Sets default values.
    mode_ = mode;
    meshFile_ = 0;
    resultFile_ = 0;
    if (numberOfOutputGiD_ == 0) {
        GiD_PostInit();
    }
    numberOfOutputGiD_++;
    // Opens files.
    deleteExistentOutputFiles();
    switch (mode_) {
    case GiD_PostAscii:
        openPostMeshFile_(fn + ".post.msh");
        break;
    case GiD_PostBinary:
        openPostResultFile_(fn + ".post.res");
        break;
    default:
        throw std::logic_error("Invalid GiD exporting mode.");
    }
    writeMesh_(smb);
}

Exporter::Exporter(const Data* smb, GiD_PostMode mode) :
                Exporter(smb->getFilename()) {
    init_(smb, mode, getFilename());
}

Exporter::Exporter(
        const Data* smb,
        const std::string& fn,
        GiD_PostMode mode) : Exporter(fn) {
    init_(smb, mode, fn);
}

Exporter::~Exporter() {
    switch (mode_) {
    case GiD_PostAscii:
        GiD_fClosePostMeshFile(meshFile_);
        break;
    default:
        GiD_fClosePostResultFile(resultFile_);
    }
    if (numberOfOutputGiD_ == 1) {
        GiD_PostDone();
    }
    numberOfOutputGiD_--;
    GiD_fClosePostMeshFile(meshFile_);
}

void Exporter::writeAllElements_(const Group<const Geometry::ElemR>& elem,
        const std::string& name) {
    writeElements_(elem.getOf<Geometry::NodR>() , name, GiD_Point, 1);
    writeElements_(elem.getOf<Geometry::LinR2>(), name, GiD_Linear, 2);
    writeElements_(elem.getOf<Geometry::Triangle3>() , name, GiD_Triangle, 3);
    writeElements_(elem.getOf<Geometry::QuaR4>(), name, GiD_Quadrilateral, 4);
    writeElements_(elem.getOf<Geometry::Tetrahedron4>() , name, GiD_Tetrahedra, 4);
    writeElements_(elem.getOf<Geometry::HexR8>(), name, GiD_Hexahedra, 8);
}

void Exporter::writeMesh_(const SmbData* smb) {
    const Mesh* inMesh = smb->mesh;
    const GroupPhysicalModels<>* mat = smb->pMGroup;
    const GroupEMSources<>* srcs = smb->emSources;
    const GroupOutRqs<>* oRqs = smb->outputRequests;
    const Grid3* grid = NULL;
    assert(inMesh != NULL);
    assert(mat != NULL);
    const MeshUnstructured* mesh;
    string preName;
    if (inMesh->is<MeshStructured>()) {
        mesh = inMesh->castTo<MeshStructured>()->getMeshUnstructured();
        preName = "str_";
        grid = inMesh->castTo<Grid3>();
    } else {
        mesh = inMesh->castTo<MeshUnstructured>();
        grid = smb->grid;
    }
    // Writes materials.
    const GroupLayers<Layer>& lay = mesh->layers();
    for (UInt i = 0; i < lay.size(); i++) {
        for (UInt j = 0; j < mat->size(); j++) {
            const MatId matId = (*mat)(j)->getId();
            const LayerId layId = lay(i)->getId();
            const string name = preName + (*mat)(j)->getName() + "@" + lay(i)->getName();
            Group<const ElemR> elem = mesh->elems().getMatLayerId(matId, layId);
            writeAllElements_(elem, name);
        }
    }
    // Writes EM Sources.
    if (srcs != NULL) {
        for (UInt i = 0; i < srcs->size(); i++) {
            const EMSourceBase* src =  (*srcs)(i);
            const string name = preName + "EMSource_" + src->getName();
            Group<const ElemR> elem =
                    mesh->elems().getId(src->elems().getIds());
            writeAllElements_(elem, name);
        }
    }
    // Writes output requests.
    if (oRqs != NULL) {
        for (UInt i = 0; i < oRqs->size(); i++) {
            const OutRqBase* oRq = (*oRqs)(i);
            const string name = preName + "OutRq_" + oRq->getName();
            Group<const ElemR> elem =
                    mesh->elems().getId(oRq->elems().getIds());
            writeAllElements_(elem, name);
        }
    }
    // Writes boundaries.
    if (smb->mesherOptions != NULL) {
        for (UInt i = 0; i < 3; i++) {
            for (UInt j = 0; j < 2; j++) {
                CoordR3Group cG;
                const Group<ElemR>& bound =
                        getBoundary(CartesianAxis(i), CartesianBound(j), cG,
                                grid, mesh, smb->mesherOptions);
                string name = getBoundaryName(smb->mesherOptions, i, j);
                writeAllElements_(bound, name);
            }
        }
    }
    // Writes grid.
    CoordR3Group cG;
    Group<ElemR> gridAux = getGridElems(cG, grid);
    writeAllElements_(gridAux, "Grid");
    //
    GiD_FlushPostFile();
    //
    if (inMesh->is<MeshStructured>()) {
        delete mesh;
    }
}

void Exporter::writeElements_(
        const Group<const ElemR>& elem,
        const string& name,
        const GiD_ElementType type,
        const Int nV) {
    if (elem.size() == 0) {
        return;
    }
    UInt tmpCounter = coordCounter_;
    int nId[nV];
    beginMesh(name, GiD_3D, type, nV);
    vector<CVecR3> pos;
    for(UInt i = 0; i < elem.size(); i++) {
        for (Int j = 0; j < nV; j++) {
            pos.push_back(elem(i)->getVertex(j)->pos());
        }
    }
    GroupCoordinates<CoordR3> cG;
    cG.addPos(pos);
    writeCoordinates_(cG);
    beginElements_();
    for (UInt j = 0; j < elem.size(); j++) {
        for (Int k = 0; k < nV; k++) {
            const CoordR3* coordInCG = cG.getPos(elem(j)->getVertex(k)->pos());
            nId[k] = tmpCounter + coordInCG->getId().toUInt();
        }
        writeElement_(++elemCounter_, nId);

    }
    endElements_();
    endMesh_();
}

//
//void
//OutputGiD::writeMaterialsMesh() {
//    vector<vector<UInt> > ids;
//    vector<string> name;
//    // Gets ids of tets with vacuum material.
//    vector<UInt> vacuumIds = smb_->mesh->elem_.getIdsWithMaterialId(0);
//    ids.push_back(smb_->mesh->getTetIds(vacuumIds));
//    name.push_back("Vacuum");
//    // Rest of materials.
//    vector<UInt> volMats = smb_->pMGroup->getVolumicMatIds();
//    for (UInt i = 0; i < volMats.size(); i++) {
//        const UInt matId = volMats[i];
//        vector<UInt> matIds = smb_->mesh->elem_.getIdsWithMaterialId(matId);
//        ids.push_back(smb_->mesh->getTetIds(matIds));
//        name.push_back(smb_->pMGroup->getPMVolumeWithId(matId)->getName());
//    }
//    writeMeshWithIds(ids, name);
//}

//void
//OutputGiD::writeBCMesh() {
//    BCGroup bc(smb_);
//    writeBCMesh(bc.get(Condition::pec), "PEC", pecColor);
//    writeBCMesh(bc.get(Condition::pmc), "PMC", pmcColor);
//    writeBCMesh(bc.get(Condition::sma), "SMA", smaColor);
//    writeBCMesh(bc.get(Condition::sibc), "SIBC", sibcColor);
//    writeBCMesh(bc.get(Condition::emSource), "EM Source", emSourceColor);
//}
//
//void
//OutputGiD::writeBCMesh(
//        const vector<const BoundaryCondition*>& bc,
//        const string& nameIn,
//        const CVecR3& RGB) {
//    Int nV;
//    smb_->mesh->isLinear() ?  nV = 3 :  nV = 6;
//    beginMesh(nameIn, GiD_3D, GiD_Triangle, nV, RGB);
//    static const UInt GiDOrder[6] = {0, 3, 5, 1, 4, 2};
//    beginCoordinates();
//    UInt tmpCounter = coordCounter_;
//    for (UInt j = 0; j < bc.size(); j++) {
//        const UInt id = bc[j]->getCell()->getId();
//        const UInt f = bc[j]->getFace();
//        const Element* e = smb_->mesh->elem_.get(id);
//        for (Int i = 0; i < nV; i++) {
//            CVecR3 pos;
//            if (smb_->mesh->isLinear()) {
//                pos = e->getSideVertex(f,i)->pos();
//            } else {
//                pos = e->getSideV(f,GiDOrder[i])->pos();
//            }
//            writeCoordinates(++coordCounter_, pos);
//        }
//    }
//    endCoordinates();
//    beginElements();
//    Int nId[nV];
//    for (UInt i = 0; i < bc.size(); i++) {
//        for (UInt j = 0; j < (UInt) nV; j++) {
//            nId[j] = ++tmpCounter;
//        }
//        writeElement(++elemCounter_, nId);
//    }
//    endElements();
//    endMesh();
//}


void Exporter::beginMesh(
        const string& name,
        GiD_Dimension dim,
        GiD_ElementType elementType,
        Int nNode,
        const CVecR3& colorRGB) const {
    char *tName;
    tName = new char[name.length() + 1];
    strcpy(tName, name.c_str());
    if (colorRGB == CVecR3(0.0, 0.0, 0.0)) {
        GiD_fBeginMesh(meshFile_, tName, dim, elementType, nNode);
    } else {
        GiD_fBeginMeshColor(meshFile_, tName, dim, elementType, nNode,
                (Real) colorRGB(0), (Real) colorRGB(1), (Real) colorRGB(2));
    }
    delete [] tName;
}

void Exporter::beginResult(
        const string& fieldName,
        const string& timeName,
        const Real time,
        GiD_ResultType resultType,
        GiD_ResultLocation resultLocaltion,
        const string gaussPointType,
        const vector<string>& componentsNames) const {
    char fName[fieldName.length() + 1];
    strcpy(fName, fieldName.c_str());
    char tName[timeName.length() + 1];
    strcpy(tName, timeName.c_str());
    char gpType[gaussPointType.length() + 1];
    strcpy(gpType, gaussPointType.c_str());
    vector<string> cNames = componentsNames;
    const char *compv[cNames.size()];
    for (UInt i = 0; i < cNames.size(); i++) {
        compv[i] = cNames[i].c_str();
        //        compv[i] = new char[cNames[i].length() + 1];
        //        strcpy(compv[i], cNames[i].c_str());
    }
    GiD_fBeginResult(resultFile_, fName, tName, time, resultType,
            getGiDResultLocation_(), gpType, NULL, cNames.size(), compv);
}

void Exporter::openPostMeshFile_(
        const string& filename) {
    char *auxChar;
    auxChar = new char[filename.length() + 1];
    strcpy(auxChar, filename.c_str());
    meshFile_ = GiD_fOpenPostMeshFile(auxChar, mode_);
    delete [] auxChar;
}

void Exporter::openPostResultFile_(
        const string& filename)  {
    char *auxChar;
    auxChar = new char[filename.length() + 1];
    strcpy(auxChar, filename.c_str());
    resultFile_ = GiD_fOpenPostResultFile(auxChar, mode_);
    if (mode_ == GiD_PostBinary) {
        meshFile_ = resultFile_;
    }
    delete [] auxChar;
}

void Exporter::flushPostFile() const {
    if (meshFile_ != 0) {
        GiD_fFlushPostFile(meshFile_);
    }
    if (resultFile_ != 0) {
        GiD_fFlushPostFile(resultFile_);
    }
}

GiD_ResultType Exporter::getGiDResultType_(OutRqBase::Type type) const {
    switch (type) {
    case OutRqBase::electric:
        return GiD_ResultType::GiD_Vector;
    case OutRqBase::magnetic:
        return GiD_ResultType::GiD_Vector;
    default:
        assert(false);
        return GiD_ResultType::GiD_Scalar;
    }
}

GiD_ResultLocation Exporter::getGiDResultLocation_() const {
    return GiD_ResultLocation::GiD_OnNodes;
}

void Exporter::writeCoordinates_(CoordR3Group& cG)  {
    GiD_fBeginCoordinates(meshFile_);
    for (UInt i = 0; i < cG.size(); i++) {
        GiD_fWriteCoordinates(meshFile_, ++coordCounter_,
                (*cG(i))(x), (*cG(i))(y), (*cG(i))(z));
    }
    GiD_fEndCoordinates(meshFile_);
}

void Exporter::writeElement_(Int elemId, int nId[]) const {
    GiD_fWriteElement(meshFile_, elemId, nId);
}

string Exporter::makeValid_(string name) {
    name.erase(remove(name.begin(), name.end(), '\n'), name.end());
    return name;
}

void Exporter::beginCoordinates_() const {
    GiD_fBeginCoordinates(meshFile_);
}

void Exporter::endCoordinates_() const {
    GiD_fEndCoordinates(meshFile_);
}

void Exporter::beginElements_() const {
    GiD_fBeginElements(meshFile_);
}

void Exporter::endElements_() const {
    GiD_fEndElements(meshFile_);
}

void Exporter::endMesh_() const {
    GiD_fEndMesh(meshFile_);
}

}
}
}

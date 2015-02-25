#include "OutputNFDE.h"

const string OutputNFDE::space = " ";

OutputNFDE::OutputNFDE(const NFDEData *nfde)
:	nfde(nfde) {

}

OutputNFDE::~OutputNFDE() {
    output.close();
}

void OutputNFDE::exportNFDE(const string &file,
        bool outputHeaders,
        bool outputEnd) {

    output.exceptions(ifstream::failbit);

    try {
        output.open(file.c_str());
    }
    catch(exception &e) {
        cerr << "ERROR @ OutputNFDE::export()" << endl;
        cerr << "File can't be opened: " << file << endl;
        exit(1);
    }

    time_t rawtime;
    tm 	*timeinfo;
    char 	 buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string date(buffer);

    output << "* " << file << endl;
    output << "* " << date << endl;
    output << endl;

    if(outputHeaders) {
        output << "************************* GENERAL **************************";
        output << endl << endl;
        exportTimeSteps();

        output << "*********************** SPACE STEPS ************************";
        output << endl << endl;
        exportSpaceSteps();

        output << "************************ BACKGROUND ************************";
        output << endl << endl;
        exportBackground();

        output << "******************* BOUNDARY CONDITIONS ********************";
        output << endl << endl;
        exportBoundaries();
    }

    if(!nfde->planeWaveSource.empty()) {

        output << "********************* EXTENDED SOURCES *********************";
        output << endl << endl;
        exportPlaneWaveSource();
    }

    if(!nfde->currentDensitySource.empty() ||
            !nfde->fieldSource.empty()) {

        output << "********************** NODAL SOURCES ***********************";
        output << endl << endl;
        exportCurrentDensitySource();
        exportFieldSource();
    }

    if(!nfde->isotropicBody.empty() ||
            !nfde->isotropicSurf.empty() ||
            !nfde->isotropicLine.empty()) {

        output << "******************* ISOTROPIC MATERIALS ********************";
        output << endl << endl;
        exportIsotropicBody();
        exportIsotropicSurf();
        exportIsotropicLine();
    }

    if(!nfde->anisotropicBody.empty() ||
            !nfde->anisotropicSurf.empty() ||
            !nfde->anisotropicLine.empty()) {

        output << "******************* ANISOTROPIC MATERIALS ******************";
        output << endl << endl;
        exportAnisotropicBody();
        exportAnisotropicSurf();
        exportAnisotropicLine();
    }


    if(!nfde->dispersiveBody.empty() ||
            !nfde->dispersiveSurf.empty() ||
            !nfde->dispersiveLine.empty()) {

        output << "************** FREQUENCY DEPENDENT MATERIALS ***************";
        output << endl << endl;
        exportDispersiveBody();
        exportDispersiveSurf();
        exportDispersiveLine();
    }

    if(!nfde->compositeSurf.empty()) {

        output << "*************** COMPOSITE SURFACE MATERIALS ****************";
        output << endl << endl;
        exportCompositeSurf();
    }

    if(!nfde->thinWire.empty()) {

        output << "************************* THIN WIRES ***********************";
        output << endl << endl;
        exportThinWire();
    }

    if(!nfde->thinGap.empty()) {

        output << "************************* THIN GAPS ************************";
        output << endl << endl;
        exportThinGap();
    }

    if(!nfde->traditionalProbe.empty() ||
            !nfde->newProbe.empty() ||
            !nfde->bulkProbe.empty() ||
            !nfde->sliceProbe.empty()) {
        output << "************************** PROBES **************************";
        output << endl << endl;
        if(!nfde->traditionalProbe.empty()) {
            output << "** TRADITIONAL PROBES **" << endl;
            output << endl;
            exportTraditionalProbe();
        }
        if(!nfde->newProbe.empty()) {
            output << "** NEW PROBES **" << endl;
            output << endl;
            exportNewProbe();
        }
        if(!nfde->bulkProbe.empty()) {
            output << "** BULK CURRENT PROBES **" << endl;
            output << endl;
            exportBulkProbes();
        }
        if(!nfde->sliceProbe.empty()) {
            output << "** SLICE PROBES **" << endl;
            output << endl;
            exportSliceProbes();
        }
    }

    if(outputEnd)
        output << "!END" << endl;
}


inline string OutputNFDE::toString1PNT(const NFDEData::Coords coord) {
    stringstream res;
    res << toString(coord.coords.second) << endl;
    return res.str();
}

inline string OutputNFDE::toString1PNT(const NFDEData::CoordsLine coord) {
    stringstream res;
    res << toString(coord.coords) << endl;
    return res.str();
}

inline string OutputNFDE::toString2PNT(
        const NFDEData::CoordsDir coord,
        int skip) {
    stringstream res;
    res << toString(coord.coords.first);
    switch (coord.dir) {
    case x:
        res << "X";
        break;
    case y:
        res << "Y";
        break;
    case z:
        res << "Z";
        break;
    }
    if (skip != 0) {
        res << " " << skip;
    }
    res << endl;
    res << toString(coord.coords.second) << endl;
    return res.str();
}

inline string OutputNFDE::toString2PNT(const NFDEData::Coords coord) {
    stringstream res;
    res << toString(coord.coords.first) << endl << toString(coord.coords.second)
            << endl;
    return res.str();
}

inline string OutputNFDE::toString(
        const NFDEData::Boundary& boundary,
        const uint d,
        const uint p) {
    stringstream res;
    bool pml = false;
    double lay = 0.0, order = 0.0, ref = 0.0;
    bool sym = false;
    string sp;
    switch (boundary.bctype) {
    case NFDEData::Boundary::Types::PEC:
        res << "PEC";
        break;
    case NFDEData::Boundary::Types::PMC:
        res << "PMC";
        break;
    case NFDEData::Boundary::Types::SYMMETRIC:
        sym = true;
        switch (boundary.bctype) {
        case NFDEData::Boundary::PeriodicTypes::ETAN:
            res << "SYMMETRIC";
            sp = "ETAN";
            break;
        case NFDEData::Boundary::PeriodicTypes::HTAN:
            res << "SYMMETRIC";
            sp = "HTAN";
            break;
        default:
            res << "PEC" << endl;
            sym = false;
            break;
        }
        break;
    case NFDEData::Boundary::Types::PERIODIC:
        res << "PERIODIC";
        break;
    case NFDEData::Boundary::Types::MUR1:
        res << "MUR1";
        break;
    case NFDEData::Boundary::Types::PML:
        res << "PML";
        pml = true;
        lay = boundary.lay;
        order = boundary.order;
        ref = boundary.ref;
        break;
    default:
        res << "PEC";
        break;
    }
    if (p == 2) {
        res << " " << "ALL" << endl;
    } else {
        res << toString(CartesianBound(p)) << endl;
    }
    if (sym) {
        res << sp << endl;
    }
    if (pml) {
        res << lay << " " << order << " " << ref << endl;
    }
    if (d != 3) {
        res << space << toString(CartesianAxis(d));
    }
    return res.str();
}

string OutputNFDE::toString(const NFDEData::CoordsMultiplier coord) {
    stringstream res;
    res << toString(coord.coords.first) << toString(coord.multiplier) << endl;
    res << toString(coord.coords.second) << endl;
    return res.str();
}

string OutputNFDE::toString(CartesianBound pos) {
    switch (pos) {
    case 0:
        return "L";
    case 1:
        return "U";
    default:
        return "";
    }
}

string OutputNFDE::toString(CartesianAxis dir) {
    switch (dir) {
    case x:
        return "X";
    case y:
        return "Y";
    case z:
        return "Z";
    default:
        return "";
    }
}

string OutputNFDE::toString(const NFDEData::MaterialTypes::value mat) {
    switch (mat) {
    case NFDEData::MaterialTypes::METAL:
        return "!!METAL";
    case NFDEData::MaterialTypes::MGMET:
        return "!!MGMET";
    case NFDEData::MaterialTypes::NONME:
        return "!!NONME";
    default:
        cerr << "ERROR @ OutputNFDE: " << "Undefined material type." << endl;
        return "!!NONE";
    }
}

void OutputNFDE::exportTimeSteps() {
    output << "!TIMESTEPS" << endl;
    output << nfde->timesteps.dt << space << nfde->timesteps.nmax << endl;
    output << endl;
}

void OutputNFDE::exportSpaceSteps() {
    output << "!NEWSPACESTEPS" << endl;
    for(int d = 0; d < 3; d++) {
        switch(d) {
        case x:
            output << "!!X";
            break;
        case y:
            output << "!!Y";
            break;
        case z:
            output << "!!Z";
            break;
        }

        if(nfde->spacesteps[d].cons)
            output << "CONS";
        else
            output << "VARI";
        output << endl;
        output << nfde->spacesteps[d].origin << endl;
        output << nfde->spacesteps[d].m << space << nfde->spacesteps[d].n << endl;
        output << nfde->spacesteps[d].d[0] << endl;
        if (!nfde->spacesteps[d].cons) {
            for (uint i = 1; i < nfde->spacesteps[d].d.size(); i++) {
                output << nfde->spacesteps[d].d[i] << endl;
            }
        }
    }
    output << endl;
}

void OutputNFDE::exportBackground() {
    output << "!BACKGROUND" << endl;
    output << "!!LINEAR" << endl;
    output << nfde->background.sigma << space
            << nfde->background.eps << space
            << nfde->background.mu << space
            << nfde->background.sigmam << endl;

    output << endl;
}
void OutputNFDE::exportBoundaries() {
    output << "!BOUNDARY CONDITION" << endl;
    if((nfde->boundaries[x][0].bctype == nfde->boundaries[x][1].bctype) ||
            (nfde->boundaries[x][0].bctype == nfde->boundaries[y][0].bctype) ||
            (nfde->boundaries[x][0].bctype == nfde->boundaries[y][1].bctype) ||
            (nfde->boundaries[x][0].bctype == nfde->boundaries[z][0].bctype) ||
            (nfde->boundaries[x][0].bctype == nfde->boundaries[z][1].bctype)) {
        output << toString(nfde->boundaries[x][0]);
    } else {
        for(int d = 0; d < 3; d++)
            for(int p = 0; p < 2; p++) {
                output << toString(nfde->boundaries[d][p], d, p);
            }
    }
    output << endl;
}

void OutputNFDE::exportPlaneWaveSource() {
    for (uint i = 0; i < nfde->planeWaveSource.size(); i++) {
        const NFDEData::PlaneWaveSource* ent = &nfde->planeWaveSource[i];
        output << "!PLANE WAVE SOURCE" << endl;
        output << ent->filename << endl;
        output << "LOCKED" << endl;
        output << toString2PNT(ent->coords);
        output << ent->theta << space << ent->phi << space
                << ent->alpha << space << ent->beta << endl;
        output << endl;
    }
}

void OutputNFDE::exportCurrentDensitySource() {
    for(uint i = 0; i < nfde->currentDensitySource.size(); i++) {
        const NFDEData::CurrentDensitySource& ent =
                nfde->currentDensitySource[i];
        output << "!CURRENT DENSITY SOURCE" << endl;
        switch(ent.type) {
        case NFDEData::CurrentDensitySource::Types::ELECT:
            output << "!!ELECT" << endl;
            break;
        default:
            output << "!!MAGNE" << endl;
        }
        output << "!!!2PNT" << endl;
        output << ent.filename << endl;
        for(uint j = 0; j < ent.entities.size(); j++)
            output << toString(ent.entities[j]);

        output << endl;
    }
}
void OutputNFDE::exportFieldSource() {
    for(uint i = 0; i < nfde->fieldSource.size(); i++) {
        const NFDEData::FieldSource& ent = nfde->fieldSource[i];
        output << "!CURRENT DENSITY SOURCE" << endl;
        switch(ent.type) {
        case NFDEData::FieldSource::Types::ELECT:
            output << "!!ELECT" << endl;
            break;
        default:
            output << "!!MAGNE" << endl;
        }
        output << "!!!2PNT" << endl;
        output << ent.filename << endl;
        for(uint j = 0; j < ent.entities.size(); j++)
            output << toString(ent.entities[j]);

        output << endl;
    }
}

void OutputNFDE::exportIsotropicBody() {
    for(uint i = 0; i < nfde->isotropicBody.size(); i++) {
        const NFDEData::IsotropicBody* ent = &nfde->isotropicBody[i];
        if(!ent->layer.empty() || !ent->name.empty()) {
            output << "* " << ent->name << "@"<< ent->layer << endl;
        }
        output << "!ISOTROPIC BODY" << endl;
        output << toString(ent->type) << endl;
        output << "!!!1PNT" << endl;
        if(ent->type == NFDEData::MaterialTypes::NONME) {
            output << ent->sigma << space
                    << ent->eps << space
                    << ent->mu << space
                    << ent->sigmam << endl;
        }
        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString1PNT(ent->entities[j]);
        }
        output << endl;
    }
}
void OutputNFDE::exportIsotropicSurf() {
    for(uint i = 0; i < nfde->isotropicSurf.size(); i++) {
        const NFDEData::IsotropicSurf* ent = &nfde->isotropicSurf[i];
        if(!ent->layer.empty() || !ent->name.empty() ) {
            output << "* " << ent->name << "@"<< ent->layer << endl;
        }
        output << "!ISOTROPIC SURFACE" << endl;
        output << toString(ent->type) << endl;
        output << "!!!2PNT" << endl;
        if(ent->type == NFDEData::MaterialTypes::NONME) {
            output << ent->sigma << space << ent->eps << space
                    << ent->mu << space << ent->sigmam << endl;
        }
        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString2PNT(ent->entities[j]);
        }
        output << endl;
    }
}
void OutputNFDE::exportIsotropicLine() {
    for(uint i = 0; i < nfde->isotropicLine.size(); i++) {
        const NFDEData::IsotropicLine* ent = &nfde->isotropicLine[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!ISOTROPIC LINE" << endl;
        output << toString(nfde->isotropicLine[i].type) << endl;
        output << "!!!2PNT" << endl;
        if(ent->type == NFDEData::MaterialTypes::NONME) {
            output << ent->sigma << space
                    << ent->eps << space
                    << ent->mu << space
                    << ent->sigmam << endl;
        }
        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString1PNT(ent->entities[j]);
        }

        output << endl;
    }
}

void OutputNFDE::exportAnisotropicBody() {
    for(uint i = 0; i < nfde->anisotropicBody.size(); i++) {
        const NFDEData::AnisotropicBody* ent = &nfde->anisotropicBody[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!ANISOTROPIC BODY" << endl;
        output << "!!1PNT" << endl;

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->sigma[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->eps[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->mu[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->sigmam[d1][d2] << space;
            output << endl;
        }

        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString1PNT(ent->entities[j]);
        }
        output << endl;
    }
}
void OutputNFDE::exportAnisotropicSurf() {
    for(uint i = 0; i < nfde->anisotropicSurf.size(); i++) {
        const NFDEData::AnisotropicSurf* ent = &nfde->anisotropicSurf[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!ANISOTROPIC SURF" << endl;
        output << "!!2PNT" << endl;

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->sigma[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->eps[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->mu[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->sigmam[d1][d2] << space;
            output << endl;
        }

        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString2PNT(ent->entities[j]);
        }
        output << endl;
    }
}
void OutputNFDE::exportAnisotropicLine() {
    for(uint i = 0; i < nfde->anisotropicLine.size(); i++) {
        const NFDEData::AnisotropicLine* ent = &nfde->anisotropicLine[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!ANISOTROPIC LINE" << endl;
        output << "!!2PNT" << endl;

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->sigma[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->eps[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->mu[d1][d2] << space;
            output << endl;
        }

        for(int d1 = 0; d1 < 3; d1++) {
            for(int d2 = 0; d2 < 3; d2++)
                output << ent->sigmam[d1][d2] << space;
            output << endl;
        }

        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString2PNT(ent->entities[j]);
        }
        output << endl;
    }
}

void OutputNFDE::exportDispersiveBody() {
    for(uint i = 0; i < nfde->dispersiveBody.size(); i++) {
        const NFDEData::DispersiveBody* ent = &nfde->dispersiveBody[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!FREQUENCY DEPENDENT BODY" << endl;
        output << "!!1PNT" << endl;

        output << ent->sigma0 << space
                << ent->epsinf << space
                << ent->muinf << space
                << 0.0 << endl;

        output << ent->K << space << 0 << space << 0 << space << 0 << endl;

        for(int k = 0; k < ent->K; k++)
            output << ent->a[k] << space << ent->b[k] << endl;

        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString1PNT(ent->entities[j]);
        }
        output << endl;
    }
}
void OutputNFDE::exportDispersiveSurf() {
    for(uint i = 0; i < nfde->dispersiveSurf.size(); i++) {
        const NFDEData::DispersiveSurf* ent = &nfde->dispersiveSurf[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!FREQUENCY DEPENDENT SURF" << endl;
        output << "!!2PNT" << endl;

        output << ent->sigma0 << space
                << ent->epsinf << space
                << ent->muinf << space
                << 0.0 << endl;

        output << ent->K << space << 0 << space << 0 << space << 0 << endl;

        for(int k = 0; k < ent->K; k++)
            output << ent->a[k] << space << ent->b[k] << endl;

        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString2PNT(ent->entities[j]);
        }
        output << endl;
    }
}
void OutputNFDE::exportDispersiveLine() {
    for(uint i = 0; i < nfde->dispersiveLine.size(); i++) {
        const NFDEData::DispersiveLine* ent = &nfde->dispersiveLine[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!FREQUENCY DEPENDENT LINE" << endl;
        output << "!!2PNT" << endl;

        output << ent->sigma0 << space
                << ent->epsinf << space
                << ent->muinf << space
                << 0.0 << endl;

        output << ent->K << space << 0 << space << 0 << space << 0 << endl;

        for(int k = 0; k < ent->K; k++)
            output << ent->a[k] << space << ent->b[k] << endl;

        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString2PNT(ent->entities[j]);
        }
        output << endl;
    }
}

void OutputNFDE::exportCompositeSurf() {
    for(uint i = 0; i < nfde->compositeSurf.size(); i++) {
        const NFDEData::CompositeSurf* ent = &nfde->compositeSurf[i];
        if(!ent->layer.empty()) {
            output << "* " << ent->name << "@" << ent->layer << endl;
        }
        output << "!ISOTROPIC SURFACE" << endl;
        output << "!!COMPO" << endl;
        output << "!!!2PNT";
        if(!ent->name.empty()) {
            output << space << ent->name;
        } else {
            output << space << "undefinedName";
        }
        output << space << ent->numberoflayers << endl;
        for (int j = 0; j < ent->numberoflayers; j++) {
            output << ent->sigma[i] << space
                    << ent->eps[i] << space
                    << ent->mu[i] << space
                    << ent->sigmam[i] << space
                    << ent->thk[i] << endl;
        }
        for(uint j = 0; j < ent->entities.size(); j++) {
            output << toString2PNT(ent->entities[j]);
        }
        output << endl;
    }
}

void OutputNFDE::exportThinWire() {
    for(uint i = 0; i < nfde->thinWire.size(); i++) {
        if(nfde->thinWire[i].segments.size() < 1)
            continue;

        if(!nfde->thinWire[i].layer.empty()) {
            output << "* " << nfde->thinWire[i].getNameAtLayer() << endl;
        }
        output << "!NEW THIN WIRE" << endl;
        output << nfde->thinWire[i].rad << space
               << nfde->thinWire[i].res << space
               << nfde->thinWire[i].ind << space;

        switch(nfde->thinWire[i].tl) {
        case NFDEData::ThinWire::Extremes::MATERIAL:
            output << "MATERIAL";
            break;
        case NFDEData::ThinWire::Extremes::SERIES:
            output << "SERIES";
            break;
        case NFDEData::ThinWire::Extremes::PARALLEL:
            output << "PARALLEL";
            break;
        default:
            output << "MATERIAL";
        }

        output << space;

        switch(nfde->thinWire[i].tr) {
        case NFDEData::ThinWire::Extremes::MATERIAL:
            output << "MATERIAL";
            break;
        case NFDEData::ThinWire::Extremes::SERIES:
            output << "SERIES";
            break;
        case NFDEData::ThinWire::Extremes::PARALLEL:
            output << "PARALLEL";
            break;
        default:
            output << "MATERIAL";
        }

        output << endl;

        if ((nfde->thinWire[i].tl == NFDEData::ThinWire::Extremes::SERIES) ||
            (nfde->thinWire[i].tl == NFDEData::ThinWire::Extremes::PARALLEL))

            output << nfde->thinWire[i].rtl << space
            << nfde->thinWire[i].itl << space
            << nfde->thinWire[i].ctl << endl;

        if ((nfde->thinWire[i].tr == NFDEData::ThinWire::Extremes::SERIES) ||
            (nfde->thinWire[i].tr == NFDEData::ThinWire::Extremes::PARALLEL))

            output << nfde->thinWire[i].rtr << space
            << nfde->thinWire[i].itr << space
            << nfde->thinWire[i].ctr << endl;

        output << nfde->thinWire[i].enl << space
                << nfde->thinWire[i].enr << endl;

        for(uint j = 0; j < nfde->thinWire[i].segments.size(); j++) {

            output << nfde->thinWire[i].segments[j].coords(x) << space
                   << nfde->thinWire[i].segments[j].coords(y) << space
                   << nfde->thinWire[i].segments[j].coords(z) << space
                   << toString(nfde->thinWire[i].segments[j].dir) << space
                   << nfde->thinWire[i].segments[j].multiplier << space
                   << nfde->thinWire[i].segments[j].node << endl;

            if(abs(nfde->thinWire[i].segments[j].multiplier) > 1e-4) {
                switch(nfde->thinWire[i].segments[j].srctype) {
                case NFDEData::CoordsWire::Types::CURR:
                    output << "CURR";
                    break;
                case NFDEData::CoordsWire::Types::VOLT:
                    output << "VOLT";
                    break;
                default:
                    output << "CURR";
                }

                output << space << nfde->thinWire[i].segments[j].srcfile << endl;
            }
        }

        output << endl;
    }
}
void OutputNFDE::exportThinGap() {
    for(uint i = 0; i < nfde->thinGap.size(); i++) {
        const NFDEData::ThinGap* ent = &nfde->thinGap[i];
        if(!ent->layer.empty())
            output << "* " << ent->layer << endl;
        output << "!THIN GAP" << endl;

        output << ent->width << endl;

        for(uint j = 0; j < ent->gaps.size(); j++) {
            output << ent->gaps[j].coords(x) << space
                    << ent->gaps[j].coords(y) << space
                    << ent->gaps[j].coords(z) << space
                    << toString(ent->gaps[j].dir) << space
                    << ent->gaps[j].node << endl;
        }
        output << endl;
    }
}

void OutputNFDE::exportTraditionalProbe() {
    // Only supports far field electric probes.
    for (uint i = 0; i < nfde->traditionalProbe.size(); i++) {
        const NFDEData::TraditionalProbe* ent = &nfde->traditionalProbe[i];
        output << "!PROBE" << endl;
        output << "!!FF" << endl;
        output << "* " << probeName(ent->name, ent->layer, ent->log) << endl;
        output << ent->thStart << " " << ent->thEnd << " " << ent->thStep << endl;
        output << ent->phStart << " " << ent->phEnd << " " << ent->phStep << endl;
        output << ent->filename << endl;
        output << toString1PNT(ent->region) << endl;
    }
}
void OutputNFDE::exportNewProbe() {
    for(uint i = 0; i < nfde->newProbe.size(); i++) {
        const NFDEData::NewProbe* ent = &nfde->newProbe[i];
        output << "* " << probeName(ent->name, ent->layer, ent->log) << endl;
        bool time = false;
        bool freq = false;
        bool tran = false;
        output << "!NEW PROBE" << endl;
        output << "!!NUMER" << endl;

        switch(ent->domain) {
        case NFDEData::NewProbe::DomainTypes::TIME:
            output << "!!!TIME" << endl;
            time = true;
            break;
        case NFDEData::NewProbe::DomainTypes::FREQ:
            output << "!!!FREQ" << endl;
            freq = true;
            break;
        case NFDEData::NewProbe::DomainTypes::TRAN:
            output << "!!!TRAN" << endl;
            freq = true;
            tran = true;
            break;
        case NFDEData::NewProbe::DomainTypes::TIFR:
            output << "!!!TIFR" << endl;
            time = true;
            freq = true;
            break;
        case NFDEData::NewProbe::DomainTypes::TITR:
            output << "!!!TITR" << endl;
            time = true;
            freq = true;
            break;
        case NFDEData::NewProbe::DomainTypes::FRTR:
            output << "!!!FRTR" << endl;
            freq = true;
            break;
        case NFDEData::NewProbe::DomainTypes::ALL:
            output << "!!!ALL" << endl;
            time = true;
            freq = true;
            break;
        default:
            output << "!!!TIME" << endl;
            time = true;
            break;
        }

        if(time) {
            output << ent->tstart << space
                    << ent->tstop << space
                    << ent->tstep << endl;
        }

        if(freq) {
            output << ent->fstart << space
                    << ent->fstop << space
                    << ent->fstep << endl;
        }

        if(tran) {
            output << ent->filename << endl;
        }
        for(uint j = 0; j < ent->probes.size(); j++) {
            switch(ent->probes[j].type) {
            case NFDEData::CoordsNewProbe::Types::EX:
                output << "EX";
                break;
            case NFDEData::CoordsNewProbe::Types::EY:
                output << "EY";
                break;
            case NFDEData::CoordsNewProbe::Types::EZ:
                output << "EZ";
                break;
            case NFDEData::CoordsNewProbe::Types::HX:
                output << "HX";
                break;
            case NFDEData::CoordsNewProbe::Types::HY:
                output << "HY";
                break;
            case NFDEData::CoordsNewProbe::Types::HZ:
                output << "HZ";
                break;
            case NFDEData::CoordsNewProbe::Types::IW:
                output << "IW";
                break;
            case NFDEData::CoordsNewProbe::Types::VG:
                output << "VG";
                break;
            default:
                output << "EX";
                break;
            }

            output << space << toString(ent->probes[j].coords) << endl;
        }
        output << endl;
    }
}
void OutputNFDE::exportBulkProbes() {
    for(uint i = 0; i < nfde->bulkProbe.size(); i++) {
        const NFDEData::BulkProbe* ent = &nfde->bulkProbe[i];
        output << "* " << probeName(ent->name, ent->layer, ent->log) << endl;
        output << "!BULK CURRENT PROBE" << endl;
        switch (ent->type) {
        case NFDEData::BulkProbe::Types::ELECT:
            output << "!!ELECT" << endl;
            break;
        case NFDEData::BulkProbe::Types::MAGNE:
            output << "!!MAGNE" << endl;
            break;
        default:
            cerr<< "ERROR @ exportBulkProbe: "
            << "Undefined type." << endl;
            break;
        }
        output << ent->tstart << space << ent->tstop << space <<
                ent->tstep << endl;
        output << toString2PNT(ent->coord, ent->skip) << endl;
        output << endl;
    }
}
void OutputNFDE::exportSliceProbes() {
    for(uint i = 0; i < nfde->sliceProbe.size(); i++) {
        const NFDEData::SliceProbe* ent = &nfde->sliceProbe[i];
        output << "* " << probeName(ent->name, ent->layer, ent->log) << endl;
        output << "!SLICE PROBE" << endl;
        switch (ent->field) {
        case NFDEData::SliceProbe::FieldTypes::BC:
            output << "!!BC" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::BCX:
            output << "!!BCX" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::BCY:
            output << "!!BCY" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::BCZ:
            output << "!!BCZ" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::ME:
            output << "!!ME" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::MH:
            output << "!!MH" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::EX:
            output << "!!EX" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::EY:
            output << "!!EY" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::EZ:
            output << "!!EZ" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::HX:
            output << "!!HX" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::HY:
            output << "!!HY" << endl;
            break;
        case NFDEData::SliceProbe::FieldTypes::HZ:
            output << "!!HZ" << endl;
            break;
        default:
            cerr << "ERROR @ exportSliceProbe: "
            << "Undefined type." << endl;
            break;
        }
        switch(ent->domain) {
        case NFDEData::SliceProbe::DomainTypes::TIME:
            output << "!!!TIME" << endl;
            output << ent->tstart << space
                    << ent->tstop  << space
                    << ent->tstep  << endl;
            break;
        case NFDEData::SliceProbe::DomainTypes::FREQ:
            output << "!!!FREQ" << endl;
            output << ent->fstart << space
                    << ent->fstop  << space
                    << ent->fstep  << endl;
            break;
        case NFDEData::SliceProbe::DomainTypes::TRAN:
            output << "!!!TRAN" << endl;
            output << ent->fstart << space
                    << ent->fstop  << space
                    << ent->fstep  << endl;
            output << ent->filename << endl;
            break;
        default:
            cerr << "ERROR @ exportSliceProbe: "
            << "Undefined type." << endl;
            break;
        }
        output << "CUTAWAY" << endl;
        output << toString1PNT(ent->region) << endl;
    }
}

string
OutputNFDE::probeName(const string& name, const string& layer, const bool log) const {
    string res = name;
    if (!layer.empty()) {
        res += "@" + layer;
    }
    if (log) {
        res += "_log_";
    }
    return res;
}

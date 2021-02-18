#include <set>
#include <cmath>
#include <gmsh.h>
#include <iostream>

int main(int argc, char **argv)
{
    double lc = 0.01;
    gmsh::initialize();

    gmsh::model::add("t13");

    try {
        gmsh::merge("bullfinch.stl");
        std::cout<<"load STL"<<'\n';// For complex geometries, patches can be too complex, too elongated or too
    // large to be parametrized; setting the following option will force the

    } catch(...) {
        std::cout<<"Could not load STL mesh: bye!"<<'\n';
        gmsh::finalize();
        return 0;
    }

    double angle = 40;

    // For complex geometries, patches can be too complex, too elongated or too
    // large to be parametrized; setting the following option will force the
    // creation of patches that are amenable to reparametrization:
    bool forceParametrizablePatches = true;

    // For open surfaces include the boundary edges in the classification process:
    bool includeBoundary = true;

    gmsh::model::mesh::classifySurfaces(angle * M_PI / 180., includeBoundary,forceParametrizablePatches);
    gmsh::model::mesh::createGeometry();
    std::vector<std::pair<int, int> > s;
    gmsh::model::getEntities(s, 2);
    std::vector<int> sl;
    for(auto surf : s) sl.push_back(surf.second);
    int l = gmsh::model::geo::addSurfaceLoop(sl);
    gmsh::model::geo::addVolume({l});

    gmsh::model::geo::synchronize();

    gmsh::model::getEntities(s);
    gmsh::model::mesh::setSize(s,lc);

    gmsh::model::mesh::generate(3);

    std::set<std::string> args(argv, argv + argc);
    if(!args.count("-nopopup")) gmsh::fltk::run();

    gmsh::finalize();
    return 0;
}

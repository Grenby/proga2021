#include <set>
#include <iostream>
#include <gmsh.h>
#include <cmath>



int main(int argc, char **argv)
{

    gmsh::initialize();

    gmsh::model::add("t3");
    double lc = 0.1;
    double angle = M_PI;

    gmsh::model::occ::addTorus(0,0,0,.4,.2,1,angle*2);
    gmsh::model::occ::addCylinder(0,0,0,1,0,0.5,.3);
    std::vector<std::pair<int, int> > ov;
    std::vector<std::vector<std::pair<int, int> > > ovv;
    gmsh::model::occ::cut({{3, 1}}, {{3, 2}}, ov, ovv, 3);
    gmsh::model::occ::synchronize();

    std::vector<std::pair<int, int> > s;
    gmsh::model::getEntities(s);
    gmsh::model::mesh::setSize(s,lc);

    gmsh::model::mesh::generate(3);
    gmsh::write("t3.msh");
    std::set<std::string> args(argv, argv + argc);
    if(!args.count("-nopopup")) gmsh::fltk::run();

    gmsh::finalize();

    return 0;
}

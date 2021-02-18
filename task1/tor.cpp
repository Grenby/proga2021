#include <set>
#include <gmsh.h>
int main(int argc, char **argv)
{

    gmsh::initialize();

    gmsh::model::add("t3");

    double l1 = 0.3;
    double l2 = 0.1;

    double lc = 0.1;
    gmsh::model::geo::addPoint(0, 0, 0, lc, 1);

    gmsh::model::geo::addPoint(l1, 0, 0, lc, 2);
    gmsh::model::geo::addPoint(-l1, 0, 0, lc, 3);

    gmsh::model::geo::addPoint(l2, 0, 0, lc, 4);
    gmsh::model::geo::addPoint(-l2, 0, 0, lc, 5);

    gmsh::model::geo::addPoint((l1+l2)/2, 0, 0, lc, 6);
    gmsh::model::geo::addPoint(-(l1+l2)/2, 0, 0, lc, 7);


    gmsh::model::geo::addCircleArc(2,1,3,1);//2-3
    gmsh::model::geo::addCircleArc(3,1,2,2);//3-2

    gmsh::model::geo::addCircleArc(4,1,5,3);//4-5
    gmsh::model::geo::addCircleArc(5,1,4,4);//5-4

    gmsh::model::geo::addCircleArc(2,6,4,5,0,1,0);//2-4
    gmsh::model::geo::addCircleArc(4,6,2,6,0,1,0);//4-2

    gmsh::model::geo::addCircleArc(3,7,5,7,0,1,0);//3-5
    gmsh::model::geo::addCircleArc(5,7,3,8,0,1,0);//5-3


    gmsh::model::geo::addCurveLoop({1,7,-3,6},1);
    gmsh::model::geo::addCurveLoop({1,-8,-3,-5},2);


    gmsh::model::geo::addCurveLoop({2,5,-4,8},3);
    gmsh::model::geo::addCurveLoop({2,-6,-4,-7},4);


    for(int i=1;i<=4;i++){
        gmsh::model::geo::addSurfaceFilling({i},i);
    }

    gmsh::model::geo::addSurfaceLoop({1, 2, 3, 4}, 1);
    gmsh::model::geo::addVolume({1});


    gmsh::model::geo::remove({{6,0},{7,0}});
    gmsh::model::geo::synchronize();

    gmsh::model::mesh::generate(3);

    gmsh::write("t3.msh");

    std::set<std::string> args(argv, argv + argc);
    if(!args.count("-nopopup")) gmsh::fltk::run();

    gmsh::finalize();

    return 0;
}


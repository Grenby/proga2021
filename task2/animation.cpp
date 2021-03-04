#include <iostream>
#include <cmath>
#include <vector>

//for create folder
#include <sys/stat.h>
//for vtk
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkTetra.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>

#include <gmsh.h>

using namespace std;

//путь к папке в которой будет сохранены .vts
#define FOLDER_NAME "../task2/p"

// Класс расчётной точки
class CalcNode
{
// Класс сетки будет friend-ом точки
    friend class CalcMesh;

protected:
    // Координаты
    double x;
    double y;
    double z;
    // Некая величина, в попугаях
    double smth;
    // Скорость
    double vx0 =0;
    double vy0 =0;
    double vz0 =0;

    double vx,vy,vz;

    double r0 =0;
    double time= 0;

public:
    CalcNode() : x(0.0), y(0.0), z(0.0), smth(0.0), vx0(0.0), vy0(0.0), vz0(0.0){}

    CalcNode(double x, double y, double z, double smth, double vx, double vy, double vz): x(x), y(y), z(z), smth(smth){
        r0=sqrt(x*x+y*y+z*z);
        if(r0 != 0){
            double v = sqrt(vx * vx + vy * vy + vz * vz);
            v = max(.3,v);
            vx0 = -v*x/r0;
            vy0 = -v*y/r0;
            vz0 = -v*z/r0;
        }else{
            vx0 = 0;
            vy0 = 0;
            vz0 = 0;
        }

        this->vx=vx0;
        this->vy=vy0;
        this->vz=vz0;
    }

    void move(double tau) {
        time += tau;

        x += vx * tau;
        y += vy * tau;
        z += vz * tau;

        vx= vx0 * sin(2*time);
        vy= vy0 * sin(3*time);
        vz= vz0 * sin(time);

        smth = sin(time*sqrt(x*x+y*y+z*z));
    }
};

// элемент сетки
class Element{
    friend class CalcMesh;
protected:
    //индексы
    unsigned long nodesIds[4];
};

// Класс расчётной сетки
class CalcMesh{
protected:
    vector<CalcNode> nodes;
    vector<Element> elements;
public:
    CalcMesh(const std::vector<double>& nodesCoords, const std::vector<std::size_t>& tetrsPoints) {

        nodes.resize(nodesCoords.size() / 3);
        for(unsigned int i = 0; i < nodesCoords.size() / 3; i++) {
            double x = nodesCoords[i * 3];
            double y = nodesCoords[i * 3 + 1];
            double z = nodesCoords[i * 3 + 2];
            // Модельная скалярная величина распределена как-то вот так
            double smth = pow(x, 2) + pow(y, 2) + pow(z, 2);
            nodes[i] = CalcNode(x, y, z, smth, 0.0, 0.0, 0.0);
        }

        // Пройдём по элементам в модели gmsh
        elements.resize(tetrsPoints.size() / 4);
        for(unsigned int i = 0; i < tetrsPoints.size() / 4; i++) {
            elements[i].nodesIds[0] = tetrsPoints[i*4] - 1;
            elements[i].nodesIds[1] = tetrsPoints[i*4 + 1] - 1;
            elements[i].nodesIds[2] = tetrsPoints[i*4 + 2] - 1;
            elements[i].nodesIds[3] = tetrsPoints[i*4 + 3] - 1;
        }

    }

    void doTimeStep(double tau) {
        for(auto & node : nodes)
            node.move(tau);
    }

    // запись текущего состояния сетки в снапшот vts
    void snapshot(unsigned int snap_number) {
        vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        vtkSmartPointer<vtkPoints> dumpPoints = vtkSmartPointer<vtkPoints>::New();

        auto smth = vtkSmartPointer<vtkDoubleArray>::New();
        smth->SetName("smth");

        auto vel = vtkSmartPointer<vtkDoubleArray>::New();
        vel->SetName("velocity");
        vel->SetNumberOfComponents(3);

        for(auto & node : nodes) {
            dumpPoints->InsertNextPoint(node.x, node.y, node.z);
            double _vel[3] = {node.vx, node.vy, node.vz};
            vel->InsertNextTuple(_vel);
            smth->InsertNextValue(node.smth);
        }

        // Грузим точки в сетку
        unstructuredGrid->SetPoints(dumpPoints);

        // Присоединяем векторное и скалярное поля к точкам
        unstructuredGrid->GetPointData()->AddArray(vel);
        unstructuredGrid->GetPointData()->AddArray(smth);

        // А теперь пишем, как наши точки объединены в тетраэдры
        for(unsigned int i = 0; i < elements.size(); i++) {
            auto tetra = vtkSmartPointer<vtkTetra>::New();
            tetra->GetPointIds()->SetId( 0, elements[i].nodesIds[0] );
            tetra->GetPointIds()->SetId( 1, elements[i].nodesIds[1] );
            tetra->GetPointIds()->SetId( 2, elements[i].nodesIds[2] );
            tetra->GetPointIds()->SetId( 3, elements[i].nodesIds[3] );
            unstructuredGrid->InsertNextCell(tetra->GetCellType(), tetra->GetPointIds());
        }

        // Создаём снапшот в файле с заданным именем
        string name = FOLDER_NAME;
        string fileName = name+"/tetr3d-step-" + std::to_string(snap_number) + ".vtu";
        vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
        writer->SetFileName(fileName.c_str());
        writer->SetInputData(unstructuredGrid);
        writer->Write();
    }
};

void make_dir(){
    mkdir(FOLDER_NAME, 0777);
}

int gmsh_create(){

    //размер сетки
    double lc = 0.1;

    gmsh::initialize();
    gmsh::model::add("t13");

    try {
        gmsh::merge("../task2/data/bullfinch.stl");
    } catch(...) {
        gmsh::logger::write("Could not load STL mesh: bye!");
        gmsh::finalize();
        return -1;
    }

    // Восстановим геометрию
    double angle = 40;
    bool forceParametrizablePatches = false;
    bool includeBoundary = true;

    gmsh::model::mesh::classifySurfaces(angle * M_PI / 180., includeBoundary, forceParametrizablePatches);
    gmsh::model::mesh::createGeometry();

    // Зададим объём по считанной поверхности
    std::vector<std::pair<int, int> > s;
    gmsh::model::getEntities(s, 2);
    std::vector<int> sl;
    for(auto surf : s) sl.push_back(surf.second);
    int l = gmsh::model::geo::addSurfaceLoop(sl);
    gmsh::model::geo::addVolume({l});

    gmsh::model::getEntities(s);
    gmsh::model::mesh::setSize(s,lc);

    gmsh::model::geo::synchronize();

//    // Зададим мелкость желаемой сетки
//    int f = gmsh::model::mesh::field::add("MathEval");
//    gmsh::model::mesh::field::setString(f, "F", "4");
//    gmsh::model::mesh::field::setAsBackgroundMesh(f);

    gmsh::model::mesh::generate(3);
    return 0;
}

int main()
{
    make_dir();

    // Шаг точек по пространству
    double h = 4.0;
    // Шаг по времени
    double tau = 0.01;

    const unsigned int GMSH_TETR_CODE = 4;

    if (gmsh_create()==-1)return -1;

    // Теперь извлечём из gmsh данные об узлах сетки
    std::vector<double> nodesCoord;
    std::vector<std::size_t> nodeTags;
    std::vector<double> parametricCoord;
    gmsh::model::mesh::getNodes(nodeTags, nodesCoord, parametricCoord);

    // И данные об элементах сетки тоже извлечём, нам среди них нужны только тетраэдры, которыми залит объём
    std::vector<std::size_t>* tetrsNodesTags = nullptr;
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags;
    std::vector<std::vector<std::size_t>> elementNodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, elementNodeTags);

    for(unsigned int i = 0; i < elementTypes.size(); i++) {
        if(elementTypes[i] != GMSH_TETR_CODE)
            continue;
        tetrsNodesTags = &elementNodeTags[i];
    }

    if(tetrsNodesTags == nullptr) {
        cout << "Can not find tetra data. Exiting." << endl;
        gmsh::finalize();
        return -2;
    }

    cout << "The model has " <<  nodeTags.size() << " nodes and " << tetrsNodesTags->size() / 4 << " tetrs." << endl;

    // На всякий случай проверим, что номера узлов идут подряд и без пробелов
    for(int i = 0; i < nodeTags.size(); ++i) {
        // Индексация в gmsh начинается с 1, а не с нуля. Ну штош, значит так.
        assert(i == nodeTags[i] - 1);
    }
    // И ещё проверим, что в тетраэдрах что-то похожее на правду лежит.
    assert(tetrsNodesTags->size() % 4 == 0);

    CalcMesh mesh(nodesCoord, *tetrsNodesTags);

    gmsh::finalize();

    for(unsigned int step = 1; step < 1000; step++) {
        mesh.doTimeStep(tau);
        mesh.snapshot(step);
    }
    return 0;
}
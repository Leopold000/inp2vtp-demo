#ifndef CELLTYPESTRATEGY_H
#define CELLTYPESTRATEGY_H

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

class CellTypeStrategy
{
public:
    virtual ~CellTypeStrategy() {}
    virtual void convert(vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid) = 0;
};

#endif // CELLTYPESTRATEGY_H

// QuadraticTetraStrategy.h
#ifndef QUADRATICTETRAStrategy_H
#define QUADRATICTETRAStrategy_H

#include "CellTypeStrategy.h"
#include <vtkQuadraticTetra.h>
#include <vtkCellArray.h>

class QuadraticTetraStrategy : public CellTypeStrategy
{
public:
    void convert(vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid) override
    {
        // 具体的转换逻辑
        // 例如，将所有单元转换为 vtkQuadraticTetra
        vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkPoints> points = unstructuredGrid->GetPoints();

        // 获取原始的单元和节点
        vtkIdType numCells = unstructuredGrid->GetNumberOfCells();
        for (vtkIdType i = 0; i < numCells; ++i) {
            vtkCell* cell = unstructuredGrid->GetCell(i);
            vtkIdList* idList = cell->GetPointIds();
            vtkIdType numPts = idList->GetNumberOfIds();

            if (numPts == 10) {  // 假设原始单元是线性四面体
                vtkSmartPointer<vtkQuadraticTetra> quadraticTetra = vtkSmartPointer<vtkQuadraticTetra>::New();
                for (vtkIdType j = 0; j < numPts; ++j) {
                    quadraticTetra->GetPointIds()->SetId(j, idList->GetId(j));
                }
                cells->InsertNextCell(quadraticTetra);
            }
        }

        // 创建新的 UnstructuredGrid
        vtkSmartPointer<vtkUnstructuredGrid> newUnstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        newUnstructuredGrid->SetPoints(points);
        newUnstructuredGrid->SetCells(VTK_QUADRATIC_TETRA, cells);

        // 更新原始的 UnstructuredGrid
        unstructuredGrid->DeepCopy(newUnstructuredGrid);
    }
};

#endif // QUADRATICTETRAStrategy_H


// QuadraticQuadStrategy.h
#ifndef QUADRATICQUADStrategy_H
#define QUADRATICQUADStrategy_H

#include "CellTypeStrategy.h"
#include <vtkQuadraticQuad.h>
#include <vtkCellArray.h>

class QuadraticQuadStrategy : public CellTypeStrategy
{
public:
    void convert(vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid) override
    {
        // 具体的转换逻辑
        // 例如，将所有单元转换为 vtkQuadraticQuad
        vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkPoints> points = unstructuredGrid->GetPoints();

        // 获取原始的单元和节点
        vtkIdType numCells = unstructuredGrid->GetNumberOfCells();
        for (vtkIdType i = 0; i < numCells; ++i) {
            vtkCell* cell = unstructuredGrid->GetCell(i);
            vtkIdList* idList = cell->GetPointIds();
            vtkIdType numPts = idList->GetNumberOfIds();

            if (numPts == 10) {  // 假设原始单元是线性四边形
                vtkSmartPointer<vtkQuadraticQuad> quadraticQuad = vtkSmartPointer<vtkQuadraticQuad>::New();
                for (vtkIdType j = 0; j < numPts; ++j) {
                    quadraticQuad->GetPointIds()->SetId(j, idList->GetId(j));
                }
                cells->InsertNextCell(quadraticQuad);
            }
        }

        // 创建新的 UnstructuredGrid
        vtkSmartPointer<vtkUnstructuredGrid> newUnstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        newUnstructuredGrid->SetPoints(points);
        newUnstructuredGrid->SetCells(VTK_QUADRATIC_QUAD, cells);

        // 更新原始的 UnstructuredGrid
        unstructuredGrid->DeepCopy(newUnstructuredGrid);
    }
};

#endif // QUADRATICQUADStrategy_H

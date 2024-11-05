// -*- coding: UTF-8 -*-
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFile>
#include<QTextStream>
#include<QFileDialog>
#include<QDebug>
#include<vtkPoints.h>
#include<vtkQuadraticTetra.h>
#include<vtkUnstructuredGrid.h>
#include<vtkQuadraticQuad.h>
#include<vtkXMLPolyDataWriter.h>
#include<vtkPolyData.h>
#include<vtkCellData.h>
#include<vtkProperty.h>
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initVtkWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::parseInpFile(const QString &filename, QVector<MainWindow::Node> &nodes, QVector<MainWindow::Element> &elements)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    bool readingNodes = false;
    bool readingElements = false;
    QString elementType;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QString lineUpper = line.toUpper();  // 转换为大写

        if (lineUpper.startsWith("*NODE")) {
            readingNodes = true;
            readingElements = false;
            continue;
        } else if (lineUpper.startsWith("*ELEMENT")) {
            readingNodes = false;
            readingElements = true;
            QStringList parts = line.split(",");
            for (const auto &part : parts) {
                if (part.trimmed().startsWith("TYPE=")) {
                    elementType = part.split("=")[1].trimmed();
                    break;
                }
            }
            continue;
        } else if (lineUpper.startsWith("*")) {
            readingNodes = false;
            readingElements = false;
            continue;
        }

        if (readingNodes) {
            QStringList parts = line.split(",", Qt::SkipEmptyParts);
            if (parts.size() >= 3) {  // 二维或三维节点
                Node node;
                node.id = parts[0].toInt();
                node.x = parts[1].toDouble();
                node.y = parts[2].toDouble();
                if (parts.size() == 4) {  // 三维节点
                    node.z = parts[3].toDouble();
                } else {  // 二维节点
                    node.z = 0.0;
                }
                nodes.append(node);
            }
        } else if (readingElements) {
            QStringList parts = line.split(",", Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                Element element;
                element.id = parts[0].toInt();
                element.type = elementType;  // 设置单元类型
                for (int i = 1; i < parts.size(); ++i) {
                    element.nodes.append(parts[i].toInt());
                }
                elements.append(element);
            }
        }
    }

    file.close();
    return true;
}

vtkSmartPointer<vtkUnstructuredGrid> MainWindow::createVTKMesh(const QVector<MainWindow::Node> &nodes, const QVector<MainWindow::Element> &elements)
{
    vtkNew<vtkPoints> points;
    vtkNew<vtkUnstructuredGrid> grid;

    // 添加所有节点
    for (const auto &node : nodes) {
        points->InsertNextPoint(node.x, node.y, node.z);
    }

    // 添加所有单元
    for (const auto &element : elements) {
        if (element.type.toUpper() == "DC3D10"||"C3D10") {
            if (element.nodes.size() != 10) {
                qDebug() << "Invalid number of nodes for DC3D10 element";
                continue;
            }
            vtkNew<vtkQuadraticTetra> tetra;
            for (int i = 0; i < 10; ++i) {
                tetra->GetPointIds()->SetId(i, element.nodes[i] - 1);  // 注意节点ID可能从1开始
            }
            grid->InsertNextCell(tetra->GetCellType(), tetra->GetPointIds());
        } else if (element.type.toUpper() == "CAX8RT") {
            if (element.nodes.size() != 8) {
                qDebug() << "Invalid number of nodes for CAX8RT element";
                continue;
            }
            vtkNew<vtkQuadraticQuad> quad;
            for (int i = 0; i < 8; ++i) {
                quad->GetPointIds()->SetId(i, element.nodes[i] - 1);  // 注意节点ID可能从1开始
            }
            grid->InsertNextCell(quad->GetCellType(), quad->GetPointIds());
        }
        // 这里可以添加其他类型的单元处理
    }

    grid->SetPoints(points);
    return grid;
}

vtkSmartPointer<vtkPolyData> MainWindow::convertToPolyData(vtkUnstructuredGrid *unstructuredGrid)
{
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();

    // 获取顶点数据
    vtkPoints* points = unstructuredGrid->GetPoints();
    polyData->SetPoints(points);

    // 复制单元数据
    polyData->SetPolys(unstructuredGrid->GetCells());


    // 如果有单元数据的属性，也可以复制这些属性
    polyData->GetCellData()->ShallowCopy(unstructuredGrid->GetCellData());

    return polyData;
}
#include <windows.h>
bool MainWindow::writeVTPFile(const QString &filename, vtkSmartPointer<vtkUnstructuredGrid> &grid)
{
    vtkSmartPointer<vtkPolyData> polyData = convertToPolyData(grid);
    // 将 polyData 保存为 VTP 文件

    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();

    writer->SetFileName(filename.toStdString().c_str());
    writer->SetInputData(polyData);

    // 设置输出数据格式为 ASCII
    writer->SetDataModeToAscii();

    writer->Write();

    return true;
}


void MainWindow::on_btn_openInp_clicked()
{
    //路径
    QString curPath=QCoreApplication::applicationDirPath();
    //名称
    QString dlgTitle="打开文件";
    //过滤器打开文本类型
    QString filter="(*.inp);;文本(*.txt);;(*.*)";
    QString fileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if(fileName.isEmpty()){
        return;
    }

    if(parseInpFile(fileName,m_nodes,m_elements)){
        qDebug()<<"读取成功";
        qDebug()<<m_nodes[0].x;
        qDebug()<<m_elements.size();
    }else{
        qDebug()<<"读取失败";
    }

    ui->label_inpCellType->setText("inp网格类型: "+m_elements[0].type);
    ui->label_inpNodeNum->setText("inp节点数量: "+QString::number(m_nodes.size()));
    ui->label_inpCellNodeNum->setText("单个网格节点数量: "+QString::number(m_elements[0].nodes.size()));
    ui->label_inpCellNum->setText("inp网格数量: "+QString::number(m_elements.size()));

    m_inpCellType=m_elements[0].type;


}

void MainWindow::on_btn_renderModel_clicked()
{
    m_UnstructuredGrid = createVTKMesh(m_nodes,m_elements);


//    QString filename = "chamber.vtp";
//    writeVTPFile(filename,m_UnstructuredGrid);

    setDataAndRender(m_UnstructuredGrid);



}

void MainWindow::initVtkWidget()
{
    // 创建 VTK 渲染器
   m_renderer = vtkSmartPointer<vtkRenderer>::New();
   m_window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
   m_window->AddRenderer(m_renderer);
   ui->vtk_widget->SetRenderWindow(m_window);

   // 创建 VTK 交互器
   m_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   m_window->SetInteractor(m_interactor);

   // 设置交互器样式
   vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
   m_interactor->SetInteractorStyle(style);

   // 设置背景颜色
   m_renderer->GradientBackgroundOn();
   m_renderer->SetBackground2(0.1, 0.1, 0.4);
   m_renderer->SetBackground(0.9, 0.9, 0.9);

   // 重置相机
   m_renderer->ResetCamera();

   // 启动交互器
   m_interactor->Initialize();
   m_window->Render();
}

void MainWindow::setDataAndRender(vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid)
{
    // 创建 Mapper
    m_mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    m_mapper->SetInputData(unstructuredGrid);

    // 创建 Actor
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(m_mapper);
    m_actor->GetProperty()->SetRepresentationToSurface();

    // 将 Actor 添加到 Renderer
    m_renderer->AddActor(m_actor);

    // 重置相机
    m_renderer->ResetCamera();

    // 渲染
    m_window->Render();
}

void MainWindow::on_btn_addFieldData_clicked()
{

}

void MainWindow::on_btn_addPointData_clicked()
{

}

void MainWindow::on_btn_saveVtp_clicked()
{
    if(m_UnstructuredGrid){
        //打开对话框，保存文件
         QString fileName = QFileDialog::getSaveFileName(this, tr("Save VTK File"), "", tr("VTK Files (*.vtp)"));
         // 输出文件路径
         qDebug() << "Selected file path:" << fileName.toStdString().c_str();
         if(writeVTPFile(fileName,m_UnstructuredGrid)){
             QMessageBox::information(this,"提示","保存成功");
         }else{
             QMessageBox::critical(this,"警告","保存失败");
         }
    }
}

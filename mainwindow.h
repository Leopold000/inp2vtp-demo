#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#ifndef INITIAL_OPENGL
#define INITIAL_OPENGL
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
#endif

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarRepresentation.h>
#include <vtkScalarBarWidget.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_openInp_clicked();

    void on_btn_renderModel_clicked();

    void on_btn_addFieldData_clicked();

    void on_btn_addPointData_clicked();

    void on_btn_saveVtp_clicked();

private:
    Ui::MainWindow *ui;

private:
    struct Node{
        int id;
        double x,y,z;
    };

    struct Element{
        int id;
        QString type;
        QVector<int> nodes;
    };

    QString m_inpCellType;
    QVector<Node>m_nodes;
    QVector<Element>m_elements;

    //vtk对象
    vtkSmartPointer<vtkUnstructuredGrid> m_UnstructuredGrid;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_window;
    vtkSmartPointer<vtkDataSetMapper> m_mapper;
    vtkSmartPointer<vtkActor>m_actor;
    vtkSmartPointer<vtkRenderWindowInteractor> m_interactor;

    //初始化vtk窗口
    void initVtkWidget();

    //输入渲染数据
    void setDataAndRender(vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid);

    //读取inp文件
    bool parseInpFile(const QString &filename, QVector<Node> &nodes, QVector<Element> &elements);

    //转化为VTK数据结构
    vtkSmartPointer<vtkUnstructuredGrid> createVTKMesh(const QVector<Node> &nodes, const QVector<Element> &elements);

    //将vtkUnstructuredGrid 转换为 vtkPolyData
    vtkSmartPointer<vtkPolyData> convertToPolyData(vtkUnstructuredGrid* unstructuredGrid);
    bool writeVTPFile(const QString &filename, vtkSmartPointer<vtkUnstructuredGrid> &grid);
};
#endif // MAINWINDOW_H

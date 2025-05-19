/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionLoad;
    QAction *actionLoad_Specular;
    QAction *actionLoad_Diffuse;
    QAction *actionLoad_Color;
    QAction *actionLoad_Roughness;
    QAction *actionLoad_Metalness;
    QWidget *Widget;
    QHBoxLayout *horizontalLayout;
    GLWidget *glwidget;
    QVBoxLayout *Configuration;
    QGroupBox *TreeOptions;
    QRadioButton *radio_reflection;
    QRadioButton *radio_iblpbs;
    QDoubleSpinBox *spin_f0r;
    QDoubleSpinBox *spin_f0g;
    QDoubleSpinBox *spin_f0b;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QRadioButton *radio_phong;
    QRadioButton *radio_texMap;
    QComboBox *combo_tex;
    QRadioButton *radio_pbs;
    QCheckBox *check_sky;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QDoubleSpinBox *dspin_rough;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QDoubleSpinBox *dspin_metal;
    QComboBox *comboBox;
    QCheckBox *checkBox;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *AO_params;
    QLabel *K_Radius;
    QDoubleSpinBox *dspin_rough_2;
    QWidget *horizontalLayoutWidget_4;
    QHBoxLayout *AO_params_2;
    QSpinBox *spinBox;
    QLabel *K_Radius_2;
    QWidget *horizontalLayoutWidget_5;
    QHBoxLayout *AO_params_3;
    QSpinBox *spinBox_2;
    QLabel *K_Radius_3;
    QGroupBox *RenderOptions;
    QLabel *Label_NumFaces;
    QLabel *Label_Faces;
    QLabel *Label_Vertices;
    QLabel *Label_NumVertices;
    QLabel *Label_Framerate;
    QLabel *Label_NumFramerate;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(918, 817);
        MainWindow->setMinimumSize(QSize(827, 618));
        MainWindow->setBaseSize(QSize(600, 600));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName("actionQuit");
        actionLoad = new QAction(MainWindow);
        actionLoad->setObjectName("actionLoad");
        actionLoad_Specular = new QAction(MainWindow);
        actionLoad_Specular->setObjectName("actionLoad_Specular");
        actionLoad_Diffuse = new QAction(MainWindow);
        actionLoad_Diffuse->setObjectName("actionLoad_Diffuse");
        actionLoad_Color = new QAction(MainWindow);
        actionLoad_Color->setObjectName("actionLoad_Color");
        actionLoad_Roughness = new QAction(MainWindow);
        actionLoad_Roughness->setObjectName("actionLoad_Roughness");
        actionLoad_Metalness = new QAction(MainWindow);
        actionLoad_Metalness->setObjectName("actionLoad_Metalness");
        Widget = new QWidget(MainWindow);
        Widget->setObjectName("Widget");
        Widget->setMinimumSize(QSize(827, 0));
        Widget->setBaseSize(QSize(600, 600));
        horizontalLayout = new QHBoxLayout(Widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        glwidget = new GLWidget(Widget);
        glwidget->setObjectName("glwidget");
        glwidget->setMinimumSize(QSize(600, 600));
        glwidget->setMaximumSize(QSize(16777215, 16777215));
        glwidget->setBaseSize(QSize(600, 600));

        horizontalLayout->addWidget(glwidget);

        Configuration = new QVBoxLayout();
        Configuration->setSpacing(6);
        Configuration->setObjectName("Configuration");
        TreeOptions = new QGroupBox(Widget);
        TreeOptions->setObjectName("TreeOptions");
        TreeOptions->setMinimumSize(QSize(200, 0));
        TreeOptions->setMaximumSize(QSize(200, 16777215));
        radio_reflection = new QRadioButton(TreeOptions);
        radio_reflection->setObjectName("radio_reflection");
        radio_reflection->setGeometry(QRect(20, 140, 117, 22));
        radio_reflection->setChecked(false);
        radio_iblpbs = new QRadioButton(TreeOptions);
        radio_iblpbs->setObjectName("radio_iblpbs");
        radio_iblpbs->setGeometry(QRect(20, 250, 117, 22));
        spin_f0r = new QDoubleSpinBox(TreeOptions);
        spin_f0r->setObjectName("spin_f0r");
        spin_f0r->setGeometry(QRect(70, 300, 69, 27));
        spin_f0r->setMaximum(2.000000000000000);
        spin_f0r->setSingleStep(0.050000000000000);
        spin_f0r->setValue(0.200000000000000);
        spin_f0g = new QDoubleSpinBox(TreeOptions);
        spin_f0g->setObjectName("spin_f0g");
        spin_f0g->setGeometry(QRect(70, 340, 69, 27));
        spin_f0g->setMaximum(2.000000000000000);
        spin_f0g->setSingleStep(0.050000000000000);
        spin_f0g->setValue(0.200000000000000);
        spin_f0b = new QDoubleSpinBox(TreeOptions);
        spin_f0b->setObjectName("spin_f0b");
        spin_f0b->setGeometry(QRect(80, 380, 69, 27));
        spin_f0b->setMaximum(2.000000000000000);
        spin_f0b->setSingleStep(0.050000000000000);
        spin_f0b->setValue(0.200000000000000);
        label = new QLabel(TreeOptions);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 290, 31, 31));
        label_2 = new QLabel(TreeOptions);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 340, 31, 31));
        label_3 = new QLabel(TreeOptions);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(30, 380, 31, 31));
        radio_phong = new QRadioButton(TreeOptions);
        radio_phong->setObjectName("radio_phong");
        radio_phong->setGeometry(QRect(20, 30, 111, 24));
        radio_phong->setChecked(true);
        radio_texMap = new QRadioButton(TreeOptions);
        radio_texMap->setObjectName("radio_texMap");
        radio_texMap->setGeometry(QRect(20, 60, 151, 24));
        combo_tex = new QComboBox(TreeOptions);
        combo_tex->addItem(QString());
        combo_tex->addItem(QString());
        combo_tex->addItem(QString());
        combo_tex->setObjectName("combo_tex");
        combo_tex->setGeometry(QRect(20, 90, 121, 34));
        radio_pbs = new QRadioButton(TreeOptions);
        radio_pbs->setObjectName("radio_pbs");
        radio_pbs->setGeometry(QRect(20, 160, 112, 23));
        check_sky = new QCheckBox(TreeOptions);
        check_sky->setObjectName("check_sky");
        check_sky->setGeometry(QRect(40, 420, 92, 23));
        check_sky->setChecked(true);
        horizontalLayoutWidget = new QWidget(TreeOptions);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(20, 180, 148, 31));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(horizontalLayoutWidget);
        label_4->setObjectName("label_4");

        horizontalLayout_2->addWidget(label_4);

        dspin_rough = new QDoubleSpinBox(horizontalLayoutWidget);
        dspin_rough->setObjectName("dspin_rough");
        dspin_rough->setMaximum(1.000000000000000);
        dspin_rough->setSingleStep(0.050000000000000);

        horizontalLayout_2->addWidget(dspin_rough);

        horizontalLayoutWidget_2 = new QWidget(TreeOptions);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(10, 210, 160, 31));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(horizontalLayoutWidget_2);
        label_5->setObjectName("label_5");

        horizontalLayout_3->addWidget(label_5);

        dspin_metal = new QDoubleSpinBox(horizontalLayoutWidget_2);
        dspin_metal->setObjectName("dspin_metal");
        dspin_metal->setMaximum(1.000000000000000);
        dspin_metal->setSingleStep(0.050000000000000);

        horizontalLayout_3->addWidget(dspin_metal);

        comboBox = new QComboBox(TreeOptions);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setGeometry(QRect(20, 470, 161, 21));
        comboBox->setEditable(false);
        checkBox = new QCheckBox(TreeOptions);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(40, 440, 141, 26));
        horizontalLayoutWidget_3 = new QWidget(TreeOptions);
        horizontalLayoutWidget_3->setObjectName("horizontalLayoutWidget_3");
        horizontalLayoutWidget_3->setGeometry(QRect(20, 500, 148, 31));
        AO_params = new QHBoxLayout(horizontalLayoutWidget_3);
        AO_params->setSpacing(6);
        AO_params->setContentsMargins(11, 11, 11, 11);
        AO_params->setObjectName("AO_params");
        AO_params->setContentsMargins(0, 0, 0, 0);
        K_Radius = new QLabel(horizontalLayoutWidget_3);
        K_Radius->setObjectName("K_Radius");

        AO_params->addWidget(K_Radius);

        dspin_rough_2 = new QDoubleSpinBox(horizontalLayoutWidget_3);
        dspin_rough_2->setObjectName("dspin_rough_2");
        dspin_rough_2->setMaximum(1.000000000000000);
        dspin_rough_2->setSingleStep(0.050000000000000);
        dspin_rough_2->setValue(0.500000000000000);

        AO_params->addWidget(dspin_rough_2);

        horizontalLayoutWidget_4 = new QWidget(TreeOptions);
        horizontalLayoutWidget_4->setObjectName("horizontalLayoutWidget_4");
        horizontalLayoutWidget_4->setGeometry(QRect(20, 540, 148, 31));
        AO_params_2 = new QHBoxLayout(horizontalLayoutWidget_4);
        AO_params_2->setSpacing(6);
        AO_params_2->setContentsMargins(11, 11, 11, 11);
        AO_params_2->setObjectName("AO_params_2");
        AO_params_2->setContentsMargins(0, 0, 0, 0);
        spinBox = new QSpinBox(horizontalLayoutWidget_4);
        spinBox->setObjectName("spinBox");
        spinBox->setValue(4);

        AO_params_2->addWidget(spinBox);

        K_Radius_2 = new QLabel(horizontalLayoutWidget_4);
        K_Radius_2->setObjectName("K_Radius_2");

        AO_params_2->addWidget(K_Radius_2);

        horizontalLayoutWidget_5 = new QWidget(TreeOptions);
        horizontalLayoutWidget_5->setObjectName("horizontalLayoutWidget_5");
        horizontalLayoutWidget_5->setGeometry(QRect(20, 580, 148, 31));
        AO_params_3 = new QHBoxLayout(horizontalLayoutWidget_5);
        AO_params_3->setSpacing(6);
        AO_params_3->setContentsMargins(11, 11, 11, 11);
        AO_params_3->setObjectName("AO_params_3");
        AO_params_3->setContentsMargins(0, 0, 0, 0);
        spinBox_2 = new QSpinBox(horizontalLayoutWidget_5);
        spinBox_2->setObjectName("spinBox_2");
        spinBox_2->setValue(10);

        AO_params_3->addWidget(spinBox_2);

        K_Radius_3 = new QLabel(horizontalLayoutWidget_5);
        K_Radius_3->setObjectName("K_Radius_3");

        AO_params_3->addWidget(K_Radius_3);


        Configuration->addWidget(TreeOptions);

        RenderOptions = new QGroupBox(Widget);
        RenderOptions->setObjectName("RenderOptions");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RenderOptions->sizePolicy().hasHeightForWidth());
        RenderOptions->setSizePolicy(sizePolicy);
        RenderOptions->setMaximumSize(QSize(200, 80));
        RenderOptions->setBaseSize(QSize(0, 100));
        Label_NumFaces = new QLabel(RenderOptions);
        Label_NumFaces->setObjectName("Label_NumFaces");
        Label_NumFaces->setGeometry(QRect(90, 10, 91, 17));
        Label_Faces = new QLabel(RenderOptions);
        Label_Faces->setObjectName("Label_Faces");
        Label_Faces->setGeometry(QRect(10, 10, 67, 17));
        Label_Vertices = new QLabel(RenderOptions);
        Label_Vertices->setObjectName("Label_Vertices");
        Label_Vertices->setGeometry(QRect(10, 30, 67, 17));
        Label_NumVertices = new QLabel(RenderOptions);
        Label_NumVertices->setObjectName("Label_NumVertices");
        Label_NumVertices->setGeometry(QRect(90, 30, 91, 17));
        Label_Framerate = new QLabel(RenderOptions);
        Label_Framerate->setObjectName("Label_Framerate");
        Label_Framerate->setGeometry(QRect(10, 60, 71, 17));
        Label_NumFramerate = new QLabel(RenderOptions);
        Label_NumFramerate->setObjectName("Label_NumFramerate");
        Label_NumFramerate->setGeometry(QRect(90, 60, 91, 17));

        Configuration->addWidget(RenderOptions);


        horizontalLayout->addLayout(Configuration);

        MainWindow->setCentralWidget(Widget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 918, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad);
        menuFile->addSeparator();
        menuFile->addAction(actionLoad_Specular);
        menuFile->addAction(actionLoad_Diffuse);
        menuFile->addSeparator();
        menuFile->addAction(actionLoad_Color);
        menuFile->addAction(actionLoad_Roughness);
        menuFile->addAction(actionLoad_Metalness);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);

        retranslateUi(MainWindow);
        QObject::connect(glwidget, SIGNAL(SetFaces(QString)), Label_NumFaces, SLOT(setText(QString)));
        QObject::connect(glwidget, SIGNAL(SetVertices(QString)), Label_NumVertices, SLOT(setText(QString)));
        QObject::connect(glwidget, SIGNAL(SetFramerate(QString)), Label_NumFramerate, SLOT(setText(QString)));
        QObject::connect(radio_reflection, SIGNAL(clicked(bool)), glwidget, SLOT(SetReflection(bool)));
        QObject::connect(radio_iblpbs, SIGNAL(clicked(bool)), glwidget, SLOT(SetIBLPBS(bool)));
        QObject::connect(spin_f0b, SIGNAL(valueChanged(double)), glwidget, SLOT(SetFresnelB(double)));
        QObject::connect(spin_f0g, SIGNAL(valueChanged(double)), glwidget, SLOT(SetFresnelG(double)));
        QObject::connect(spin_f0r, SIGNAL(valueChanged(double)), glwidget, SLOT(SetFresnelR(double)));
        QObject::connect(radio_phong, SIGNAL(clicked(bool)), glwidget, SLOT(SetPhong(bool)));
        QObject::connect(radio_texMap, SIGNAL(clicked(bool)), glwidget, SLOT(SetTexMap(bool)));
        QObject::connect(combo_tex, SIGNAL(currentIndexChanged(int)), glwidget, SLOT(SetCurrentTexture(int)));
        QObject::connect(radio_pbs, SIGNAL(clicked(bool)), glwidget, SLOT(SetPBS(bool)));
        QObject::connect(check_sky, SIGNAL(clicked(bool)), glwidget, SLOT(SetSkyVisible(bool)));
        QObject::connect(dspin_metal, SIGNAL(valueChanged(double)), glwidget, SLOT(SetMetalness(double)));
        QObject::connect(dspin_rough, SIGNAL(valueChanged(double)), glwidget, SLOT(SetRoughness(double)));
        QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), glwidget, SLOT(SetCurrentBuffer(int)));
        QObject::connect(checkBox, SIGNAL(clicked(bool)), glwidget, SLOT(Set2StepRenderer(bool)));
        QObject::connect(dspin_rough_2, SIGNAL(valueChanged(double)), glwidget, SLOT(SetRadius(double)));
        QObject::connect(spinBox, SIGNAL(valueChanged(int)), glwidget, SLOT(SetN_Directions(int)));
        QObject::connect(spinBox_2, SIGNAL(valueChanged(int)), glwidget, SLOT(SetN_Samples(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionQuit->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
        actionLoad->setText(QCoreApplication::translate("MainWindow", "Load Model...", nullptr));
        actionLoad_Specular->setText(QCoreApplication::translate("MainWindow", "Load Specular/Sky...", nullptr));
        actionLoad_Diffuse->setText(QCoreApplication::translate("MainWindow", "Load Diffuse...", nullptr));
        actionLoad_Color->setText(QCoreApplication::translate("MainWindow", "Load Color...", nullptr));
        actionLoad_Roughness->setText(QCoreApplication::translate("MainWindow", "Load Roughness...", nullptr));
        actionLoad_Metalness->setText(QCoreApplication::translate("MainWindow", "Load Metalness...", nullptr));
        TreeOptions->setTitle(QCoreApplication::translate("MainWindow", "Options", nullptr));
        radio_reflection->setText(QCoreApplication::translate("MainWindow", "Reflection", nullptr));
        radio_iblpbs->setText(QCoreApplication::translate("MainWindow", "IBL PBS", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "F0 R", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "F0 G", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "F0 B", nullptr));
        radio_phong->setText(QCoreApplication::translate("MainWindow", "Phong", nullptr));
        radio_texMap->setText(QCoreApplication::translate("MainWindow", "Texture Mapping", nullptr));
        combo_tex->setItemText(0, QCoreApplication::translate("MainWindow", "Color", nullptr));
        combo_tex->setItemText(1, QCoreApplication::translate("MainWindow", "Roughness", nullptr));
        combo_tex->setItemText(2, QCoreApplication::translate("MainWindow", "Metalness", nullptr));

        radio_pbs->setText(QCoreApplication::translate("MainWindow", "Simple PBS", nullptr));
        check_sky->setText(QCoreApplication::translate("MainWindow", "Sky Box", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Roughness", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Metalness", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MainWindow", "Albedo", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MainWindow", "Normal", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("MainWindow", "Depth", nullptr));
        comboBox->setItemText(3, QCoreApplication::translate("MainWindow", "SSAO", nullptr));

        comboBox->setPlaceholderText(QCoreApplication::translate("MainWindow", "Albedo", nullptr));
        checkBox->setText(QCoreApplication::translate("MainWindow", "2-step-rendering", nullptr));
        K_Radius->setText(QCoreApplication::translate("MainWindow", "K_Radius", nullptr));
        K_Radius_2->setText(QCoreApplication::translate("MainWindow", "N_directions", nullptr));
        K_Radius_3->setText(QCoreApplication::translate("MainWindow", "N_samples", nullptr));
        RenderOptions->setTitle(QString());
        Label_NumFaces->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        Label_Faces->setText(QCoreApplication::translate("MainWindow", "Faces", nullptr));
        Label_Vertices->setText(QCoreApplication::translate("MainWindow", "Vertices", nullptr));
        Label_NumVertices->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        Label_Framerate->setText(QCoreApplication::translate("MainWindow", "Framerate", nullptr));
        Label_NumFramerate->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H

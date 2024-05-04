#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Mrz_GUI.h"
#include <opencv2/core.hpp>
#include <qgraphicsscene.h>
class Mrz_GUI : public QMainWindow
{
    Q_OBJECT

public:
    Mrz_GUI(QWidget* parent = nullptr);
    ~Mrz_GUI();

    void selectImageAct();
    void processAct();


private:
    Ui::Mrz_GUIClass ui;
    QString last_path_;
    QString current_filename_;
    QGraphicsScene org_scene_;
    QGraphicsScene processed_scene_;

    int error_code_{ 0 };
    void* context_{ nullptr };

    void connectButtons();
    void process(const QString& filename);


};
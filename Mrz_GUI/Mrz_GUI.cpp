#include "Mrz_GUI.h"
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <QGraphicsPixmapItem>
#include <opencv2/imgcodecs.hpp>
#include <mrz_reader_export.h>
#include <qsettings.h>
#include <opencv2/imgproc.hpp>

const QString  CompanyName = "MATIRAN";
const QString AppName = "MrzReader";

Mrz_GUI::Mrz_GUI(QWidget* parent)
    : QMainWindow(parent),
    last_path_{""}
{
    ui.setupUi(this);
    setWindowState(Qt::WindowState::WindowMaximized);
    setWindowTitle("MRZ Reader");

    connectButtons();

    ui.passport_graphicsview->setScene(&org_scene_);
    ui.result_graphicsview->setScene(&processed_scene_);

    context_ = createContextMrz("", &error_code_);

    QSettings settings(CompanyName, AppName);
    current_filename_ = settings.value("lastPath", QDir::currentPath()).toString();
}

Mrz_GUI::~Mrz_GUI()
{
    freeContextMrz(context_);
    QSettings settings(CompanyName, AppName);
    settings.setValue("lastPath", current_filename_);
}

void Mrz_GUI::selectImageAct()
{



    ui.passport_graphicsview->setRenderHint(QPainter::Antialiasing);


    QString fileName = QFileDialog::getOpenFileName(nullptr, "Select Image", current_filename_, "Image Files (*.png *.jpg *.bmp)");

    if (!fileName.isEmpty()) {
        current_filename_ = fileName;
        cv::Mat image = cv::imread(fileName.toStdString(), 1);
     

        QSettings settings(CompanyName, AppName);
        settings.setValue("lastPath", current_filename_);

        if (!image.empty()) {
            last_path_ = QFileInfo(fileName).path();
            cv::Mat view_image;
            cv::cvtColor(image, view_image, cv::COLOR_BGR2RGB);
            QPixmap pixmap = QPixmap::fromImage(QImage(view_image.data, view_image.cols, view_image.rows, view_image.step, QImage::Format_RGB888));
       
            // Use QPixmap in QGraphicsPixmapItem
            QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
            org_scene_.addItem(pixmapItem);




            ui.passport_graphicsview->fitInView(pixmapItem, Qt::KeepAspectRatio);
        }
        else {
            //   qWarning() << "Failed to load image:" << fileName;
        }
    }
    else {
        // qWarning() << "No image selected";
    }

}

void Mrz_GUI::processAct()
{
    if (!current_filename_.isEmpty()) {
        process(current_filename_);
    }
}

void Mrz_GUI::connectButtons()
{
    connect(ui.select_passport_btn, &QPushButton::clicked, this, &Mrz_GUI::selectImageAct);
    connect(ui.process_btn, &QPushButton::clicked, this, &Mrz_GUI::processAct);

}

void Mrz_GUI::process(const QString& filename)
{
    if (context_) {
        cv::Mat image = cv::imread(filename.toStdString(), 1);

        int width = 1300;
        int height = 900;
        cv::Mat dst_image(height, width, CV_8UC3);

        MrzData mrz_data;
        processMrz(image.cols, image.rows, image.step1(0), (char*)image.data, dst_image.cols, dst_image.rows,
            dst_image.step1(0), (char*)dst_image.data,&mrz_data, context_);

        ui.country_edit->setText(mrz_data.country_);
        ui.last_name_edit->setText(mrz_data.last_name_);
        ui.given_name_edit->setText(mrz_data.given_name_);
        ui.document_number_edit->setText(mrz_data.document_number_);
        ui.nationality_edit->setText(mrz_data.nationality_);
        ui.brith_date_edit->setText(mrz_data.brith_date_);
        ui.sex_edit->setText(mrz_data.sex_);
        ui.expiry_date_edit->setText(mrz_data.expiry_date_);

        cv::Mat view_image;
        cv::cvtColor(dst_image, view_image, cv::COLOR_BGR2RGB);
        QPixmap pixmap = QPixmap::fromImage(QImage(view_image.data, view_image.cols, view_image.rows, view_image.step, QImage::Format_RGB888));

        // Use QPixmap in QGraphicsPixmapItem
        QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
        processed_scene_.addItem(pixmapItem);
        ui.result_graphicsview->fitInView(pixmapItem, Qt::KeepAspectRatio);

    }


}

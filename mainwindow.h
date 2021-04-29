#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <opencv2/opencv.hpp>
#include <QList>
#include <QTimer>
#include <QMetaEnum>
#include <QDebug>

#define COUT qDebug()<< __TIME__ << __FUNCTION__ << __LINE__ << " : "

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum PixelFormat
    {
        Format_Invalid,
        Format_ARGB32,
        Format_ARGB32_Premultiplied,
        Format_RGB32,
        Format_RGB24,
        Format_RGB565,
        Format_RGB555,
        Format_ARGB8565_Premultiplied,
        Format_BGRA32,
        Format_BGRA32_Premultiplied,
        Format_BGR32,
        Format_BGR24,
        Format_BGR565,
        Format_BGR555,
        Format_BGRA5658_Premultiplied,

        Format_AYUV444,
        Format_AYUV444_Premultiplied,
        Format_YUV444,
        Format_YUV420P,
        Format_YV12,
        Format_UYVY,
        Format_YUYV,
        Format_NV12,
        Format_NV21,
        Format_IMC1,
        Format_IMC2,
        Format_IMC3,
        Format_IMC4,
        Format_Y8,
        Format_Y16,

        Format_Jpeg,

        Format_CameraRaw,
        Format_AdobeDng,
    };
    Q_ENUM(PixelFormat)

private:
    Ui::MainWindow *ui;
    QList<QCameraInfo> cameras;
    QList<QStringList> camerasresolution;
    QList<QStringList> camerasformat;
    QList<QStringList> camerasframerate;
    QTimer timer;
    QMetaEnum formatmeta;
    cv::VideoCapture cap;
    cv::VideoWriter videorecord;
    bool isSaveImage;
    bool isRecordVideo;
    void CamerasInit();
    void GetCameraList();
    void GetCameraInfo(QCameraInfo info);
    void GetCameraResolution(QCamera *camera);
    void GetCameraFormat(QCamera *camera);
    void GetCameraFrameRate(QCamera *camera);
    void SetComboxValue(int index);
public slots:
    void slot_SetIndexCaptureCamera(int index);
    void slot_CaptureImage();
    void slot_LabelShowImg(cv::Mat image);
    void slot_SetIndexCameraResolution(int index);
    void slot_SetIndexCameraFrameRate(int index);
    void slot_SetCameraBrightness(int value);
    void slot_SetCameraSaturation(int value);
    void slot_SetCameraHue(int value);
    void slot_SetCameraExposure(int value);
    void slot_SetCameraContrast(int value);
    void slot_SetCameraGain(int value);
    void slot_SetCameraWhilteBalance(int value);
    void slot_SetCameraAutoWb(int status);
    void slot_SetCameraAutoExposure(int status);
    void slot_SetCameraPram();
    void slot_Photograph();
    void slot_RecordVideo();
    void slot_SaveVideo(cv::Mat image);

signals:
    void signal_MatImage(cv::Mat);
};

#endif // MAINWINDOW_H

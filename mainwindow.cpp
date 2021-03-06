#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QElapsedTimer>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),isSaveImage(false),isRecordVideo(false)
{
    ui->setupUi(this);
    CamerasInit();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(cap.isOpened())
    {
        timer.stop();
        cap.release();
    }
}

void MainWindow::CamerasInit()
{
    GetCameraList();
    connect(ui->comboBox_name,SIGNAL(activated(int)),this,SLOT(slot_SetIndexCaptureCamera(int)));
    connect(ui->comboBox_resolution,SIGNAL(activated(int)),this,SLOT(slot_SetIndexCameraResolution(int)));
    connect(ui->comboBox_framerate,SIGNAL(activated(int)),this,SLOT(slot_SetIndexCameraFrameRate(int)));
    connect(ui->spinBox_brightness,SIGNAL(valueChanged(int)),this,SLOT(slot_SetCameraBrightness(int)));
    connect(ui->spinBox_contrast,SIGNAL(valueChanged(int)),this,SLOT(slot_SetCameraContrast(int)));
    connect(ui->spinBox_exposure,SIGNAL(valueChanged(int)),this,SLOT(slot_SetCameraExposure(int)));
    connect(ui->spinBox_hue,SIGNAL(valueChanged(int)),this,SLOT(slot_SetCameraHue(int)));
    connect(ui->spinBox_saturation,SIGNAL(valueChanged(int)),this,SLOT(slot_SetCameraSaturation(int)));
    connect(ui->spinBox_gain,SIGNAL(valueChanged(int)),this,SLOT(slot_SetCameraGain(int)));
    connect(ui->spinBox_wb,SIGNAL(valueChanged(int)),this,SLOT(slot_SetCameraWhilteBalance(int)));
    connect(ui->checkBox_autoexposure,&QCheckBox::stateChanged,this,&MainWindow::slot_SetCameraAutoExposure);
    connect(ui->checkBox_autowb,&QCheckBox::stateChanged,this,&MainWindow::slot_SetCameraAutoWb);
    connect(ui->pushButton_setting,&QPushButton::clicked,this,&MainWindow::slot_SetCameraPram);
    connect(ui->pushButton_photo,&QPushButton::clicked,this,&MainWindow::slot_Photograph);
    connect(ui->pushButton_recordvideo,&QPushButton::clicked,this,&MainWindow::slot_RecordVideo);
    connect(this,&MainWindow::signal_MatImage,this,&MainWindow::slot_LabelShowImg);
    connect(this,&MainWindow::signal_MatImage,this,&MainWindow::slot_SaveVideo);
    connect(&timer,&QTimer::timeout,this,&MainWindow::slot_CaptureImage);
}

void MainWindow::GetCameraList()
{
    formatmeta = QMetaEnum::fromType<PixelFormat>();
    cameras = QCameraInfo::availableCameras();
    foreach(const QCameraInfo &cameraInfo, cameras)
    {
        ui->comboBox_name->addItem(cameraInfo.description());
        GetCameraInfo(cameraInfo);
        COUT << cameraInfo.description() <<cameraInfo.deviceName() << cameraInfo.orientation();
    }
}

void MainWindow::GetCameraInfo(QCameraInfo info)
{
    QCamera * camera = new QCamera(info);
    camera->start();
    GetCameraResolution(camera);
    GetCameraFormat(camera);
    GetCameraFrameRate(camera);
    camera->stop();
    delete camera;
    camera = nullptr;
}

void MainWindow::GetCameraResolution(QCamera *camera)
{
    QStringList resolutions;
    QList<QSize> resSize = camera->supportedViewfinderResolutions();
    for(auto it : resSize)
    {
        resolutions.push_back(QString("%1X%2").arg(it.width()).arg(it.height()));
        COUT << QString("%1X%2").arg(it.width()).arg(it.height());
    }
    camerasresolution.push_back(resolutions);
}

void MainWindow::GetCameraFormat(QCamera *camera)
{
    QStringList formats;
    QList<QVideoFrame::PixelFormat> formatlist = camera->supportedViewfinderPixelFormats();

    for(auto it : formatlist)
    {
        COUT << formatmeta.valueToKey(it) ;
        formats.push_back(formatmeta.valueToKey(it));
    }
    camerasformat.push_back(formats);
}

void MainWindow::GetCameraFrameRate(QCamera *camera)
{
    QStringList frames;
    QList<QCamera::FrameRateRange> framerate = camera->supportedViewfinderFrameRateRanges();

    for(auto it : framerate)
    {
        frames.push_back(QString("%1").arg(it.minimumFrameRate));
    }
    camerasframerate.push_back(frames);
}

static int DoubleToInt(double input)
{
    QString out = QString("%1").arg(input);
    return out.toInt();
}

void MainWindow::SetComboxValue(int index)
{
    ui->comboBox_resolution->clear();
    ui->comboBox_resolution->addItems(camerasresolution.at(index));
    QString resolution = QString("%1X%2").arg(cap.get(cv::CAP_PROP_FRAME_WIDTH)).arg(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    ui->comboBox_resolution->setCurrentIndex(ui->comboBox_resolution->findText(resolution));
    COUT << "resolution:" <<resolution;

    ui->comboBox_format->clear();
    ui->comboBox_format->addItems(camerasformat.at(index));
    QString format = QString("%1").arg(cap.get(cv::CAP_PROP_FORMAT));
    COUT << "format:" << format;

    ui->comboBox_framerate->clear();
    ui->comboBox_framerate->addItems(camerasframerate.at(index));
    QString framerate = QString("%1").arg(cap.get(cv::CAP_PROP_FPS));
    ui->comboBox_framerate->setCurrentIndex(ui->comboBox_framerate->findText(framerate));
    COUT << "framerate:" << framerate;

    QString pixelformat = QString("%1").arg(cap.get(cv::CAP_PROP_CODEC_PIXEL_FORMAT));
    COUT << "pixelformat:" << pixelformat;

    QString gain = QString("%1").arg(cap.get(cv::CAP_PROP_GAIN));
    COUT << "gain:" << gain;

    ui->spinBox_brightness->setValue(DoubleToInt(cap.get(cv::CAP_PROP_BRIGHTNESS))); //??????????????????
    ui->spinBox_contrast->setValue(DoubleToInt(cap.get(cv::CAP_PROP_CONTRAST))); //?????????????????????
    ui->spinBox_exposure->setValue(DoubleToInt(cap.get(cv::CAP_PROP_EXPOSURE))); //??????????????????
    ui->spinBox_hue->setValue(DoubleToInt(cap.get(cv::CAP_PROP_HUE))); //??????????????????
    ui->spinBox_saturation->setValue(DoubleToInt(cap.get(cv::CAP_PROP_SATURATION))); //?????????????????????
    ui->spinBox_wb->setValue(DoubleToInt(cap.get(cv::CAP_PROP_WHITE_BALANCE_BLUE_U))); //?????????????????????
    ui->spinBox_gain->setValue(DoubleToInt(cap.get(cv::CAP_PROP_GAIN))); //??????????????????
    ui->checkBox_autoexposure->setCheckState((cap.get(cv::CAP_PROP_AUTO_EXPOSURE))?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
    ui->checkBox_autowb->setCheckState((cap.get(cv::CAP_PROP_AUTO_WB))?Qt::CheckState::Checked:Qt::CheckState::Unchecked);
}

void MainWindow::slot_LabelShowImg(cv::Mat image)
{
    cv::Mat out_image;
    cv::cvtColor(image,out_image,cv::COLOR_BGR2RGB);
    QImage qImg = QImage((const unsigned char*)(out_image.data),out_image.cols,out_image.rows,out_image.step,QImage::Format_RGB888);
    ui->label_video->setPixmap(QPixmap::fromImage(qImg.mirrored(true,false)));
    ui->label_video->resize(qImg.size());
    ui->label_video->show();
    if(isSaveImage){
        QString image_name = QString("%1.png").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"));
        if(qImg.save(image_name))
        {
            COUT << "???????????????" << image_name;
            isSaveImage = false;
        }
        else {
            COUT << "?????????????????????"  << image_name;
        }
    }
}

void MainWindow::slot_SetIndexCameraResolution(int index)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    QString resolution = ui->comboBox_resolution->itemText(index);
    double width = resolution.split("X").at(0).toDouble();
    double height = resolution.split("X").at(1).toDouble();
    COUT << cap.get(cv::CAP_PROP_FRAME_WIDTH) << cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    cap.set(cv::CAP_PROP_FRAME_WIDTH,width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT,height);
}

void MainWindow::slot_SetIndexCameraFrameRate(int index)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    QString framerate = ui->comboBox_framerate->itemText(index);
    cap.set(cv::CAP_PROP_FPS,framerate.toDouble());
}

void MainWindow::slot_SetCameraBrightness(int value)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_BRIGHTNESS,value);
}

void MainWindow::slot_SetCameraSaturation(int value)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_SATURATION,value);
}

void MainWindow::slot_SetCameraHue(int value)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_HUE,value);
}

void MainWindow::slot_SetCameraExposure(int value)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_EXPOSURE,value);
    COUT << "CAP_PROP_EXPOSURE:" << QString("%1").arg(cap.get(cv::CAP_PROP_EXPOSURE));
}

void MainWindow::slot_SetCameraContrast(int value)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_CONTRAST,value);
}

void MainWindow::slot_SetCameraGain(int value)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_GAIN,value);
}

void MainWindow::slot_SetCameraWhilteBalance(int value)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_WHITE_BALANCE_BLUE_U,value);
}

void MainWindow::slot_SetCameraAutoWb(int status)
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_AUTO_WB,(status == Qt::CheckState::Checked) ? -1 : cap.get(cv::CAP_PROP_EXPOSURE));
    ui->spinBox_wb->setDisabled(status);
}

void MainWindow::slot_SetCameraAutoExposure(int status)
{
    COUT << "??????????????????" << status;
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    if(status == Qt::CheckState::Checked)
    {
        cap.set(cv::CAP_PROP_AUTO_EXPOSURE,0);
        cv::VideoCaptureProperties();
    }
    else
    {
        cap.set(cv::CAP_PROP_AUTO_EXPOSURE,1);
        cap.set(cv::CAP_PROP_EXPOSURE,cap.get(cv::CAP_PROP_EXPOSURE));
    }
    ui->spinBox_exposure->setDisabled(status);
    COUT << "???????????????" << cap.get(cv::CAP_PROP_AUTO_EXPOSURE);
    COUT << "?????????" << cap.get(cv::CAP_PROP_EXPOSURE);
}

void MainWindow::slot_SetCameraPram()
{
    COUT << "?????????????????????";
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }
    cap.set(cv::CAP_PROP_SETTINGS, 1);
    COUT << "???????????????" << cap.get(cv::CAP_PROP_AUTO_EXPOSURE);
    COUT << "?????????" << cap.get(cv::CAP_PROP_EXPOSURE);
    COUT << "??????????????????" << cap.get(cv::CAP_PROP_AUTO_WB);
    COUT << "????????????" << cap.get(cv::CAP_PROP_WHITE_BALANCE_BLUE_U);
}

void MainWindow::slot_Photograph()
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }

    isSaveImage = true;
}

void MainWindow::slot_RecordVideo()
{
    if(!cap.isOpened())
    {
        COUT << "???????????????????????????";
        return;
    }

    if(!videorecord.isOpened() && isRecordVideo == false)
    {
        QString resolution = ui->comboBox_resolution->currentText();
        int width = resolution.split("X").at(0).toInt();
        int height = resolution.split("X").at(1).toInt();
        COUT << "video size:" << width << " " << height;
        cv::Size _size = cv::Size(width,height);

        QString video_name = QString("%1.mp4").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"));

        videorecord.open(video_name.toStdString(),cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),30,_size,true);
        if(videorecord.isOpened())
        {
            isRecordVideo = true;
            COUT << "??????????????????";
            ui->pushButton_recordvideo->setText("????????????");
        }
    }
    else if(videorecord.isOpened() && isRecordVideo)
    {
        ui->pushButton_recordvideo->setText("????????????");
        videorecord.release();
        isRecordVideo = false;
        COUT << "??????????????????";
    }
}

void MainWindow::slot_SaveVideo(cv::Mat image)
{
    if(isRecordVideo)
    {
       videorecord.write(image);
    }
}

void MainWindow::slot_SetIndexCaptureCamera(int index)
{
    if(cap.isOpened())
    {
        timer.stop();
        cap.release();
    }
    cap.open(index,cv::CAP_DSHOW);

    if(!cap.isOpened())
    {
        COUT << cameras.at(index).description() << "????????????";
        return;
    }
    SetComboxValue(index);

    timer.start(20);
}

void MainWindow::slot_CaptureImage()
{
    cv::Mat frame,image;
    cap.read(frame);
    if(frame.empty())
        return;
    emit signal_MatImage(frame);
}

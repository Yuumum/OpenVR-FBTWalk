#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QtWidgets/QWidget>
#include "../headers/openvr.h"
#include "../headers/pathtools.h"
namespace Ui {
class OverlayWidget;
}
extern vr::IVRSystem* VRSystem;
extern vr::EVRInitError eError;
extern vr::HmdMatrix34_t orig;
extern bool start;
extern char _ini[1024];
extern char _icon[1024];
extern char Locale[1024];
extern void(*POSE_LOOP)();
extern vr::VROverlayHandle_t m_Handle;
extern vr::HmdMatrix34_t _ORIG;
class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OverlayWidget(QWidget *parent = 0);
    void Tracker_loop();
    void assessment();
    void SetLanguage();
    void RestChaperoneSetup();
    ~OverlayWidget();

 

    vr::HmdMatrix34_t orig;
    bool waist_proofreader = false;

    class readys {
    public:
        bool strat_P = false;
        bool pointC = false;
    };
    class PointC {
    public:
        float x = 0;
        float z = 0;
        bool ready = false;
    };
    class playmod {
    public:
        int mod = 0;
        int speed = 100;
        float limit = 0.0004;
        bool frist = true;
        PointC _POINTC;
        bool smooth = true;
        bool back = false;
    };

    class posecha {
    public:
        float distance_ima;
        float distance_mae;
        float cha; // Moving distance
        float farthest = 0;
        bool back_ok = false;
        int walk_zt = 0; //0 = Still  1 = forward 2 = backward
        bool front_ok = false;
        int cont = 0; // Movement count, greater than 450 equals stop
        float UGOKU3_dis=0; 
        float p_dis = 0; 
        float s_x = 0; 
        float s_z = 0;
        float s_i = 0;
        vr::HmdMatrix34_t strat_P;
        vr::HmdMatrix34_t pointC;
        readys ready;
    };
 
    class Trackers {
    public:
        bool ok = false;
        vr::TrackedDeviceIndex_t waist = -1;
        vr::TrackedDeviceIndex_t footL = -1;
        vr::TrackedDeviceIndex_t footR = -1;
        vr::HmdMatrix34_t lastposeR;
        vr::HmdMatrix34_t lastposeL;
    };

protected:
    bool eventFilter(QObject* watched, QEvent* event);

     

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_7_clicked();
    void on_spinBox_valueChanged(int);
    void on_doubleSpinBox_valueChanged(double);

 

 

public:
    Ui::OverlayWidget *ui;
    void mode_change(int tomod);
private:

 
 
};
extern OverlayWidget::playmod _PLAYMOD;

 


#endif // OVERLAYWIDGET_H

#include "overlaywidget.h"
#include "ui_OpenVR-FBTWalk.h"
#include "../headers/openvr.h"
#include <windows.h>
#include <math.h>
#include <fstream>
#include <thread>
#include "../headers/strtools.h"

#define PI acos(-1)

OverlayWidget::Trackers tracker;

 

OverlayWidget::OverlayWidget(QWidget* parent) :
    QWidget(parent),
     
     ui(new Ui::OverlayWidget)
{
    ui->setupUi(this);
    QImage* img = new QImage;
    img->load("img/2.png");
    ui->label->setPixmap(QPixmap::fromImage(*img));
    img->load("img/3.png");
    ui->label_2->setPixmap(QPixmap::fromImage(*img));

    if (_PLAYMOD.mod == 0) img->load("img/ptb.png");
    else img->load("img/pta.png");
    ui->label_3->setPixmap(QPixmap::fromImage(*img));

    if (_PLAYMOD.mod == 0) img->load("img/pta.png");
    else img->load("img/ptb.png");

    ui->label_4->setPixmap(QPixmap::fromImage(*img));
    img->load("img/4.png");
    ui->label_5->setPixmap(QPixmap::fromImage(*img));
    img->load("img/fh.png");
    ui->label_6->setPixmap(QPixmap::fromImage(*img));
    ui->label_8->setPixmap(QPixmap::fromImage(*img));
    img->load("img/tt.png");
    ui->label_13->setPixmap(QPixmap::fromImage(*img));
    img->load("img/EE.png");
    ui->label_16->setPixmap(QPixmap::fromImage(*img));
    img->load("img/DD.png");
    ui->label_18->setPixmap(QPixmap::fromImage(*img));
    if (_PLAYMOD.smooth) img->load("img/pt2b.png");
    else img->load("img/pt2a.png");
    ui->label_22->setPixmap(QPixmap::fromImage(*img));
    img->load("img/1.png");
    ui->label_25->setPixmap(QPixmap::fromImage(*img));
    img->load("img/R.png");
    ui->label_34->setPixmap(QPixmap::fromImage(*img));
    if (_PLAYMOD.back) img->load("img/pt2b.png");
    else img->load("img/pt2a.png");
    ui->label_23->setPixmap(QPixmap::fromImage(*img));

    ui->label_23->installEventFilter(this);
    ui->label_22->installEventFilter(this);
    ui->label_8->installEventFilter(this);
    ui->label_6->installEventFilter(this);
    ui->label->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    ui->label_4->installEventFilter(this);
    ui->label_2->installEventFilter(this);
 
    ui->spinBox->setValue(_PLAYMOD.speed);
    ui->doubleSpinBox->setValue(_PLAYMOD.limit);
    SetLanguage();
    if (_PLAYMOD.frist) {
        ui->groupBox_3->setGeometry(-10, -10, 1271, 681);
        std::thread t1(&OverlayWidget::Tracker_loop, this);
        t1.detach();
    }
    std::thread t1(&OverlayWidget::ShowRecord_loop, this);
    t1.detach();
}

void OverlayWidget::ShowRecord_loop() {
    char buf[1024];
    while (true)
    {
        Sleep(1000);
        sprintf_s(buf, sizeof(buf), "%d   (%.2f km)", _WALKRECORD.Steps,_WALKRECORD.Distance/1000);
        ui->label_35->setText(buf);
    }
}

void OverlayWidget::Tracker_loop() {
    vr::TrackedDevicePose_t devicePose[vr::k_unMaxTrackedDeviceCount];
    vr::ETrackedDeviceClass trackedDeviceClass;
    char buf[1024];
    vr::ETrackedPropertyError err = vr::TrackedProp_Success;
     

    while (true)
    {
        VRSystem->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseRawAndUncalibrated, 0, devicePose, vr::k_unMaxTrackedDeviceCount);

        for (vr::TrackedDeviceIndex_t ids = 0; ids < vr::k_unMaxTrackedDeviceCount; ids++) {
            trackedDeviceClass = vr::VRSystem()->GetTrackedDeviceClass(ids);
            if (trackedDeviceClass == vr::TrackedDeviceClass_GenericTracker) {
 
                VRSystem->GetStringTrackedDeviceProperty(ids, vr::TrackedDeviceProperty::Prop_ControllerType_String, buf, sizeof(buf), &err);
                std::string str = buf;
                if (str.find("waist", 0) != -1) {
                    tracker.waist = ids;
                    ui->label_26->setStyleSheet("background-color:rgb(38, 189, 38);");
                }
                else if (str.find("left_foot", 0) != -1)
                {
                    tracker.footL = ids;
                    ui->label_28->setStyleSheet("background-color:rgb(38, 189, 38);");
                }
                else if (str.find("right_foot", 0) != -1)
                {
                    ui->label_27->setStyleSheet("background-color:rgb(38, 189, 38);");
                    tracker.footR = ids;
                }
                if (tracker.waist != -1 && tracker.footL != -1 && tracker.footR != -1) {
                    tracker.ok = true;

                    std::string loa = Locale;

                    if (loa.find("zh-", 0) != -1) ui->label_29->setText("追踪器：已就緒");
                    else if(loa.find("ja", 0) != -1) ui->label_29->setText("トラッカー：設置完了");
                    else if (loa.find("ko", 0) != -1) ui->label_29->setText("트래커: 설치 완료");
                    else ui->label_29->setText("Tracker: ready");
                     
                    return;
                    
                }
            }

 
        }
        Sleep(1000);
    }
}
 

void OverlayWidget::SetLanguage() {
    std::string loa = Locale;
 
    if (loa.find("zh-",0) != -1) {
        if (loa.find("CN") == -1) {
            ui->label_7->setText("現在請如左圖所示，站在跑步機後方點擊下方標記按鈕");
            ui->pushButton->setText("標記");
            ui->pushButton_2->setText("開始");
            ui->pushButton_3->setText("選項");
            ui->pushButton_4->setText("重置位置");
            ui->label_10->setText("移動倍率");
            ui->label_19->setText("靈敏度");
            ui->label_20->setText("( 如果無法移動請下調該數值 )");
            ui->label_21->setText("運動平滑");
            ui->label_12->setText("跑步機位置");
            ui->pushButton_5->setText("重新標記");

            ui->label_33->setText("允許後退");
            ui->label_24->setText("現在需要確認您的追踪器是否已就緒，請確保您已在SteamVR→設定→控制器→管理追踪器中設定了相應的位置（腰，左脚，右脚）");
            ui->label_29->setText("追踪器：未就緒");
            ui->pushButton_6->setText("下一步");
            ui->label_30->setText("現在需要評估您的追踪器靈敏度，請保持靜止站立，準備好了請點擊下方【開始評估】");
            ui->pushButton_8->setText("開始評估");
            ui->label_31->setText("靈敏度：");
            ui->pushButton_7->setText("完成");
            ui->pushButton_9->setText("評估");
        }
        else
        {
            ui->label_7->setText("现在请如左图所示, 站在跑步机后方点击下方标记按钮");
            ui->pushButton->setText("标记");
            ui->pushButton_2->setText("开始");
            ui->pushButton_3->setText("选项");
            ui->pushButton_4->setText("重置位置");
            ui->label_10->setText("移动倍率");
            ui->label_19->setText("灵敏度");
            ui->label_20->setText("（如果无法移动请下调该数值）");
            ui->label_21->setText("运动平滑");
            ui->label_12->setText("跑步机位置");
            ui->pushButton_5->setText("重新标记");
            ui->label_33->setText("允许后退");
            ui->label_24->setText("现在需要确认您的追踪器是否已就绪,请确保您已在 SteamVR→设置→控制器→管理追踪器 中设置了相应的位置（腰，左脚，右脚）");
            ui->label_29->setText("追踪器：未就绪");
            ui->pushButton_6->setText("下一步");
            ui->label_30->setText("现在需要评估您的追踪器灵敏度, 请保持静止站立, 准备好了请点击下方【开始评估】");
            ui->pushButton_8->setText("开始评估");
            ui->label_31->setText("灵敏度：");
            ui->pushButton_7->setText("完成");
            ui->pushButton_9->setText("评估");
        }
    }
    else if(loa.find("ja", 0) != -1)
    {
        ui->label_7->setText("左側の画像のようにトレッドミルの後ろに立ち「標記」ポタンを押してください");
        ui->pushButton->setText("標記");
        ui->pushButton_2->setText("スタート");
        ui->pushButton_3->setText("オプション");
        ui->pushButton_4->setText("リセット位置");
        ui->label_10->setText("移動倍率");
        ui->label_19->setText("感度");
        ui->label_20->setText("( 移動できない場合はこの数値を下げてください )");
        ui->label_21->setText("スムージングモード");
        ui->label_12->setText("トレッドミルポジション");
        ui->pushButton_5->setText("標記");

        ui->label_33->setText("後退あり");
        ui->label_24->setText("トラッカーを認識するため ： SteamVR→設定→コントローラ→トラッキングの管理 （腰、左足、右足）に該当する位置が設定されているかを確認してください");
        ui->label_29->setText("トラッカー：認識できません");
        ui->pushButton_6->setText("次へ");
        ui->label_30->setText("トラカーの感度を計算します,５秒ほどかかります計算中はなるべき動かないでください, 準備ができたら【開始】ポタンをクリックしてください");
        ui->pushButton_8->setText("開始");
        ui->label_31->setText("感度：");
        ui->pushButton_7->setText("完了");
        ui->pushButton_9->setText("計算");
    }
    else if (loa.find("ko", 0) != -1)
    {
        ui->label_7->setText("왼쪽화면처럼 트레드밀 뒤에 서서 「표기」 버튼을 눌러주세요");
        ui->pushButton->setText("표기");
        ui->pushButton_2->setText("스타트");
        ui->pushButton_3->setText("옵션");
        ui->pushButton_4->setText("리셋포지션");
        ui->label_10->setText("이동 배율");
        ui->label_19->setText("감도");
        ui->label_20->setText("（이동할 수 없는 경우는 이 수치를 낮춰 주세요）");
        ui->label_21->setText("스무딩 모드");
        ui->label_12->setText("트레드밀 포지션");
        ui->pushButton_5->setText("표기");

        ui->label_33->setText("후퇴(뒤로이동) 있음");
        ui->label_24->setText("트래커를 인식하기 위해서 : SteamVR→설정→컨트롤러→트래킹관리(허리,왼발,오른발)에 해당하는 위치가 설정되어 있는지를 확인해주세요");
        ui->label_29->setText("트래커: 인식할 수 없습니다");
        ui->pushButton_6->setText("다음");
        ui->label_30->setText("트래커의 감도를 계산합니다. 5초 정도 걸리기 때문에 계산 중에는 되도록 움직이지 말아주세요. 준비가 되면 【개시】버튼을 클릭하세요.");
        ui->pushButton_8->setText("개시");
        ui->label_31->setText("감도:");
        ui->pushButton_7->setText("완료");
        ui->pushButton_9->setText("평가 하 다");
    }
    else
    {
        ui->label_7->setText("Please stand behind the treadmill and click the 「Mark」 button below as shown in the figure on the left");
        ui->pushButton->setText("Mark");
        ui->label_10->setText("Moving speed");
        ui->label_19->setText("Sensitivity");
        ui->label_20->setText("（If it cannot be moved, lower the value）");
        ui->label_21->setText("Smooth movement");
        ui->label_12->setText("Treadmill position");
        ui->pushButton_5->setText("Mark");

        ui->label_33->setText("Allow back");
        ui->label_24->setText("Now you need to confirm whether your tracker is ready. Please make sure that you have set the corresponding position (waist, left foot, right foot) in steamvr → settings → controller → management tracker");
        ui->label_29->setText("Tracker: not ready");
        ui->pushButton_6->setText("Next");
        ui->label_30->setText("Now we need to evaluate the sensitivity of your tracker. Please stand still. When you are ready, please click [evaluation] below");
        ui->pushButton_8->setText("evaluation");
        ui->label_31->setText("sensitivity：");
        ui->pushButton_7->setText("Finish");
        ui->pushButton_9->setText("evaluation");
    }


}

OverlayWidget::~OverlayWidget()
{
    delete ui;
}


void OverlayWidget::on_pushButton_6_clicked() {
    if (tracker.ok) {
        ui->groupBox_3->setGeometry(-10, 900, 1271, 681);
        ui->groupBox_4->setGeometry(-10, -10, 1271, 681);
        LPTSTR lpPath = new char[MAX_PATH];

        strcpy(lpPath, _ini);
        WritePrivateProfileString("PLAYMOD", "frist", "1", lpPath);
        delete[] lpPath;
    }
}

void OverlayWidget::on_pushButton_9_clicked() {
    ui->groupBox_4->setGeometry(-10, -10, 1271, 681);
}
 
void OverlayWidget::on_pushButton_8_clicked() {
    std::thread t1(&OverlayWidget::assessment, this);
    t1.detach();
}
void OverlayWidget::on_pushButton_7_clicked() {
    ui->groupBox_4->setGeometry(-10, 900, 1271, 681);
}

void OverlayWidget::assessment() {
    vr::TrackedDevicePose_t devicePose[vr::k_unMaxTrackedDeviceCount];
    vr::ETrackedDeviceClass trackedDeviceClass;
    char buf[1024];
    vr::ETrackedPropertyError err = vr::TrackedProp_Success;
    bool trackerok = false;
    float s_dis = 0;
    float dis = 0;
    float s_i = 0;
    vr::HmdMatrix34_t lastP;
    std::string loa = Locale;
    for (int times = 0; times < 2500; times++) {
        VRSystem->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseRawAndUncalibrated, 0, devicePose, vr::k_unMaxTrackedDeviceCount);
        for (vr::TrackedDeviceIndex_t ids = 0; ids < vr::k_unMaxTrackedDeviceCount; ids++) {
            trackedDeviceClass = vr::VRSystem()->GetTrackedDeviceClass(ids);
            if (trackedDeviceClass == vr::TrackedDeviceClass_GenericTracker) {

                VRSystem->GetStringTrackedDeviceProperty(ids, vr::TrackedDeviceProperty::Prop_ControllerType_String, buf, sizeof(buf), &err);
                std::string str = buf;
                if (str.find("left_foot", 0) != -1)
                {
                    if (!trackerok) {
                        trackerok = true;
                        lastP = devicePose[ids].mDeviceToAbsoluteTracking;
                    }
                    else
                    {
                        dis = sqrt((lastP.m[0][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[0][3]) *
                                (lastP.m[0][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[0][3]) +
                                (lastP.m[2][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[2][3]) *
                                (lastP.m[2][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[2][3]));
                        s_dis = s_dis + dis;
                        s_i = s_i + 1;
 
                        lastP = devicePose[ids].mDeviceToAbsoluteTracking;
                    }
                    
                }

            }


        }
        if (!trackerok) {
             


            if (loa.find("zh-", 0) != -1) ui->label_30->setText("评估失败, 未发现您的追踪器");
            else if (loa.find("ja", 0) != -1) ui->label_30->setText("失敗、トラッカーを認識できません");
            else if (loa.find("ko", 0) != -1) ui->label_30->setText("실패,트래커를 인식하지 못합니다");
            else ui->label_30->setText("Evaluation failed, your tracker was not found");

            LPTSTR lpPath = new char[MAX_PATH];
            strcpy(lpPath, _ini);
            WritePrivateProfileString("PLAYMOD", "frist", "0", lpPath);
            delete[] lpPath;
            return;
        }
        int bfb = (float)((float)times / 2500) * 100;

        if (loa.find("zh-", 0) != -1)  sprintf_s(buf, sizeof(buf), "评估中, 请保持静止不动。已完成：%d %%", bfb);
        else if (loa.find("ja", 0) != -1) sprintf_s(buf, sizeof(buf), "計算中、動かないでください　進度：%d %%", bfb);
        else if (loa.find("ko", 0) != -1) sprintf_s(buf, sizeof(buf), "계산중. 움직이지 마세요. 진도: %d %%", bfb);
        else sprintf_s(buf, sizeof(buf), "Please keep still during the evaluation. Completed: %d %%", bfb);

        ui->label_30->setText(buf);
        sprintf_s(buf, sizeof(buf), "%.5f", s_dis / s_i);
        ui->label_32->setText(buf);
        Sleep(2);
    }

    if (loa.find("zh-", 0) != -1) ui->label_30->setText("评估完成！");
    else if (loa.find("ja", 0) != -1) ui->label_30->setText("計算完了しました！");
    else if (loa.find("ko", 0) != -1) ui->label_30->setText("계산완료했습니다！");
    else ui->label_30->setText("Assessment completed！");

    LPTSTR lpPath = new char[MAX_PATH];
    strcpy(lpPath, _ini);
    sprintf_s(buf, sizeof(buf), "%.5f", s_dis/s_i);
    WritePrivateProfileString("PLAYMOD", "limit", buf, lpPath);
    delete[] lpPath;
    _PLAYMOD.limit = s_dis / s_i;
    ui->doubleSpinBox->setValue((float)s_dis / s_i);
    ui->pushButton_7->setGeometry(980, 580, 231, 51);
}

bool OverlayWidget::eventFilter(QObject* watched, QEvent* event)
{

    if (watched == ui->label_6 &&event->type() == QEvent::MouseButtonPress)
    {
        ui->groupBox->setGeometry(-10,900, 1271, 681);
    }
    else if (watched == ui->label &&event->type() == QEvent::MouseButtonPress)
    {
        mode_change(0);
    }
    else if (watched == ui->label_3 && event->type() == QEvent::MouseButtonPress)
    {
        mode_change(0);
    }
    else if (watched == ui->label_4 && event->type() == QEvent::MouseButtonPress)
    {
        mode_change(1);
    }
    else if (watched == ui->label_2 && event->type() == QEvent::MouseButtonPress)
    {
        mode_change(1);
    }
    else if (watched == ui->label_8 && event->type() == QEvent::MouseButtonPress)
    {
        ui->groupBox_2->setGeometry(-10, 900, 1261, 671);
    }
    else if (watched == ui->label_22 && event->type() == QEvent::MouseButtonPress)
    {
        LPTSTR lpPath = new char[MAX_PATH];

        strcpy(lpPath, _ini);
        QImage* img = new QImage;
        if (_PLAYMOD.smooth) {
            _PLAYMOD.smooth = false;
            img->load("img/pt2a.png");
            ui->label_22->setPixmap(QPixmap::fromImage(*img));
            WritePrivateProfileString("PLAYMOD", "smooth", "1", lpPath);
        }
        else
        {
            _PLAYMOD.smooth = true;
            img->load("img/pt2b.png");
            ui->label_22->setPixmap(QPixmap::fromImage(*img));
            WritePrivateProfileString("PLAYMOD", "smooth", "0", lpPath);
        }
        delete[] lpPath;
    }
    else if (watched == ui->label_23 && event->type() == QEvent::MouseButtonPress) {
        LPTSTR lpPath = new char[MAX_PATH];

        strcpy(lpPath, _ini);
        QImage* img = new QImage;
        if (_PLAYMOD.back) {
            _PLAYMOD.back = false;
            img->load("img/pt2a.png");
            ui->label_23->setPixmap(QPixmap::fromImage(*img));
            WritePrivateProfileString("PLAYMOD", "back", "0", lpPath);
        }
        else
        {
            _PLAYMOD.back = true;
            img->load("img/pt2b.png");
            ui->label_23->setPixmap(QPixmap::fromImage(*img));
            WritePrivateProfileString("PLAYMOD", "back", "1", lpPath);
        }
        delete[] lpPath;
    }
 

    return QWidget::eventFilter(watched, event);
}

void OverlayWidget::mode_change(int tomod) {
    QImage* img = new QImage;
    LPTSTR lpPath = new char[MAX_PATH];

    strcpy(lpPath, _ini);

 
     
    if (tomod==0) {
        img->load("img/ptb.png");
        ui->label_3->setPixmap(QPixmap::fromImage(*img));
        img->load("img/pta.png");
        ui->label_4->setPixmap(QPixmap::fromImage(*img));
        _PLAYMOD.mod = tomod;
        WritePrivateProfileString("PLAYMOD", "mod", "0", lpPath);
    }
    else
    {
        if (_PLAYMOD._POINTC.ready) {
            _PLAYMOD.mod = tomod;
            img->load("img/pta.png");
            ui->label_3->setPixmap(QPixmap::fromImage(*img));
            img->load("img/ptb.png");
            ui->label_4->setPixmap(QPixmap::fromImage(*img));
            WritePrivateProfileString("PLAYMOD", "mod", "1", lpPath);
        }
        else
        {
            ui->groupBox->setGeometry(-10, -10, 1271, 681);
        }
 
    }
    delete[] lpPath;
}
 
void OverlayWidget::on_pushButton_clicked()
{
 
    vr::TrackedDevicePose_t devicePose[vr::k_unMaxTrackedDeviceCount];
    VRSystem->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseRawAndUncalibrated, 0, devicePose, vr::k_unMaxTrackedDeviceCount);
    _PLAYMOD._POINTC.x = devicePose[0].mDeviceToAbsoluteTracking.m[0][3];
    _PLAYMOD._POINTC.z = devicePose[0].mDeviceToAbsoluteTracking.m[2][3];
    _PLAYMOD._POINTC.ready = true;


    LPTSTR lpPath = new char[MAX_PATH];

    strcpy(lpPath, _ini);
 
 
    WritePrivateProfileString("PLAYMOD", "mod", "1", lpPath);
    char buf[1024];
    sprintf_s(buf, sizeof(buf), "%.5f", _PLAYMOD._POINTC.x);
    WritePrivateProfileString("PLAYMOD_POINTC", "x", buf, lpPath);
    sprintf_s(buf, sizeof(buf), "%.5f", _PLAYMOD._POINTC.z);
    WritePrivateProfileString("PLAYMOD_POINTC", "z", buf, lpPath);

    delete[] lpPath;
    // 
    QImage* img = new QImage;
    img->load("img/pta.png");
    ui->label_3->setPixmap(QPixmap::fromImage(*img));
    img->load("img/ptb.png");
    ui->label_4->setPixmap(QPixmap::fromImage(*img));
 
    _PLAYMOD.mod = 1;
    std::string loa = Locale;
    if (loa.find("zh-", 0) != -1) {
        if (loa.find("CN") == -1) ui->label_7->setText("標記成功，請退回至主頁面。");
        else ui->label_7->setText("标记成功，请退回至主页面。");
    }
    else if(loa.find("ja", 0) != -1) ui->label_7->setText("標記完了、メイン画面に戻ってください");
    else if(loa.find("ko", 0) != -1) ui->label_7->setText("표기완료, 메인화면으로 돌아가주세요");
    else ui->label_7->setText("Mark successfully, please return to the main page.");

}

void OverlayWidget::on_pushButton_2_clicked()
{
    std::string loa = Locale;
    if (start) {
        start = false;
        if (loa.find("zh-", 0) != -1) {
            if (loa.find("CN") == -1) ui->pushButton_2->setText("開始");
            else ui->pushButton_2->setText("开始");
        }
        else if (loa.find("ja", 0) != -1) ui->pushButton_2->setText("スタート");
        else if (loa.find("ko", 0) != -1) ui->pushButton_2->setText("스타트");
        else ui->pushButton_2->setText("Start");
    }
    else
    {
        start = true;
        if (loa.find("zh-", 0) != -1) {
            if (loa.find("CN") == -1) ui->pushButton_2->setText("停止");
            else ui->pushButton_2->setText("停止");
        }
        else if (loa.find("ja", 0) != -1) ui->pushButton_2->setText("ストップ");
        else if (loa.find("ko", 0) != -1) ui->pushButton_2->setText("중지");
        else ui->pushButton_2->setText("Stop");
        std::thread t1(POSE_LOOP);
        t1.detach();
    };
}

void OverlayWidget::on_pushButton_4_clicked() {
    vr::HmdMatrix34_t newpose;
    newpose = _ORIG;
    if (start) {
        start = false;
        Sleep(500);
        RestChaperoneSetup();
        Sleep(500);
        on_pushButton_2_clicked();
    }
    else RestChaperoneSetup();
}
void OverlayWidget::on_pushButton_5_clicked() {
    ui->groupBox->setGeometry(-10, -10, 1271, 681);
    std::string loa = Locale;

    if (loa.find("zh-", 0) != -1) {
        if (loa.find("CN") == -1) {
            ui->label_7->setText("現在請如左圖所示，站在跑步機後方點擊下方標記按鈕");
        }
        else
        {
            ui->label_7->setText("现在请如左图所示, 站在跑步机后方点击下方标记按钮");
        }
    }
    else if (loa.find("ja", 0) != -1)
    {
        ui->label_7->setText("左側の画像のようにトレッドミルの後ろに立ち「標記」ポタンを押してください");
    }
    else if (loa.find("ko", 0) != -1)
    {
        ui->label_7->setText("왼쪽화면처럼 트레드밀 뒤에 서서 「표기」 버튼을 눌러주세요");
    }
    else
    {
        ui->label_7->setText("Please stand behind the treadmill and click the 「Mark」 button below as shown in the figure on the left");
    }
}

void OverlayWidget::RestChaperoneSetup() {
    vr::HmdMatrix34_t newpose;
    vr::VRChaperoneSetup()->RevertWorkingCopy();
    vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&orig);
    printf_s("\nRest");
    _ORIG.m[1][3] = orig.m[1][3];
    newpose = _ORIG;
    vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&newpose);
    vr::VRChaperoneSetup()->ShowWorkingSetPreview();
}
void OverlayWidget::on_pushButton_3_clicked() {
    ui->groupBox_2->setGeometry(-10, -10, 1271, 681);
}
 
 

  
void OverlayWidget::on_spinBox_valueChanged(int) {
    LPTSTR lpPath = new char[MAX_PATH];
    int a = ui->spinBox->value();
    strcpy(lpPath, _ini);

    char buf[1024];
    sprintf_s(buf, sizeof(buf), "%d", a);
    WritePrivateProfileString("PLAYMOD", "Walk_speed", buf, lpPath);
    delete[] lpPath;
    _PLAYMOD.speed = a;

}
 
void OverlayWidget::on_doubleSpinBox_valueChanged(double) {
    LPTSTR lpPath = new char[MAX_PATH];
    float a = ui->doubleSpinBox->value();
    strcpy(lpPath, _ini);

    char buf[1024];
    sprintf_s(buf, sizeof(buf), "%.5f", a);
    WritePrivateProfileString("PLAYMOD", "limit", buf, lpPath);
    delete[] lpPath;
    _PLAYMOD.limit = a;

}

 
 
 

 

 
 

 

 
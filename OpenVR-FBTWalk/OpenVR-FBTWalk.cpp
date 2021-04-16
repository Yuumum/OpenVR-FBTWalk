#include "overlaywidget.h"
#include "openvroverlaycontroller.h"
#include <QtWidgets/QApplication>
#include <thread>
#include "../headers/openvr.h"
#include "overlaywidget.h"
#include <ShlObj.h>
#define PI acos(-1)
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }
extern "C" { __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; }
OverlayWidget::posecha PchaL;
OverlayWidget::posecha PchaR;
vr::HmdMatrix34_t lastr;
bool lastok = false;
 
 
float trans(float x) {
    if (x < 0)return x / -1;
    else return x;
}

/*
* Abandoned
*/
void Move(OverlayWidget::posecha& Pcha, vr::HmdMatrix34_t nowpose, vr::HmdMatrix34_t lastpose) {



    vr::HmdMatrix34_t newpose;
    char buf[1024];
    float dis1;
    float dis2;

    //The moving distance is greater than limit
    if (Pcha.cha > _PLAYMOD.limit) {
        Pcha.cont = 0;
        if (!Pcha.ready.pointC) {
            if (Pcha.cha < 0.002) {
                return;
            }
        }

        if (!Pcha.ready.pointC) {
            dis1 = sqrt((nowpose.m[0][3] - Pcha.strat_P.m[0][3]) * (nowpose.m[0][3] - Pcha.strat_P.m[0][3]) + (nowpose.m[2][3] - Pcha.strat_P.m[2][3]) * (nowpose.m[2][3] - Pcha.strat_P.m[2][3]));
            dis2 = sqrt((lastpose.m[0][3] - Pcha.strat_P.m[0][3]) * (lastpose.m[0][3] - Pcha.strat_P.m[0][3]) + (lastpose.m[2][3] - Pcha.strat_P.m[2][3]) * (lastpose.m[2][3] - Pcha.strat_P.m[2][3]));
            if (!Pcha.ready.strat_P) {
                Pcha.strat_P = nowpose;
                Pcha.ready.strat_P = true;
                return;
            }

            if (dis1 - dis2 < 0) {
                Pcha.pointC = Pcha.strat_P;

                if (nowpose.m[0][3] > Pcha.strat_P.m[0][3]) Pcha.pointC.m[0][3] = Pcha.pointC.m[0][3] - (nowpose.m[0][3] - Pcha.strat_P.m[0][3]) * 3;
                else Pcha.pointC.m[0][3] = Pcha.pointC.m[0][3] + (Pcha.strat_P.m[0][3] - nowpose.m[0][3]) * 3;

                if (nowpose.m[2][3] > Pcha.strat_P.m[2][3]) Pcha.pointC.m[2][3] = Pcha.pointC.m[2][3] - (nowpose.m[2][3] - Pcha.strat_P.m[2][3]) * 3;
                else Pcha.pointC.m[2][3] = Pcha.pointC.m[2][3] + (Pcha.strat_P.m[2][3] - nowpose.m[2][3]) * 3;

                sprintf_s(buf, sizeof(buf), "\n pointC 建立  x = %.5f, y = %.5f", Pcha.pointC.m[0][3], Pcha.pointC.m[2][3]);
                printf_s(buf);
                Pcha.ready.pointC = true;
            }
        }



        if (Pcha.ready.pointC) {
            Pcha.distance_ima =
                sqrt((nowpose.m[0][3] - Pcha.pointC.m[0][3]) * (nowpose.m[0][3] - Pcha.pointC.m[0][3]) +
                    (nowpose.m[2][3] - Pcha.pointC.m[2][3]) * (nowpose.m[2][3] - Pcha.pointC.m[2][3]));

            Pcha.distance_mae =
                sqrt((lastpose.m[0][3] - Pcha.pointC.m[0][3]) * (lastpose.m[0][3] - Pcha.pointC.m[0][3]) +
                    (lastpose.m[2][3] - Pcha.pointC.m[2][3]) * (lastpose.m[2][3] - Pcha.pointC.m[2][3]));

            if (Pcha.distance_ima - Pcha.distance_mae < 0) {
                orig.m[0][3] = orig.m[0][3] + (nowpose.m[0][3] - lastpose.m[0][3]) * ((float)_PLAYMOD.speed / 100);
                orig.m[2][3] = orig.m[2][3] + (nowpose.m[2][3] - lastpose.m[2][3]) * ((float)_PLAYMOD.speed / 100);
                newpose = orig;
                sprintf_s(buf, sizeof(buf), "\n %s, ima = %.5f, mae = %.5f, cha = %.5f", "靠近", Pcha.distance_ima, Pcha.distance_mae, Pcha.cha);
                printf_s(buf);
                vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&newpose);
                vr::VRChaperoneSetup()->ShowWorkingSetPreview();
            }
            else
            {
                sprintf_s(buf, sizeof(buf), "\n %s, ima = %.5f, mae = %.5f, cha = %.5f", "远离", Pcha.distance_ima, Pcha.distance_mae, Pcha.cha);
                printf_s(buf);
            }


        }



    }
    else
    {
        Pcha.cont = Pcha.cont + 1;
        if (Pcha.cont == 200) {
            vr::VRChaperoneSetup()->CommitWorkingCopy(vr::EChaperoneConfigFile::EChaperoneConfigFile_Live);
            printf_s("\n ugoku_Still");
            Pcha.ready.pointC = false;
            Pcha.ready.strat_P = false;
        }
    }

}




bool UGOKU2_STILL = true;

void Move_2(OverlayWidget::posecha& Pcha, vr::HmdMatrix34_t nowpose, vr::HmdMatrix34_t lastpose) {

 
    vr::HmdMatrix34_t newpose;
    char buf[1024];

    float pdis;


    //The moving distance is greater than limit
    if (Pcha.cha > _PLAYMOD.limit*2) {

        if (Pcha.cha > 0.3) {
            printf_s("\n Tracker_lose cha = %.5f",Pcha.cha);
            return;
        }
 
        //Calculate current distance
        Pcha.distance_ima =
            sqrt((nowpose.m[0][3] - _PLAYMOD._POINTC.x) * (nowpose.m[0][3] - _PLAYMOD._POINTC.x) +
                (nowpose.m[2][3] - _PLAYMOD._POINTC.z) * (nowpose.m[2][3] - _PLAYMOD._POINTC.z));

        //Calculate last distance
        Pcha.distance_mae =
            sqrt((lastpose.m[0][3] - _PLAYMOD._POINTC.x) * (lastpose.m[0][3] - _PLAYMOD._POINTC.x) +
                (lastpose.m[2][3] - _PLAYMOD._POINTC.z) * (lastpose.m[2][3] - _PLAYMOD._POINTC.z));

        pdis = Pcha.distance_ima - Pcha.distance_mae;

        Pcha.p_dis = Pcha.p_dis + pdis;
        if (trans(Pcha.p_dis) > 0.03) {
            Pcha.p_dis = 0;
            Pcha.cont = 0;
        }
        else {
            Pcha.cont = Pcha.cont + 1;

            if (Pcha.cont > 450) {
                Pcha.walk_zt = 0;
                Pcha.UGOKU3_dis = 0;
                if (!UGOKU2_STILL) {
                    if (PchaL.walk_zt == 0 & PchaR.walk_zt == 0) {
                        UGOKU2_STILL = true;
                        printf_s("\n Move2_Still");
                    }
                }

            }
        }



        if (UGOKU2_STILL) {
            Pcha.UGOKU3_dis = Pcha.UGOKU3_dis + pdis;
            if (trans(Pcha.UGOKU3_dis) < 0.03) return;
            else UGOKU2_STILL = false;
        }
        Pcha.walk_zt = 1;

        if (_PLAYMOD._POINTC.ready) {


            if (Pcha.distance_ima - Pcha.distance_mae < 0) {

                Pcha.HTdis = 0;
                if (Pcha.Tracker == 1) {
                    if (Pcha.cha < PchaR.cha && PchaR.walkb) return;
                }
                else
                {
                    if (Pcha.cha < PchaL.cha && PchaL.walkb) return;
                }
                

 


                if (_PLAYMOD.smooth) {
                    Pcha.s_x = Pcha.s_x + nowpose.m[0][3] - lastpose.m[0][3];
                    Pcha.s_z = Pcha.s_z + nowpose.m[2][3] - lastpose.m[2][3];
                    Pcha.s_i = Pcha.s_i + 1;
                    orig.m[0][3] = orig.m[0][3] + (Pcha.s_x / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
                    orig.m[2][3] = orig.m[2][3] + (Pcha.s_z / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
                }
                else
                {
                    orig.m[0][3] = orig.m[0][3] + (nowpose.m[0][3] - lastpose.m[0][3]) * ((float)_PLAYMOD.speed / 100);
                    orig.m[2][3] = orig.m[2][3] + (nowpose.m[2][3] - lastpose.m[2][3]) * ((float)_PLAYMOD.speed / 100);

                }

                if (Pcha.NewSteap) {
                    Pcha.R_dis = Pcha.R_dis + pdis;
                    if (trans(Pcha.R_dis) > 0.25) {
                        Pcha.NewSteap = false;
                        _WALKRECORD.Steps = _WALKRECORD.Steps + 1;
                    }
                }
                _WALKRECORD.Distance = _WALKRECORD.Distance + Pcha.cha;

                Pcha.walkb = true;
                newpose = orig;
                vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&newpose);
                vr::VRChaperoneSetup()->ShowWorkingSetPreview();
            }
            else
            {
                Pcha.HTdis = Pcha.HTdis + Pcha.cha;
                if (Pcha.HTdis > 0.2) {
                     
                    if (!Pcha.NewSteap) {
                        Pcha.NewSteap = true;
                        Pcha.R_dis = 0;
                    }

                    if (Pcha.s_i != 0) {
                        Pcha.s_i = 0;
                        Pcha.s_x = 0;
                        Pcha.s_z = 0;
                    }
                }

 

            }


        }



    }
    else
    {
        Pcha.cont = Pcha.cont + 1;

        if (Pcha.cont > 450) {
            Pcha.walk_zt = 0;
            Pcha.UGOKU3_dis = 0;
            if (!UGOKU2_STILL) {
                if (PchaL.walk_zt == 0 & PchaR.walk_zt == 0) {
                    UGOKU2_STILL = true;
                    printf_s("\n Move2_Still");
                }
            }

        }
    }

}

int UGOKU3_zt = 0; // 0 = Still  1 = forward 2 = backward

void Move_3(OverlayWidget::posecha& Pcha, vr::HmdMatrix34_t nowpose, vr::HmdMatrix34_t lastpose, vr::HmdMatrix34_t pointC) {
 


    vr::HmdMatrix34_t newpose;
    char buf[1024];

    float pdis;
 

    // 0.00017

    if (Pcha.cha > _PLAYMOD.limit*2) {
 
        if (Pcha.cha > 0.3) return;

        Pcha.distance_ima =
            sqrt((nowpose.m[0][3] - pointC.m[0][3]) * (nowpose.m[0][3] - pointC.m[0][3]) +
                (nowpose.m[2][3] - pointC.m[2][3]) * (nowpose.m[2][3] - pointC.m[2][3]));

        Pcha.distance_mae =
            sqrt((lastpose.m[0][3] - pointC.m[0][3]) * (lastpose.m[0][3] - pointC.m[0][3]) +
                (lastpose.m[2][3] - pointC.m[2][3]) * (lastpose.m[2][3] - pointC.m[2][3]));

        pdis = Pcha.distance_ima - Pcha.distance_mae;

        Pcha.p_dis = Pcha.p_dis + pdis;

        if (trans(Pcha.p_dis) > 0.03) {
            Pcha.p_dis = 0;
            Pcha.cont = 0;
        }
        else {
            Pcha.cont = Pcha.cont + 1;
            if (Pcha.cont > 450) {
                Pcha.walk_zt = 0;
                Pcha.UGOKU3_dis = 0;
                if (UGOKU3_zt != 0) {
                    if (PchaL.walk_zt == 0 & PchaR.walk_zt == 0) {
                        UGOKU3_zt = 0;
                        printf_s("\n move3_Still");
                    }
                }

            }
        }


        if (UGOKU3_zt == 0) {
            Pcha.UGOKU3_dis = Pcha.UGOKU3_dis + pdis;
            if (trans(Pcha.UGOKU3_dis) < 0.03) return;
        }

        if (Pcha.distance_ima - Pcha.distance_mae < 0) {
            if (UGOKU3_zt == 0) {
                UGOKU3_zt = 2;
                PchaL.walk_zt = 2;
                PchaR.walk_zt = 2;
            }
            if (UGOKU3_zt == 1) {
                Pcha.walk_zt = 1;

                Pcha.HTdis = 0;
                if (Pcha.Tracker == 1) {
                    if (Pcha.cha < PchaR.cha && PchaR.walkb) return;
                }
                else
                {
                    if (Pcha.cha < PchaL.cha && PchaL.walkb) return;
                }

                Pcha.s_x = Pcha.s_x + nowpose.m[0][3] - lastpose.m[0][3];
                Pcha.s_z = Pcha.s_z + nowpose.m[2][3] - lastpose.m[2][3];
                Pcha.s_i = Pcha.s_i + 1;
                if (_PLAYMOD.smooth) {
                    orig.m[0][3] = orig.m[0][3] + (Pcha.s_x / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
                    orig.m[2][3] = orig.m[2][3] + (Pcha.s_z / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
                }
                else
                {
                    orig.m[0][3] = orig.m[0][3] + (nowpose.m[0][3] - lastpose.m[0][3]) * ((float)_PLAYMOD.speed / 100);
                    orig.m[2][3] = orig.m[2][3] + (nowpose.m[2][3] - lastpose.m[2][3]) * ((float)_PLAYMOD.speed / 100);
                }

                if (Pcha.NewSteap) {
                    Pcha.R_dis = Pcha.R_dis + pdis;
                    if (trans(Pcha.R_dis) > 0.25) {
                        Pcha.NewSteap = false;
                        _WALKRECORD.Steps = _WALKRECORD.Steps + 1;
                    }
                }
                _WALKRECORD.Distance = _WALKRECORD.Distance + Pcha.cha;
                Pcha.walkb = true;
                newpose = orig;
                vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&newpose);
                vr::VRChaperoneSetup()->ShowWorkingSetPreview();
            }
            else
            {
                Pcha.HTdis = Pcha.HTdis + Pcha.cha;
                if (Pcha.HTdis > 0.2) {
                    if (!Pcha.NewSteap) {
                        Pcha.NewSteap = true;
                        Pcha.R_dis = 0;
                    }


                    if (Pcha.s_i != 0) {
                        Pcha.s_i = 0;
                        Pcha.s_x = 0;
                        Pcha.s_z = 0;
                    }
                }
 
            }
        }
        else
        {
            if (UGOKU3_zt == 0) {
                UGOKU3_zt = 1;
                PchaL.walk_zt = 1;
                PchaR.walk_zt = 1;
            }
            if (UGOKU3_zt == 2) {
                Pcha.walk_zt = 2;

                Pcha.HTdis = 0;
                if (Pcha.Tracker == 1) {
                    if (Pcha.cha < PchaR.cha && PchaR.walkb) return;
                }
                else
                {
                    if (Pcha.cha < PchaL.cha && PchaL.walkb) return;
                }

                Pcha.s_x = Pcha.s_x + nowpose.m[0][3] - lastpose.m[0][3];
                Pcha.s_z = Pcha.s_z + nowpose.m[2][3] - lastpose.m[2][3];
                Pcha.s_i = Pcha.s_i + 1;
                if (_PLAYMOD.smooth) {
                    orig.m[0][3] = orig.m[0][3] + (Pcha.s_x / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
                    orig.m[2][3] = orig.m[2][3] + (Pcha.s_z / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
                }
                else
                {
                    orig.m[0][3] = orig.m[0][3] + (nowpose.m[0][3] - lastpose.m[0][3]) * ((float)_PLAYMOD.speed / 100);
                    orig.m[2][3] = orig.m[2][3] + (nowpose.m[2][3] - lastpose.m[2][3]) * ((float)_PLAYMOD.speed / 100);
                }

                if (Pcha.NewSteap) {
                    Pcha.R_dis = Pcha.R_dis + pdis;
                    if (trans(Pcha.R_dis) > 0.25) {
                        Pcha.NewSteap = false;
                        _WALKRECORD.Steps = _WALKRECORD.Steps + 1;
                    }
                }
                _WALKRECORD.Distance = _WALKRECORD.Distance + Pcha.cha;
                Pcha.walkb = true;
                newpose = orig;
                vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&newpose);
                vr::VRChaperoneSetup()->ShowWorkingSetPreview();
            }
            else
            {
                Pcha.HTdis = Pcha.HTdis + Pcha.cha;
                if (Pcha.HTdis > 0.2) {
                    if (!Pcha.NewSteap) {
                        Pcha.NewSteap = true;
                        Pcha.R_dis = 0;
                    }


                    if (Pcha.s_i != 0) {
                        Pcha.s_i = 0;
                        Pcha.s_x = 0;
                        Pcha.s_z = 0;
                    }
                }
            }

        }
    }
    else
    {
        Pcha.cont = Pcha.cont + 1;
        if (Pcha.cont > 450) {
            Pcha.walk_zt = 0;
            Pcha.UGOKU3_dis = 0;
            if (UGOKU3_zt != 0) {
                if (PchaL.walk_zt == 0 & PchaR.walk_zt == 0) {
                    UGOKU3_zt = 0;
                    printf_s("\n Move3_Still");
                }
            }

        }
    }

}

void Move_4(OverlayWidget::posecha& Pcha, vr::HmdMatrix34_t nowpose, vr::HmdMatrix34_t lastpose, vr::HmdMatrix34_t pointC) {


    vr::HmdMatrix34_t newpose;
    char buf[1024];

    float pdis;

    //0.00004
    //The moving distance is greater than limit
    if (Pcha.cha > _PLAYMOD.limit * 2) {

        if (Pcha.cha > 0.3) return;


        //Calculate current distance
        Pcha.distance_ima =
            sqrt((nowpose.m[0][3] - pointC.m[0][3]) * (nowpose.m[0][3] - pointC.m[0][3]) +
                (nowpose.m[2][3] - pointC.m[2][3]) * (nowpose.m[2][3] - pointC.m[2][3]));

        //Calculate last distance
        Pcha.distance_mae =
            sqrt((lastpose.m[0][3] - pointC.m[0][3]) * (lastpose.m[0][3] - pointC.m[0][3]) +
                (lastpose.m[2][3] - pointC.m[2][3]) * (lastpose.m[2][3] - pointC.m[2][3]));

        pdis = Pcha.distance_ima - Pcha.distance_mae;

        Pcha.p_dis = Pcha.p_dis + pdis;
        if (trans(Pcha.p_dis) > 0.03) {
            Pcha.p_dis = 0;
            Pcha.cont = 0;
        }
        else {
            Pcha.cont = Pcha.cont + 1;

            if (Pcha.cont > 450) {
                Pcha.walk_zt = 0;
                Pcha.UGOKU3_dis = 0;
                if (!UGOKU2_STILL) {
                    if (PchaL.walk_zt == 0 & PchaR.walk_zt == 0) {
                        UGOKU2_STILL = true;
                        printf_s("\n move4_Still");
                    }
                }

            }
        }



        if (UGOKU2_STILL) {
            Pcha.UGOKU3_dis = Pcha.UGOKU3_dis + pdis;
            if (trans(Pcha.UGOKU3_dis) < 0.03) return;
            else UGOKU2_STILL = false;
        }
        Pcha.walk_zt = 1;


        if (Pcha.distance_ima - Pcha.distance_mae < 0) {

            Pcha.HTdis = 0;
            if (Pcha.Tracker == 1) {
                if (Pcha.cha < PchaR.cha && PchaR.walkb) return;
            }
            else
            {
                if (Pcha.cha < PchaL.cha && PchaL.walkb) return;
            }

            if (_PLAYMOD.smooth) {
                Pcha.s_x = Pcha.s_x + nowpose.m[0][3] - lastpose.m[0][3];
                Pcha.s_z = Pcha.s_z + nowpose.m[2][3] - lastpose.m[2][3];
                Pcha.s_i = Pcha.s_i + 1;
                orig.m[0][3] = orig.m[0][3] + (Pcha.s_x / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
                orig.m[2][3] = orig.m[2][3] + (Pcha.s_z / Pcha.s_i) * ((float)_PLAYMOD.speed / 100);
            }
            else
            {
                orig.m[0][3] = orig.m[0][3] + (nowpose.m[0][3] - lastpose.m[0][3]) * ((float)_PLAYMOD.speed / 100);
                orig.m[2][3] = orig.m[2][3] + (nowpose.m[2][3] - lastpose.m[2][3]) * ((float)_PLAYMOD.speed / 100);

            }

            if (Pcha.NewSteap) {
                Pcha.R_dis = Pcha.R_dis + pdis;
                if (trans(Pcha.R_dis) > 0.25) {
                    Pcha.NewSteap = false;
                    _WALKRECORD.Steps = _WALKRECORD.Steps + 1;
                }
            }
            _WALKRECORD.Distance = _WALKRECORD.Distance + Pcha.cha;
            Pcha.walkb = true;
            newpose = orig;
            vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&newpose);
            vr::VRChaperoneSetup()->ShowWorkingSetPreview();
        }
        else
        {
            Pcha.HTdis = Pcha.HTdis + Pcha.cha;
            if (Pcha.HTdis > 0.2) {

                if (!Pcha.NewSteap) {
                    Pcha.NewSteap = true;
                    Pcha.R_dis = 0;
                }

                if (Pcha.s_i != 0) {
                    Pcha.s_i = 0;
                    Pcha.s_x = 0;
                    Pcha.s_z = 0;
                }
            }

        }

         
    }
    else
    {
        Pcha.cont = Pcha.cont + 1;

        if (Pcha.cont > 450) {
            Pcha.walk_zt = 0;
            Pcha.UGOKU3_dis = 0;
            if (!UGOKU2_STILL) {
                if (PchaL.walk_zt == 0 & PchaR.walk_zt == 0) {
                    UGOKU2_STILL = true;
                    printf_s("\n Move4_Still");
                }
            }

        }
    }

}
void pose_loop() {
    vr::TrackedDevicePose_t devicePose[vr::k_unMaxTrackedDeviceCount];
    vr::ETrackedDeviceClass trackedDeviceClass;
    vr::ETrackedPropertyError err = vr::TrackedProp_Success;
    vr::ETrackedControllerRole controllers;
    char buf[1024];


    PchaL.walk_zt = 0;
    PchaR.walk_zt = 0;

    PchaL.Tracker = 1;
    PchaR.Tracker = 2;

    vr::HmdMatrix34_t waistpose;
    float waist_X = 0;
    float waist_Z = 0;
    float waistRcha = 0;
    float waist_newY = 0;
    float waist_R_Y = 0;
    int an = 360;
    OverlayWidget::Trackers tracker;
    vr::HmdMatrix34_t hmdpose;
    vr::HmdMatrix34_t newpose;

    vr::TrackedDevicePose_t GdevicePose[vr::k_unMaxTrackedDeviceCount];


    vr::VRChaperoneSetup()->RevertWorkingCopy();
    vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&orig);

    sprintf_s(buf, sizeof(buf), "\n orig  X =  %.5f, Y = %.5f", orig.m[0][3], orig.m[2][3]);
    printf_s(buf);


    while (start)
    {


        Sleep(2);

        VRSystem->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseRawAndUncalibrated, 0, devicePose, vr::k_unMaxTrackedDeviceCount);


        for (vr::TrackedDeviceIndex_t ids = 0; ids < vr::k_unMaxTrackedDeviceCount; ids++) {
            trackedDeviceClass = vr::VRSystem()->GetTrackedDeviceClass(ids);

            switch (trackedDeviceClass) {
            case vr::TrackedDeviceClass_GenericTracker:


                /*
                    Tracker-> vive_tracker_right_foot
                    Tracker-> vive_tracker_waist
                    Tracker-> vive_tracker_left_foot
                */




                if (tracker.ok) {
                    if (tracker.waist == ids) {
                        /*
                        waistpose = devicePose[ids].mDeviceToAbsoluteTracking;
                        if (!waist_proofreader) {
                            waistRcha = copysign(sqrt(fmax(0, 1 - hmdpose.m[0][0] + hmdpose.m[1][1] - hmdpose.m[2][2])) / 2, hmdpose.m[0][2] - hmdpose.m[2][0]) * an -
                                copysign(sqrt(fmax(0, 1 - waistpose.m[0][0] + waistpose.m[1][1] - waistpose.m[2][2])) / 2, waistpose.m[0][2] - waistpose.m[2][0]) * an;
                            waist_proofreader = true;
                            sprintf_s(buf, sizeof(buf), "\n WaistRcha =  %.5f, hmdY = %.5f, waistY = %.5f", waistRcha, copysign(sqrt(fmax(0, 1 - hmdpose.m[0][0] + hmdpose.m[1][1] - hmdpose.m[2][2])) / 2, hmdpose.m[0][2] - hmdpose.m[2][0]) * an
                                , copysign(sqrt(fmax(0, 1 - waistpose.m[0][0] + waistpose.m[1][1] - waistpose.m[2][2])) / 2, waistpose.m[0][2] - waistpose.m[2][0]) * an);
                            printf_s(buf);
                        }
                        // 将腰部坐标向身后平移 以方便脚的计算
                        waist_R_Y = copysign(sqrt(fmax(0, 1 - waistpose.m[0][0] + waistpose.m[1][1] - waistpose.m[2][2])) / 2, waistpose.m[0][2] - waistpose.m[2][0]) * an;
                        waist_newY = waist_R_Y + waistRcha;
                        if (waist_newY > 360) {
                            waist_newY = 1 - (waist_newY - 360);
                        }
                        if (waist_newY < -360) {
                            waist_newY = (waist_newY + 360) / -1;
                        }

                        waist_X = 1 * sin(waist_newY * (PI / 360.0));
                        waist_Z = 1 * cos(waist_newY * (PI / 360.0));
                        */
                    }
                    else if (tracker.footL == ids)
                    {

                        PchaL.walkb = false;

                        PchaL.cha =
                            sqrt((tracker.lastposeL.m[0][3] -
                                devicePose[ids].mDeviceToAbsoluteTracking.m[0][3]) *
                                (tracker.lastposeL.m[0][3] -
                                    devicePose[ids].mDeviceToAbsoluteTracking.m[0][3]) +
                                (tracker.lastposeL.m[2][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[2][3]) *
                                (tracker.lastposeL.m[2][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[2][3]));

                        if (_PLAYMOD.mod == 0) { 
                            if (_PLAYMOD.back)Move_3(PchaL, devicePose[ids].mDeviceToAbsoluteTracking, tracker.lastposeL, hmdpose);
                            else Move_4(PchaL, devicePose[ids].mDeviceToAbsoluteTracking, tracker.lastposeL, hmdpose);
                        }
                        else { Move_2(PchaL, devicePose[ids].mDeviceToAbsoluteTracking, tracker.lastposeL); }

                        tracker.lastposeL = devicePose[ids].mDeviceToAbsoluteTracking;
                    }
                    else if (tracker.footR == ids)
                    {
                        PchaR.walkb = false;

                        PchaR.cha =
                            sqrt((tracker.lastposeR.m[0][3] -
                                devicePose[ids].mDeviceToAbsoluteTracking.m[0][3]) *
                                (tracker.lastposeR.m[0][3] -
                                    devicePose[ids].mDeviceToAbsoluteTracking.m[0][3]) +
                                (tracker.lastposeR.m[2][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[2][3]) *
                                (tracker.lastposeR.m[2][3] - devicePose[ids].mDeviceToAbsoluteTracking.m[2][3]));

 

                        if (_PLAYMOD.mod == 0) {
                            if (_PLAYMOD.back) Move_3(PchaR, devicePose[ids].mDeviceToAbsoluteTracking, tracker.lastposeR, hmdpose);
                            else Move_4(PchaR, devicePose[ids].mDeviceToAbsoluteTracking, tracker.lastposeR, hmdpose);
                        }
                        else { Move_2(PchaR, devicePose[ids].mDeviceToAbsoluteTracking, tracker.lastposeR); }
                        tracker.lastposeR = devicePose[ids].mDeviceToAbsoluteTracking;

                    }

                }
                else
                {
                    VRSystem->GetStringTrackedDeviceProperty(ids, vr::TrackedDeviceProperty::Prop_ControllerType_String, buf, sizeof(buf), &err);
                    std::string str = buf;
                    if (str.find("waist", 0) != -1) {
                        tracker.waist = ids;
                    }
                    else if (str.find("left_foot", 0) != -1)
                    {
                        tracker.footL = ids;
                        tracker.lastposeL = devicePose[ids].mDeviceToAbsoluteTracking;
                    }
                    else if (str.find("right_foot", 0) != -1)
                    {
                        tracker.lastposeR = devicePose[ids].mDeviceToAbsoluteTracking;
                        tracker.footR = ids;
                    }
                    if (tracker.waist != -1 && tracker.footL != -1 && tracker.footR != -1) {
                        tracker.ok = true;
                    }
                }

                break;

            case vr::TrackedDeviceClass_HMD:
                if (_PLAYMOD.mod == 0) {
                    hmdpose = devicePose[0].mDeviceToAbsoluteTracking;
                    hmdpose.m[0][3] = hmdpose.m[0][3] + 1.5 * hmdpose.m[0][2];
                    hmdpose.m[1][3] = hmdpose.m[1][3] + 1.5 * hmdpose.m[1][2];
                    hmdpose.m[2][3] = hmdpose.m[2][3] + 1.5 * hmdpose.m[2][2];
                }
                break;

            case vr::TrackedDeviceClass_Controller:

                controllers = VRSystem->GetControllerRoleForTrackedDeviceIndex(ids);

                if (controllers == vr::TrackedControllerRole_LeftHand) {


                }
                else if (controllers == vr::TrackedControllerRole_RightHand)
                {


                }


            }


        }




    }
    printf_s("\n pose_loop->exit");
}



void GetLocale(char *mm)
{
    LCID lcid = GetThreadLocale();
    wchar_t name[LOCALE_NAME_MAX_LENGTH];
    LCIDToLocaleName(lcid, name, LOCALE_NAME_MAX_LENGTH, 0);
    wcstombs(mm, name, sizeof(name));
}

 


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    GetLocale(Locale);
    std::string path = Path_GetWorkingDirectory();
    strcpy(_icon, path.c_str());
    strcat(_icon, "\\img\\ico.png");
    char documents[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents);
     
    strcat(documents, "\\OpenVR-FBTWalk");
    if (GetFileAttributesA(documents) & FILE_ATTRIBUTE_DIRECTORY) {
         CreateDirectory(documents, NULL);
    }
    strcpy(_ini, documents);
    strcat(_ini, "\\pz.ini");
 
    LPTSTR lpPath = new char[MAX_PATH];
    strcpy(lpPath, _ini);
    _PLAYMOD.mod = GetPrivateProfileInt("PLAYMOD", "mod", 0, lpPath);
    if (GetPrivateProfileInt("PLAYMOD", "smooth", 0, lpPath) == 0) _PLAYMOD.smooth = true;
    else _PLAYMOD.smooth = false;
    if (GetPrivateProfileInt("PLAYMOD", "frist", 0, lpPath) == 0) _PLAYMOD.frist = true;
    else _PLAYMOD.frist = false;
    if (GetPrivateProfileInt("PLAYMOD", "back", 0, lpPath) == 0) _PLAYMOD.back = false;
    else _PLAYMOD.back = true;
    _PLAYMOD.speed = GetPrivateProfileInt("PLAYMOD", "Walk_speed", 100, lpPath);
    char xx[512];
    GetPrivateProfileString("PLAYMOD_POINTC", "x", "", xx, 512, lpPath);
    char zz[512];
    GetPrivateProfileString("PLAYMOD_POINTC", "z", "", zz, 512, lpPath);
    char limit[512];
    GetPrivateProfileString("PLAYMOD", "limit", "0.0004", limit, 512, lpPath);
    char buf[1024];
    _PLAYMOD.limit = atof(limit);
    _PLAYMOD._POINTC.x = atof(xx);
    _PLAYMOD._POINTC.z = atof(zz);
    sprintf_s(buf, sizeof(buf), "\nPOINTC-> X= %.5f, Z = %.5f", _PLAYMOD._POINTC.x, _PLAYMOD._POINTC.z);
    printf_s(buf);
    sprintf_s(buf, sizeof(buf), "\nlimit-> %.5f", _PLAYMOD.limit);
    printf_s(buf);
    if (_PLAYMOD.mod == 0) {
        printf_s("\n mod->0");
    }
    else
    {
        if (xx && *xx != '\0') {
            printf_s("\n mod->1");
            _PLAYMOD._POINTC.ready = true;
        }
        else { printf_s("\n mod->1 (not ready)"); }

    }

    delete[] lpPath;
    printf_s("\n");
    bool hmdconnected = false;
    VRSystem = nullptr;

    while (VRSystem == nullptr) {
        VRSystem = vr::VR_Init(&eError, vr::VRApplication_Overlay);
        Sleep(500);
    }

    printf_s("SteamVR_Process->ready\n");
    while (!hmdconnected) {
        hmdconnected = VRSystem->IsTrackedDeviceConnected(0);
        Sleep(500);
    }

    printf_s("SteamVR_HMD->ready\n");

    bool orig_ok = false;



    while (!orig_ok)
    {
        orig_ok = vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&_ORIG);
        Sleep(500);
    }

    sprintf_s(buf, sizeof(buf), "\n orig  X =  %.5f, Y = %.5f", _ORIG.m[0][3], _ORIG.m[2][3]);
    printf_s(buf);
    POSE_LOOP = pose_loop;

    OverlayWidget* pOverlayWidget = new OverlayWidget;
    COpenVROverlayController::SharedInstance()->Init();
    COpenVROverlayController::SharedInstance()->SetWidget(pOverlayWidget);
 
 
 
    return a.exec();
}

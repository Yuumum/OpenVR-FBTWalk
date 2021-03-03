#include "../headers/openvr.h"
#include "overlaywidget.h"
 

vr::IVRSystem* VRSystem;
vr::EVRInitError eError;
vr::HmdMatrix34_t orig;
vr::HmdMatrix34_t _ORIG;
OverlayWidget::playmod _PLAYMOD;
OverlayWidget::WalkRecord _WALKRECORD;
bool start = false;
char _ini[1024];
void(*POSE_LOOP)();
vr::VROverlayHandle_t m_Handle;
char _icon[1024];
char Locale[1024];

#include "stdafx.h"
#include "ZoomOutCamera.h"

void ZoomOutCamera::Update() {
	//ズームアウト演出
	m_zoomOutTimer += m_isZoomout ? 0.1f : -0.1f;
	m_zoomOutTimer = CMath::Clamp(m_zoomOutTimer, 0.0f, 1.0f);
	CVector3 zoomOutOffset = CVector3(0.f,0.f,-10000.f)*m_zoomOutTimer;
	m_cam.SetPos(m_beforeCam->GetPos() + zoomOutOffset);
	//カメラ元に戻す
	if (!m_isZoomout && m_zoomOutTimer < FLT_EPSILON) {
		Disable();
	}
}

void ZoomOutCamera::Init(const GameObj::PerspectiveCamera& camera, bool isZoomOut) {
	m_beforeCam = &camera;

	if (isZoomOut) {
		m_cam.SetPos(camera.GetPos());
	}
	m_cam.SetTarget(camera.GetTarget());
	m_cam.SetUp(camera.GetUp());

	m_cam.SetNear(camera.GetNear());
	m_cam.SetFar(camera.GetFar());

	m_cam.SetViewAngle(camera.GetFOV());

	if (isZoomOut) {
		m_zoomOutTimer = 0.0f;
	}
	m_isZoomout = isZoomOut;

	m_isDisable = false;
	m_cam.SetEnable(true);
	SetEnable(true);
}

void ZoomOutCamera::Disable() {
	m_isDisable = true;
	m_cam.SetEnable(false);
	SetEnable(false);
}
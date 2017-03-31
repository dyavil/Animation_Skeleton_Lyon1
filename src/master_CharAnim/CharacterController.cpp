#include "CharacterController.h"


CharacterController::CharacterController(){
	//speed = Vector(0.f, 0.f, 0.f);
	m_v = 0;
	m_vMax = 3;
	m_ch2w = Identity ();
}

void CharacterController::update(const float dt){
	// pos + direct *speezd
	Point p = position() + direction() * m_v;
	m_ch2w[0][3] = p.x;
	m_ch2w[1][3] = p.y;
	m_ch2w[2][3] = p.z;

}

void CharacterController::turn(const Transform& transf_v){
	m_ch2w = m_ch2w * transf_v;
}

void CharacterController::turnXZ(const float& rot_angle_v){
	m_ch2w = m_ch2w * Rotation(Vector(0, 1, 0), rot_angle_v);
}

void CharacterController::accelerate(const float& speed_inc){
	if(m_v + speed_inc <= m_vMax) m_v += speed_inc;
}

void CharacterController::setVelocityMax(const float vmax){
	m_vMax = vmax;
}

void CharacterController::setPosition(const Point& p){

}

void CharacterController::setVelocityNorm(const float v){

}

const Point CharacterController::position() const{
	Point t = m_ch2w(Point(0, 0, 0));
	return t;
}

const Vector CharacterController::direction() const{
	return m_ch2w(Vector(0, 0, 1));
}


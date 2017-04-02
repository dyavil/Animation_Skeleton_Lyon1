#include "mat.h"
#include "vec.h"
#include "BVH.h"

class CharacterController
{
public:
	CharacterController();
	~CharacterController(){};

	void update(const float dt);

	void turn(const Transform& transf_v);
	void turnXZ(const float& rot_angle_v);
	void accelerate(const float& speed_inc);
	void setVelocityMax(const float vmax);
	void want_iddle();
	void want_kick();

	void setPosition(const Point& p);
	void setVelocityNorm(const float v);

	const Point position() const;
	const Vector direction() const;
	float velocity() const;
	Transform & getTrans(){return m_ch2w;};

protected:
    Transform m_ch2w;   // matrice du character vers le monde
                        // le personnage se déplace vers X
                        // il tourne autour de Y
                        // Z est sa direction droite 
	float m_v;          // le vecteur vitesse est m_v * m_ch2w * Vector(1,0,0)
	float m_vMax;       // ne peut pas accélérer plus que m_vMax
};
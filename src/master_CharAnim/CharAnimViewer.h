
#ifndef _CHARANIMVIEWER_H
#define _CHARANIMVIEWER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
#include "Skeleton.h"
#include "TransformQ.h"
#include "CharacterController.h"

#include <Particles.h>

class CharAnimViewer : public Viewer
{
public:
    CharAnimViewer();

    int init();
    int render();
    int update( const float time, const float delta );
	void applyCollide();
    Particle & draw_bullett(Point position, Vector dir);
    void draw_bullets();


protected:

    chara::BVH m_bvh;
    int m_frameNumber;

    Skeleton m_ske;

    Particles m_part;

    Particles bullets;

    CharacterController controller;

	void draw_skeleton(const Skeleton& );
	void draw_particles();

	chara::BVH iddle_bvh;
	chara::BVH walk_bvh;
	chara::BVH run_bvh;
	chara::BVH kick_bvh;
};



#endif

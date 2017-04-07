
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "CharAnimViewer.h"


using namespace std;
using namespace chara;


CharAnimViewer::CharAnimViewer() : Viewer(), m_frameNumber(0), controller()
{
}


int CharAnimViewer::init()
{
    Viewer::init();
    cout<<"==>master_CharAnim/CharAnimViewer"<<endl;
    m_camera.lookat( Point(0,100,10), 1000 );
    gl.light( Point(300, 300, 300 ) );

    //b_draw_grid = false;

	m_part.resize( 10 );

    init_cylinder();
    init_sphere();
    init_bullet();


    //m_bvh.init("data/bvh/Robot.bvh" );
	m_bvh.init( smart_path("data/bvh/motionFSM/avatar_talk.bvh") );
	run_bvh.init(smart_path("data/bvh/motionFSM/avatar_run.bvh"));
	iddle_bvh.init(smart_path("data/bvh/motionFSM/avatar_talk.bvh"));
	walk_bvh.init(smart_path("data/bvh/motionFSM/avatar_walk.bvh"));
	kick_bvh.init(smart_path("data/bvh/motionFSM/avatar_kick_roundhouse_R.bvh"));

    m_frameNumber = 0;
    cout<<endl<<"========================"<<endl;
    cout<<"BVH decription"<<endl;
    cout<<m_bvh<<endl;
    cout<<endl<<"========================"<<endl;
    m_ske.init( m_bvh );
    Transform tt = Identity();
    m_ske.setPose( m_bvh, -1, tt);// met le skeleton a la pose au repos
    return 0;
}

int nextFrame(const chara::BVH& bvh, const chara::BVH& bvh2, int currentFrame) {
	float distmin = 2000.0;
	int resFrame = 0;
	Transform tt = Identity();
	Skeleton a, b;
	a.init(bvh);
	b.init(bvh2);
	a.setPose(bvh, currentFrame, tt);
	for (size_t i = 0; i < bvh2.getNumberOfFrame(); i++)
	{
		b.setPose(bvh2, i, tt);
		float distT = Distance(a, b);
		if (distT < distmin) {
			distmin = distT;
			resFrame = i;
		}
	}
	return resFrame;
}

void CharAnimViewer::applyCollide() {
	for (size_t i = 0; i < m_part.size(); i++)
	{
		for (size_t j = 0; j < m_ske.numberOfJoint(); j++)
		{
			m_part[i].collision(m_ske.getJointPosition(j), 4.0, controller.direction());
		}
	}
}

void CharAnimViewer::draw_skeleton(const Skeleton& )
{
    
    for (int i = 0; i < m_ske.numberOfJoint(); ++i)
    {
        draw_sphere(m_ske.getJointPosition(i), 4.0);
        //cout << m_ske.get
        if (m_ske.getParentId(i) >= 0)
        {
            draw_cylinder(m_ske.getJointPosition(i), m_ske.getJointPosition(m_ske.getParentId(i)), 4);
        }
        
    }

}



int CharAnimViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Viewer::manageCameraLight();

    gl.camera(m_camera);

    draw_particles();
    draw_quad( RotationX(-90)*Scale(500,500,1) );
    draw_bullets();

	// Affiche le skeleton
    draw_skeleton( m_ske );


    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.
	static bool j = false;
	static bool run = false;
	static bool walk = false;
	static bool kick = false;
	static float oldtime = time;
	static float shotoldtime = time;
	static Point oldPosition = controller.position();
	if (!j && !run  && !kick && m_frameNumber >= m_bvh.getNumberOfFrame() - 1) {
		m_frameNumber = 0;
	}
	if (j && m_frameNumber >= iddle_bvh.getNumberOfFrame() - 1) {
		j = false;
		m_frameNumber = 0;
	}
	if (kick && m_frameNumber >= kick_bvh.getNumberOfFrame() - 2) {
		kick = false;
		//cout << endl << time << " , " << delta << endl;
		m_frameNumber = 0;
	}
	if (run && !kick && m_frameNumber >= run_bvh.getNumberOfFrame() - 2) {
		m_frameNumber = 0;
	}

	if (walk && !kick && m_frameNumber >= walk_bvh.getNumberOfFrame() - 2) {
		m_frameNumber = 0;
	}


	if (key_state('n')){
		if ((time - shotoldtime) > 100) {
			draw_bullett(controller.position(), controller.direction()*controller.velocity());
			shotoldtime = time;
		}
		

	}

    if (key_state('z') && !kick){
		if (key_state('e') && !run && !kick)
		{
			
			run = true;
			controller.setVelocityMax(5);
			//cout << endl << m_frameNumber << endl;
			m_frameNumber = nextFrame(walk_bvh, run_bvh, m_frameNumber);
			//cout << endl << m_frameNumber << endl;
			controller.accelerate(0.1);
			controller.update(0.1f);
		}
		else {
			walk = true;
			if (!key_state('e')) {
				
				if (run) {
					m_frameNumber = nextFrame(run_bvh, walk_bvh, m_frameNumber);
					run = false;
				}
				controller.setVelocityMax(1.5);
			}
			controller.accelerate(0.1);
			controller.update(0.1f);
			if ((time - oldtime) > 40) {
				m_frameNumber++;
				oldtime = time;
			}
		}
	}
	
	else walk = false;
	if (key_state('x') && !kick)
	{
		controller.want_kick();
		m_frameNumber = nextFrame(kick_bvh, m_bvh, m_frameNumber);
		kick = true;
	}
    if (key_state('q')){
        controller.turnXZ(5);

        //controller.update(0.00f);
    }
    if (key_state('d')){
        controller.turnXZ(-5);
        //controller.update(0.00f);
    }
	if (!key_state('z') && !key_state('x') && !key_state('q') && !key_state('d') && !j && !walk && !run && !kick) {
		j = true;
		m_frameNumber = 0;
	}
	if(walk || run || kick) j = false;

    /*for (int i = 0; i < m_ske.numberOfJoint(); ++i)
    {
        
        m_ske.setTransform(i, controller.getTrans());
    }
    */
	/*if (key_state('n')) { m_frameNumber++; cout << m_frameNumber << endl; }
	if (key_state('b')) { m_frameNumber--; cout << m_frameNumber << endl; }*/

    Transform t = Vector(0, 50, 0)*controller.getTrans();
	if (!kick && j && !run && !walk && m_frameNumber < iddle_bvh.getNumberOfFrame() - 1) {
		m_ske.setPose(iddle_bvh, m_frameNumber, t);
		if ((time - oldtime) > 100) {
			m_frameNumber++;
			oldtime = time;
		}
	}
	else if (!kick && run && !j && m_frameNumber < run_bvh.getNumberOfFrame() - 1) {
		m_ske.setPose(run_bvh, m_frameNumber, t);
		if ((time - oldtime) > 22) {
			m_frameNumber++;
			oldtime = time;
		}
	}
	else if (kick && !walk && !run && m_frameNumber < kick_bvh.getNumberOfFrame() - 1)
	{
		m_ske.setPose(kick_bvh, m_frameNumber, t);
		if ((time - oldtime) > 30) {
			m_frameNumber++;
			oldtime = time;
		}
	}
	else if (walk && !run && !kick && m_frameNumber < walk_bvh.getNumberOfFrame() - 1) {
		m_ske.setPose(walk_bvh, m_frameNumber, t);
	}
	else if (run && kick && m_frameNumber < kick_bvh.getNumberOfFrame() - 1) {
		run = false;
		m_ske.setPose(kick_bvh, m_frameNumber, t);
		if ((time - oldtime) > 30) {
			m_frameNumber++;
			oldtime = time;
		}
	}
	else{
		m_ske.setPose(m_bvh, m_frameNumber, t);
	}

    m_part.update(0.02f);
    bullets.update(0.02f);
	applyCollide();

	if (distance(oldPosition, controller.position()) > 0.0) {
		oldPosition = controller.position();
	}
	else if (!kick) run = false;

    return 0;
}



void CharAnimViewer::draw_particles()
{
    int i;
    for(i=0;i<m_part.size();++i)
    {
		if (m_part[i].radius()>0)
			draw_sphere( m_part[i].position(), m_part[i].radius() );
    }
}

void CharAnimViewer::draw_bullets()
{
    int i;
    for(i=0;i<bullets.size();++i)
    {
		if (bullets[i].radius()>0)
			draw_bullet( bullets[i].position(), bullets[i].radius() );
    }
}


Particle & CharAnimViewer::draw_bullett(Point position, Vector dir)
{
	Particle bullet;
	bullet.setVal(position+Vector(0, 70, 0), dir, 5.0);
	draw_bullet( position+Vector(0, 70, 0), 5.0);
	bullets.push_back(bullet);
	return bullet;
}

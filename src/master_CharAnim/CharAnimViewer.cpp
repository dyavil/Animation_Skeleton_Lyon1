
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

    //m_part.resize( 10 );


    init_cylinder();
    init_sphere();


    //m_bvh.init("data/bvh/Robot.bvh" );
	m_bvh.init( smart_path("data/bvh/motionGraph/courir.bvh") );

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



void CharAnimViewer::draw_skeleton(const Skeleton& )
{
    
    for (int i = 0; i < m_ske.numberOfJoint(); ++i)
    {
        draw_sphere(m_ske.getJointPosition(i), 3.0);
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

	// Affiche le skeleton
    draw_skeleton( m_ske );


    return 1;
}


int CharAnimViewer::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.
    if(m_frameNumber == m_bvh.getNumberOfFrame()-1) m_frameNumber = 0;
    
    if (key_state('z')){
        controller.accelerate(1);
        controller.update(0.00f);
        m_frameNumber++;
    }
    if (key_state('q')){
        controller.turnXZ(5);

        //controller.update(0.00f);
    }
    if (key_state('d')){
        controller.turnXZ(-5);
        //controller.update(0.00f);
    }
    for (int i = 0; i < m_ske.numberOfJoint(); ++i)
    {
        
        m_ske.setTransform(i, controller.getTrans());
    }
	if (key_state('n')) { m_frameNumber++; cout << m_frameNumber << endl; }
	if (key_state('b')) { m_frameNumber--; cout << m_frameNumber << endl; }

    Transform t = Vector(0, 50, 0)*controller.getTrans();
	m_ske.setPose( m_bvh, m_frameNumber , t);

    m_part.update(0.1f);

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

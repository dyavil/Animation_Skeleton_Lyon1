
#include "Skeleton.h"

using namespace chara;

void Skeleton::init(const BVH& bvh)
{
	m_joints.resize(bvh.getNumberOfJoint());
	for (int i = 0; i < bvh.getNumberOfJoint(); ++i)
	{
		m_joints[i].m_parentId = bvh.getJoint(i).getParentId();
	}
}


Point Skeleton::getJointPosition(int i) const
{
	//Point p = m_joints[i].m_l2w(Point(0, 0, 0));
	Point p = m_joints[i].m_l2w(Point(0, 0, 0));

	return p;
}


int Skeleton::getParentId(const int i) const
{
	return m_joints[i].m_parentId;
}


void Skeleton::setPose(const BVH& bvh, int frameNumber, Transform & tw)
{
	for (int i = 0; i < bvh.getNumberOfJoint(); ++i)
	{
		int nbch = bvh.getJoint(i).getNumberOfChannel();
		float x, y, z;
		bvh.getJoint(i).getOffset(x, y, z);
		Transform t = Vector(x, y, z);
		for (int j = 0; j < nbch; ++j)
		{
			BVHChannel chan = bvh.getJoint(i).getChannel(j);
			if (chan.isRotation())
			{
				switch (chan.getAxis()){
					case AXIS_X:
						t = t * RotationX(chan.getData(frameNumber));
						break;
					case AXIS_Y:
						t = t * RotationY(chan.getData(frameNumber));
						break;
					case AXIS_Z:
						t = t * RotationZ(chan.getData(frameNumber));
						break;
					default:
						break;
				}

			}
			else if(chan.isTranslation()){
				switch (chan.getAxis()){
					case AXIS_X:
						t = t * Translation(chan.getData(frameNumber), 0, 0);
						break;
					case AXIS_Y:
						t = t * Translation(0, chan.getData(frameNumber), 0);
						break;
					case AXIS_Z:
						t = t * Translation(0, 0, chan.getData(frameNumber));
						break;
					default:
						break;
				}
			}
			
		}
		if (i > 0) m_joints[i].m_l2w = m_joints[m_joints[i].m_parentId].m_l2w * t;
		else m_joints[i].m_l2w = tw;
	}
}



float Distance(const Skeleton& a, const Skeleton& b) {
	float res = 0.0;
	if (a.numberOfJoint() != b.numberOfJoint()) return res;
	//std::cout << "passed" << std::endl;
	for (size_t i = 0; i < a.numberOfJoint(); i++)
	{
		res += distance(a.m_joints[i].m_l2w(Point(0, 0, 0)), b.m_joints[i].m_l2w(Point(0, 0, 0)));
	}
	return res;
}


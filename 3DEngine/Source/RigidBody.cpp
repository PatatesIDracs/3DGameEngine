#include "RigidBody.h"

#include "Transform.h"
#include "jpPhysicsRigidBody.h"

RigidBody::RigidBody(GameObject * parent, bool isactive)
{
	if (parent != nullptr)
		transform = parent->GetTransform();
}

RigidBody::~RigidBody()
{
}

void RigidBody::Update()
{
	if (transform && rigid_body)
	{
		physx::PxTransform transf = rigid_body->px_body->getGlobalPose();
		transform->SetTransform(float3(transf.p.x, transf.p.y, transf.p.z), Quat(transf.q.x, transf.q.y, transf.q.z, transf.q.w));
		own_update = true;
	}
}

void RigidBody::UpdateTransform()
{
	if(rigid_body && !own_update) {
		physx::PxMat44 mat = physx::PxMat44((float*)transform->GetLocalTransform().ptr());
		rigid_body->px_body->setGlobalPose(physx::PxTransform(mat));
	}
	else own_update = false;
}

void RigidBody::DrawComponent()
{
}

void RigidBody::SetRigidBody(const jpPhysicsRigidBody * new_body)
{
	if (rigid_body) {

	}
	else {
		rigid_body = (jpPhysicsRigidBody*)new_body;

		if (transform == nullptr && parent)
			transform = parent->GetTransform();

		physx::PxMat44 mat = physx::PxMat44((float*)transform->GetLocalTransform().ptr());
		rigid_body->px_body->setGlobalPose(physx::PxTransform(mat));
	}
}

void RigidBody::Save(const char * buffer_data, char * cursor, int & bytes_copied)
{
}

void RigidBody::Load(char * cursor, int & bytes_copied)
{
}

void RigidBody::GetOwnBufferSize(uint & buffer_size)
{
}

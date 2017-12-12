#include "jpPhysicsRigidBody.h"


jpPhysicsRigidBody::jpPhysicsRigidBody(physx::PxPhysics* px_physics)
{
	//Create a default rigidbody 
	//default transform == identity
	//default geometry == box with 0.5 halfExtent
	//default material: 0.5 friction(static and dynamic) and 0.5 restitution
	default_material = px_physics->createMaterial(0.5f, 0.5f, 0.5f);
//	px_body = physx::PxCreateDynamic(*px_physics, physx::PxTransform(physx::PxIDENTITY()), *body_shape, 1.0f);
	px_body = physx::PxCreateDynamic(*px_physics, physx::PxTransform(physx::PxIDENTITY()), physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *default_material, 1.0f);
	body_shape = px_body->createShape(physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *default_material);

	//Detach the shape by default,
	//Shape must be activated using calling a diferent function
	//Made like this to separet the rigidbody from the collider
	px_body->detachShape(*body_shape);
	body_shape = nullptr;
}

jpPhysicsRigidBody::~jpPhysicsRigidBody()
{
}

void jpPhysicsRigidBody::ActivateShape()
{
	if (body_shape != nullptr)
		px_body->attachShape(*body_shape);
}

void jpPhysicsRigidBody::SetDynamic(bool is_dynamic)
{
	if (is_dynamic)
	{
		px_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
	}
	else
	{
		px_body->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	}
}

void jpPhysicsRigidBody::SetTransform(float * trans_mat)
{
	physx::PxMat44 mat = physx::PxMat44(trans_mat);
	px_body->setGlobalPose(physx::PxTransform(mat));
}

void jpPhysicsRigidBody::SetGeometry(physx::PxGeometry new_geometry)
{
	//Get the current material for the new shape
	physx::PxMaterial* body_material = nullptr;
	body_shape->getMaterials(&body_material, 1);

	px_body->detachShape(*body_shape);
	body_shape = px_body->createShape(new_geometry, *body_material);
	px_body->attachShape(*body_shape);
}

void jpPhysicsRigidBody::SetBoxGeometry(float x_scale, float y_scale, float z_scale)
{
	//Create tehe box geometry (sacale need to be divided by 2)
	physx::PxBoxGeometry boxGeometry;
	boxGeometry.halfExtents.x = x_scale / 2;
	boxGeometry.halfExtents.y = y_scale / 2;
	boxGeometry.halfExtents.z = z_scale / 2;

	//Get the current material for the new shape
	physx::PxMaterial* body_material = nullptr;
	if (body_shape != nullptr)
	{
		body_shape->getMaterials(&body_material, 1);
		px_body->detachShape(*body_shape);
		body_shape = nullptr;
	}
	else
		body_material = default_material;

	//We are creating a new shape
	//This is only needed when the previous shape isn't a boxGeometry, for now we will do it anyways 
	body_shape = px_body->createShape(boxGeometry, *body_material);
}

void jpPhysicsRigidBody::SetSphereGeometry(float radius)
{
	physx::PxSphereGeometry sphereGeometry;
	sphereGeometry.radius = radius;

	//Get the current material for the new shape
	physx::PxMaterial* body_material = nullptr;
	if (body_shape != nullptr)
	{
		body_shape->getMaterials(&body_material, 1);
		px_body->detachShape(*body_shape);
		body_shape = nullptr;
	}
	else 
		body_material = default_material;

	body_shape = px_body->createShape(sphereGeometry, *body_material);
}

void jpPhysicsRigidBody::SetMaterial(float static_friction, float dynamic_friction, float restitution)
{
	physx::PxMaterial* material;
	body_shape->getMaterials(&material, 1);
	material->setStaticFriction(static_friction);
	material->setDynamicFriction(dynamic_friction);
	material->setRestitution(restitution);
}

void jpPhysicsRigidBody::SetShape(physx::PxShape * new_shape)
{
	//Might not work
	px_body->detachShape(*body_shape);
	body_shape = new_shape;
}

void jpPhysicsRigidBody::GetTransform(physx::PxVec3& pos, physx::PxQuat& quat)
{
	physx::PxTransform transf = px_body->getGlobalPose();
	pos = transf.p;
	quat = transf.q;
}
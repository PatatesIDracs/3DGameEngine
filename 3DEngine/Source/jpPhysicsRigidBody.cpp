#include "jpPhysicsRigidBody.h"


jpPhysicsRigidBody::jpPhysicsRigidBody(physx::PxPhysics* px_physics)
{
	//Create a default rigidbody 
	default_material = px_physics->createMaterial(0.5f, 0.5f, 0.5f);
	px_body = physx::PxCreateDynamic(*px_physics, physx::PxTransform(physx::PxIDENTITY()), physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *default_material, 1.0f);
	
	//Detach the shape by default,
	px_body->getShapes(&body_shape, 1);

	//Shape must be activated using calling a diferent function
	//Made like this to separet the rigidbody from the collider
	px_body->detachShape(*body_shape);
	
	body_shape = nullptr;
}

jpPhysicsRigidBody::~jpPhysicsRigidBody()
{
	if(px_body)
		px_body->release();
}

void jpPhysicsRigidBody::ActivateShape()
{
	if (body_shape != nullptr)
		px_body->attachShape(*body_shape);
}

void jpPhysicsRigidBody::DeActivateShape()
{
	if (body_shape != nullptr)
		px_body->detachShape(*body_shape);
	body_shape = nullptr;
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

void jpPhysicsRigidBody::SetMaterial(float &static_friction, float &dynamic_friction, float &restitution)
{
	if (static_friction < 0) static_friction = 0;
	else if (static_friction > PX_MAX_F32) static_friction = PX_MAX_F32-1;

	if (dynamic_friction < 0) dynamic_friction = 0;
	else if (dynamic_friction > PX_MAX_F32) dynamic_friction = PX_MAX_F32-1;

	if (restitution < 0) restitution = 0;
	else if (restitution > 1) restitution = 1;

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

void jpPhysicsRigidBody::SetGeometry(physx::PxVec3 scale, float radius, physx::PxGeometryType::Enum type)
{
	if (radius < 0)
		radius = -radius;
	if(scale.isFinite())
		scale = scale.abs();
	else scale = physx::PxVec3(0.0, 0.0, 0.0);
		
	if (body_shape) {
		px_body->detachShape(*body_shape);
		body_shape = nullptr;
	}
	
	switch (type)
	{
	case physx::PxGeometryType::eSPHERE:
		body_shape = px_body->createShape(physx::PxSphereGeometry(radius), *default_material);
		break;
	case physx::PxGeometryType::ePLANE:
		body_shape = px_body->createShape(physx::PxPlaneGeometry(), *default_material);
		break;
	case physx::PxGeometryType::eCAPSULE:
		body_shape = px_body->createShape(physx::PxCapsuleGeometry(radius, scale.z), *default_material);
		break;
	case physx::PxGeometryType::eBOX: {
		scale = scale*0.5;
		body_shape = px_body->createShape(physx::PxBoxGeometry(0.5,0.5,0.5), *default_material);
	}
		break;
	// TODO: add cooking to create convex mesh ----------
	case physx::PxGeometryType::eCONVEXMESH:
		break;
	case physx::PxGeometryType::eTRIANGLEMESH:
		break;
	case physx::PxGeometryType::eHEIGHTFIELD:
		break;
	default:
		break;
	}
	
}

void jpPhysicsRigidBody::SetShapeScale(physx::PxVec3 scale, float radius)
{
	if (body_shape) {
		// Get absolute values for scale and radius
		if (radius < 0)
			radius = -radius;

		if (!scale.isFinite())
			scale = scale.abs();
		else scale = physx::PxVec3(0.0, 0.0, 0.0);

		// Modify Geometry Scale
		physx::PxGeometryType::Enum type = body_shape->getGeometryType();
		switch (type)
		{
		case physx::PxGeometryType::eSPHERE: {
			physx::PxSphereGeometry sphere = body_shape->getGeometry().sphere();
			sphere.radius = radius;
			body_shape->setGeometry(sphere);
		}
											 break;
		case physx::PxGeometryType::eCAPSULE: {
			physx::PxCapsuleGeometry capsule = body_shape->getGeometry().capsule();
			capsule.halfHeight = scale.z*0.5f;
			capsule.radius = radius;
			body_shape->setGeometry(capsule);
		}
											  break;
		case physx::PxGeometryType::eBOX:{
			physx::PxBoxGeometry box = body_shape->getGeometry().box();
			box.halfExtents = scale*0.5;
			body_shape->setGeometry(box);
		}
			break;
		case physx::PxGeometryType::eCONVEXMESH: {
			physx::PxConvexMeshGeometry convmesh = body_shape->getGeometry().convexMesh();
			convmesh.scale = scale;
			body_shape->setGeometry(convmesh);
		}
			break;
		case physx::PxGeometryType::eTRIANGLEMESH: {
			physx::PxTriangleMeshGeometry trimesh = body_shape->getGeometry().triangleMesh();
			trimesh.scale = scale;
			body_shape->setGeometry(trimesh);
		}
			break;
		case physx::PxGeometryType::eHEIGHTFIELD: {
			physx::PxHeightFieldGeometry heightfield = body_shape->getGeometry().heightField();
			heightfield.heightScale = scale.z;
			body_shape->setGeometry(heightfield);
		}
			break;
		default:
			break;
		}
	}
}

void jpPhysicsRigidBody::SetMass(float & mass)
{
	if (px_body)
		px_body->setMass(mass);
}

void jpPhysicsRigidBody::GetTransform(physx::PxVec3& pos, physx::PxQuat& quat)
{
	physx::PxTransform transf = px_body->getGlobalPose();
	pos = transf.p;
	quat = transf.q;
}
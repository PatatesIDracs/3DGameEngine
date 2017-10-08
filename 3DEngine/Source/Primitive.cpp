#include "Glew\include\glew.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"




// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

vec3 Primitive::GetPosition() const
{
	float3 box_pos = bounding_box.CenterPoint();
	return vec3(transform.M[12] + box_pos.x, transform.M[13] + box_pos.y, transform.M[14] + box_pos.z);
}

void Primitive::CalculateFaceNormals(float* vertices)
{
	float3 normal_temp;
	float3 u_temp;
	float3 v_temp;

	render_data.face_normals_pointer = new float[2*render_data.num_vertices];

	//We are calculating face normals
	for (uint i = 0; i < render_data.num_vertices/3; i++) //divided by 3 since each face has 3 vertices
	{
		float3 vertex_1(vertices[9 * i], vertices[9 * i + 1], vertices[9 * i + 2]);
		float3 vertex_2(vertices[9 * i + 3], vertices[9 * i + 4], vertices[9 * i + 5]);
		float3 vertex_3(vertices[9 * i + 6], vertices[9 * i + 7], vertices[9 * i + 8]);


		//Calculate the dir vector of the normal face
		u_temp = vertex_2 - vertex_1;
		v_temp = vertex_3 - vertex_1;
		normal_temp = math::Cross(u_temp, v_temp);
		normal_temp = normal_temp.Normalized() * 0.25; //Multply by 0.25 since the unit vector is too big for small figures

		//Get the face center (to print the normal from it)
		float3 face_center = (vertex_1 + vertex_2 + vertex_3) / 3;

		//Push booth face render and the normal dir vector to the array
		//Makes the drawing part easier to have both values in the same array



		//First push the face_center data
		render_data.face_normals_pointer[6 * i] = face_center.x;
		render_data.face_normals_pointer[6 * i + 1] = face_center.y;
		render_data.face_normals_pointer[6 * i + 2] = face_center.z;

		//Push the face_center + normal dir vector to the array
		render_data.face_normals_pointer[6 * i + 3] = (face_center.x + normal_temp.x);
		render_data.face_normals_pointer[6 * i + 4] = (face_center.y + normal_temp.y);
		render_data.face_normals_pointer[6 * i + 5] = (face_center.z + normal_temp.z);
	
	}

	//Bind the data to the buffer
	
	render_data.num_face_normals = render_data.num_vertices/3;
}

// ------------------------------------------------------------
void Primitive::Render(bool face_normals) const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	InnerRender();

	if (face_normals)
		FaceNormalsRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

void Primitive::FaceNormalsRender() const
{
}

void Primitive::PrepareToRender()
{
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
oldCube::oldCube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

oldCube::oldCube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void oldCube::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

// SPHERE ============================================
oldSphere::oldSphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

oldSphere::oldSphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;

	stacks = 20;
	slices = 20;




	for (int t = 0; t < stacks; t++)
	{
		float theta1 = ((float)t / stacks)*1*pi;
		float theta2 = ((float)(t + 1) / stacks)*1*pi;

		for (int p = 0; p < slices; p++)
		{
			float phi1 = ((float)p / slices)*2*pi;
			float phi2 = ((float)(p + 1) / slices) *2* pi;

			vec3 vertex1;
			vec3 vertex2;
			vec3 vertex3;
			vec3 vertex4;

			vertex1.z = radius * Sin(theta1) * Cos(phi1);
			vertex1.x = radius * Sin(theta1) * Sin(phi1);
			vertex1.y = radius * Cos(theta1);

			vertex2.z = radius * Sin(theta1) * Cos(phi2);
			vertex2.x = radius * Sin(theta1) * Sin(phi2);
			vertex2.y = radius * Cos(theta1);


			vertex3.z = radius * Sin(theta2) * Cos(phi2);
			vertex3.x = radius * Sin(theta2) * Sin(phi2);
			vertex3.y = radius * Cos(theta2);


			vertex4.z = radius * Sin(theta2) * Cos(phi1);
			vertex4.x = radius * Sin(theta2) * Sin(phi1);
			vertex4.y = radius * Cos(theta2);

			bounding_box.Enclose(vec(vertex1.x, vertex1.y, vertex1.z));
			bounding_box.Enclose(vec(vertex2.x, vertex2.y, vertex2.z));
			bounding_box.Enclose(vec(vertex3.x, vertex3.y, vertex3.z));
			bounding_box.Enclose(vec(vertex4.x, vertex4.y, vertex4.z));


			if (t == 0)
			{
				vertex_array.push_back(vertex1.x);
				vertex_array.push_back(vertex1.y);
				vertex_array.push_back(vertex1.z);

				vertex_array.push_back(vertex4.x);
				vertex_array.push_back(vertex4.y);
				vertex_array.push_back(vertex4.z);


				vertex_array.push_back(vertex3.x);
				vertex_array.push_back(vertex3.y);
				vertex_array.push_back(vertex3.z);
		
			}
			else if (t + 1 == stacks)
			{
				vertex_array.push_back(vertex1.x);
				vertex_array.push_back(vertex1.y);
				vertex_array.push_back(vertex1.z);

				vertex_array.push_back(vertex3.x);
				vertex_array.push_back(vertex3.y);
				vertex_array.push_back(vertex3.z);

				vertex_array.push_back(vertex2.x);
				vertex_array.push_back(vertex2.y);
				vertex_array.push_back(vertex2.z);
			}
			else
			{
				vertex_array.push_back(vertex1.x);
				vertex_array.push_back(vertex1.y);
				vertex_array.push_back(vertex1.z);

				vertex_array.push_back(vertex4.x);
				vertex_array.push_back(vertex4.y);
				vertex_array.push_back(vertex4.z);

				vertex_array.push_back(vertex2.x);
				vertex_array.push_back(vertex2.y);
				vertex_array.push_back(vertex2.z);


				vertex_array.push_back(vertex2.x);
				vertex_array.push_back(vertex2.y);
				vertex_array.push_back(vertex2.z);

				vertex_array.push_back(vertex4.x);
				vertex_array.push_back(vertex4.y);
				vertex_array.push_back(vertex4.z);

				vertex_array.push_back(vertex3.x);
				vertex_array.push_back(vertex3.y);
				vertex_array.push_back(vertex3.z);
			}
		}
	}
	render_data.num_vertices = vertex_array.size()/3;
	
	CalculateFaceNormals(&vertex_array[0]);
}

oldSphere::~oldSphere()
{
}

void oldSphere::InnerRender() const
{
	GLuint sphere_id = 0;
	
	//Face render
	glGenBuffers(1, (GLuint*)&sphere_id);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glBufferData(GL_ARRAY_BUFFER, vertex_array.size() * sizeof(float), &vertex_array[0], GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, vertex_array.size()/3);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDeleteBuffers(1, &sphere_id);




}

void oldSphere::FaceNormalsRender() const
{
	//Face normal render
	glGenBuffers(1, (GLuint*)&render_data.id_face_normals);
	glBindBuffer(GL_ARRAY_BUFFER, render_data.id_face_normals);
	glBufferData(GL_ARRAY_BUFFER, render_data.num_face_normals * 6 * sizeof(float), &render_data.face_normals_pointer[0], GL_STATIC_DRAW);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, render_data.id_face_normals);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_LINES, 0, render_data.num_face_normals * 2);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &render_data.id_face_normals);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}


// CYLINDER ============================================
oldCylinder::oldCylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

oldCylinder::oldCylinder(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void oldCylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = (float)(i * M_PI / 180); // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a =(float) (i * M_PI / 180); // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = (float)(i * M_PI / 180); // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
oldLine::oldLine() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

oldLine::oldLine(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void oldLine::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
oldPlane::oldPlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

oldPlane::oldPlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void oldPlane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for(float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}
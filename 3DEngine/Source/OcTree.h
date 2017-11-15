#ifndef __OCTREE_H__
#define __OCTREE_H__

#include "Globals.h"
#include "Math.h"

#define MAX_NODES 8

// Octree Item
template <class DATA_TYPE>
struct Octree_Item
{
public:
	// Constructor ----------------------
	Octree_Item(DATA_TYPE DATA_TYPE, AABB box): item_data(DATA_TYPE), bounding_box(box) {};

public:

	AABB								bounding_box;
	DATA_TYPE							item_data = NULL;

public:

	bool operator == (const DATA_TYPE data) {
		return item_data == data;
	}
};

// Octree Node: cubes
template <class DATA_TYPE>
class Octree_Node	
{
public:
	// Constructor
	Octree_Node(const AABB& box, uint max_objects): box(box), max_objects(max_objects) {};

	// Destructor
	// TODO -------------------
	~Octree_Node() {
		objects.clear();

		if (full) {
			for (uint i = 0; i < MAX_NODES; i++) 
				delete childs[i];
 		}
	};

public:
	// Octree Node variables -----------------------
	AABB								box;
	
	Octree_Node*						root = nullptr;
	Octree_Node*						childs[MAX_NODES];

	uint								max_objects = 0;
	std::vector<Octree_Item<DATA_TYPE>>	objects;

	bool								full = false;

public:
	// Octree Node public methods ------------------

	void Draw(float width, float4 color) const {
		box.Draw(width, color);

		if (full) {
			for (uint i = 0; i < MAX_NODES; i++)
				childs[i]->Draw(width,color);
		}

	}

	bool Insert(DATA_TYPE data, const AABB& box) {
		
		if (!ContainsAABB(box)){
			return false;
		}

		if (full) {
			uint count = 0;
			uint index = 0;
			for (uint i = 0; i < MAX_NODES; i++) {
				if(childs[i]->ContainsAABB(box))
				{
					count++;
					index = i;
				}
			}

			if (count == 1) {
				childs[index]->Insert(data, box);
			}
			else if(count > 1) {
				objects.push_back(Octree_Item<DATA_TYPE>(data, box));
			}
			else return false;
		}
		else {
			if (objects.size() < max_objects) {

				objects.push_back(Octree_Item<DATA_TYPE>(data, box));
				if (objects.size() == max_objects) {
					full = true;
					Subdivide();
				}
			}
		}
		return false;
	}

	bool ContainsAABB(const AABB& box) {
		return (this->box.Contains(box)||this->box.Intersects(box));
	}

	void Subdivide() {
		if (!full) return;

		float3 box_half_size = box.HalfSize();
		AABB childs_center;
		childs_center.SetFromCenterAndSize(box.CenterPoint(), box_half_size);

		AABB child_box;
		for (uint i = 0; i < MAX_NODES; i++) {
			child_box.SetFromCenterAndSize(childs_center.CornerPoint(i), box_half_size);
			childs[i] = new Octree_Node(child_box, max_objects);
			childs[i]->root = this;
		}

		std::vector<Octree_Item<DATA_TYPE>> temporal;

		for (uint i = 0; i < objects.size(); i++) {
			uint count = 0;
			uint index = 0;

			for (uint j = 0; j < MAX_NODES; j++) {
				if (childs[j]->ContainsAABB(objects[i].bounding_box))
				{
					count++;
					index = j;
				}
			}

			if (count == 1) {
				childs[index]->Insert(objects[i].item_data, objects[i].bounding_box);
			}
			else {
				temporal.push_back(objects[i]);
			}
		}
		objects.clear();
		objects = temporal;
	}

	// Intersect with Frustum
	bool Intersect(std::vector<DATA_TYPE>& candidates, const Frustum& frustum) {
		
		if (!(box.Contains(frustum) || box.Intersects(frustum))) return false;
		
		for(uint i = 0; i < objects.size(); i++){
			if (objects[i].bounding_box.Contains(frustum) || objects[i].bounding_box.Intersects(frustum))
			{
				candidates.push_back(objects[i].item_data);
			}
		}

		if (full) {
			for (uint i = 0; i < MAX_NODES; i++) {
				childs[i]->Intersect(candidates, frustum);
			}
		}
		return false;
	}

	bool Intersect(std::vector<DATA_TYPE>& candidates, const LineSegment& linesegment, std::vector<float>& hit_dist) {

		if (!(box.Contains(linesegment) || box.Intersects(linesegment))) return false;

		float ndist = 0;
		float fdist = 0;
		Ray raycast = linesegment.ToRay();
		for (uint i = 0; i < objects.size(); i++) {
			if (raycast.Intersects(objects[i].bounding_box, ndist, fdist))
			{
				candidates.push_back(objects[i].item_data);
				hit_dist.push_back(ndist);
			}
		}

		if (full) {
			for (uint i = 0; i < MAX_NODES; i++) {
				childs[i]->Intersect(candidates, linesegment, hit_dist);
			}
		}
		return true;
	}
};

// Oct-Tree
template <class DATA_TYPE>

class Octree
{
public:
	// Constructor
	Octree() {};
	Octree(const AABB& box, uint max_objects): max_objects(max_objects) {
		root = new Octree<DATA_TYPE>(box, max_objects)
	};

	// Destructor
	~Octree() {
		Clear();
	};


private:
	// Octree Variables ----------------------------
	Octree_Node<DATA_TYPE>*				root = nullptr;
	uint								max_objects = 0;

public:
	// Octree public methods -----------------------
	//Create(AABB limits)  Clear()  Insert(GameObject*)  Remove(GameObject*)  Intersect(vector<GameObject*>&, PRIMITIVE)

	void Create(const AABB& root_box, uint max_objects) {
		this->max_objects = max_objects;

		if (root != nullptr) {
			root->box = root_box;
			root->max_objects = max_objects;
		}
		else {
			root = new Octree_Node<DATA_TYPE>(root_box, max_objects);
		}
	}

	void Clear() {
		if (root != nullptr) {
			delete root; 
			root = nullptr;
		}
	}

	void Draw(float width, float4 color) {
		if (root != nullptr) {
			root->Draw(width,color);
		}
	}

	bool Insert(const DATA_TYPE data, const AABB& box) {

		if (root != nullptr) {
			if(box.IsFinite())
				return root->Insert(data, box);
		}
		return false; 
	};

	bool Reset() {
		
		if (root != nullptr) {
			AABB root_box = root->box;
			Clear();

			root = new Octree_Node<DATA_TYPE>(root_box, max_objects);
			return true;
		}
		return false;
	};

	// TODO -------------------
	// should add Frustum, AABB, Sphere and Ray
	bool Intersect(std::vector<DATA_TYPE>& objects, const Frustum& frustum) {
		
		if (root != nullptr)
		{
			return root->Intersect(objects, frustum);
		}
		return false; 
	}

	bool Intersect(std::vector<DATA_TYPE>& objects, const LineSegment& linesegment, std::vector<float>& hit_dist) {

		if (root != nullptr)
		{
			return root->Intersect(objects, linesegment, hit_dist);
		}
		return false;
	}
};

#endif // !__OCTREE_H__
#ifndef _RENDER_RESOURCE_H_
#define _RENDER_RESOURCE_H_

#ifdef __cplusplus

#include <cstdint>
#define uint uint32_t

#else
#endif

struct StaticMeshRenderResource
{
	uint scene_constant_;
	uint frustum_;
	uint object_constant_;
	uint mesh_constant_;
	uint material_constant_;
	uint vertices_;
	uint vertex_indices_;
	uint meshlets_;
	uint primitives_;
	uint instance_index_;
};

struct SkeletalMeshRenderResource
{
	uint scene_constant_;
	uint object_constant_;
	uint mesh_constant_;
};

#endif //_RENDER_RESOURCE_H_

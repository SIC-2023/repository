#pragma once

#include <string>

#include "../GameObject/GameObject.h"

struct DirectionLight;
struct CameraData;

namespace argent::rendering
{
	class RenderContext;
	class RenderingManager;
}

namespace argent::component
{
	class Light;
	class Camera;
}

namespace argent::scene
{
	//TODO　仮置き　いずれエディタ上だけの存在にしたい
	class BaseScene
	{
		friend class rendering::RenderingManager;
	public:
		BaseScene(const char* name);
		virtual ~BaseScene() = default;

		virtual void Awake();
		virtual void Shutdown();

		void Update();

		void OnGui();

		const std::string& GetName() const { return name_; }
		const char* GetNameCStr() const { return name_.c_str(); }


		GameObject* AddObject(std::string name);


		void RegisterDirectionLight(component::Light* light);
		void RegisterCamera(component::Camera* camera);

	private:
		
		std::vector<DirectionLight> AccumulateDirectionLightData(std::vector<DirectionLight>& direction_lights) const;
		std::vector<CameraData> AccumulateCameraData(std::vector<CameraData>& camera_data);


	private:
		std::string name_;
		std::vector<std::unique_ptr<GameObject>> game_objects_;

		component::Light* light_;
		component::Camera* camera_;

	};
}


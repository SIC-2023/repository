#pragma once
#include <memory>
#include <unordered_map>

#include <string_view>

namespace argent
{
	class Subsystem
	{
	public:
		Subsystem() = default;
		virtual ~Subsystem() = default;

		//ãNìÆéû
		virtual void OnAwake() {}

		//èIóπéû
		virtual void OnShutdown() {}
	};

	class SubsystemLocator
	{
	public:
		SubsystemLocator() = default;

	public:
		SubsystemLocator(const SubsystemLocator&) = delete;
		SubsystemLocator operator=(const SubsystemLocator&) = delete;

		~SubsystemLocator() = default;

		template<class T> std::shared_ptr<T> Get()
		{
			CertificateSubsystemType<T>();
			if (!subsystems_.contains(typeid(T).hash_code())) return nullptr;
			return std::static_pointer_cast<T>(subsystems_[typeid(T).hash_code()]);
		}

		template<class T> void Add()
		{
			CertificateSubsystemType<T>();
			std::shared_ptr<T> sp_subsystem = std::make_shared<T>();
			if (!sp_subsystem) return;
			subsystems_[typeid(T).hash_code()] = sp_subsystem;
			sp_subsystem->OnAwake();
		}

		template<class T> void Remove()
		{
			CertificateSubsystemType<T>();
			auto sp_subsystem = subsystems_[typeid(T).hash_code()];
			if(sp_subsystem)
			{
				sp_subsystem->OnShutdown();
			}
		}

	private:

		template<class T>
		void CertificateSubsystemType()
		{
			static_assert(std::is_base_of<Subsystem, T>::value, "SubsystemÇ≈ÇÕÇ»Ç¢å^Ç™éwíËÇ≥ÇÍÇ‹ÇµÇΩ");
		}

	private:
		std::unordered_map<size_t, std::shared_ptr<Subsystem>> subsystems_;

	};
}


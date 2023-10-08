#pragma once
#include "GameObject.h"
#include "Camera.h"
#include <unordered_map>
#include <string_view>
#include <utility>
namespace Iris {
	class Scene;
	static Scene* s_activeScene;
	Scene* getActiveScene() {
		if (s_activeScene == nullptr) {
			ERROR("There is currently no active scene!");
			return nullptr;
		}
		return s_activeScene;
	}
	class Scene {
	public:
		template <typename T, typename... Args>
		void createObject(const char* id, Args&&... args) {
			if (!std::is_base_of<GameObject, T>::value) {
				ERROR("Passed object isn't derived from GameObject class!");
				return;
			}
			if (m_gameObjects.contains(id)) {
				ERROR("Object with identifier \"{}\" already exists!", id);
				return;
			}
			m_gameObjects.emplace(id, new T(std::forward<Args>(args)...));
			m_gameObjects.at(id)->onCreate();
		}
		GameObject* getObject(const char* id) {
			if (!m_gameObjects.contains(id)) {
				ERROR("Object with identifier \"{}\" doesn't exists!", id);
				return nullptr;
			}
			return m_gameObjects.at(id);
		}
		template <typename T>
		T* getObject(const char* id) {
			if (!m_gameObjects.contains(id)) {
				ERROR("Object with identifier \"{}\" doesn't exists!", id);
				return nullptr;
			}
			if (typeid(T) != typeid(*m_gameObjects.at(id))) {
				ERROR("Object isn't of type \"{}\"!", typeid(m_gameObjects.at(id)).name());
				return nullptr;
			}
			return (T*)m_gameObjects.at(id);
		}
		void update() {
			for (auto& gameObject : m_gameObjects) {
				gameObject.second->onUpdate();
			}
		}
		void deleteObject(const char* identifier) {
			if (!m_gameObjects.contains(identifier)) {
				ERROR("Object with identifier \"{}\" doesn't exists!", identifier);
				return;
			}
			m_gameObjects.at(identifier)->onDestroy();
			m_gameObjects.erase(identifier);
		}
		std::string getListOfObjects() {
			std::stringstream list;
			list << "-SCENE-\n";
			for (auto& gameObject : m_gameObjects) {
				list << typeid(*gameObject.second).name() << " - "  << gameObject.first << "\n";
			}
			return list.str();
		}
	private:
		std::unordered_map<std::string_view, GameObject*> m_gameObjects;
	};
}
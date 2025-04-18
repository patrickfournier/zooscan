#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "json/single_include/nlohmann/json.hpp"

namespace ZooLib
{
    class StateComponent;

    class State
    {
        std::filesystem::path m_FilePath{};
        std::vector<StateComponent *> m_StateComponents;

    public:
        ~State();

        void AddStateComponent(StateComponent *stateComponent)
        {
            if (std::ranges::find(m_StateComponents, stateComponent) == m_StateComponents.end())
            {
                m_StateComponents.push_back(stateComponent);
            }
        }

        void RemoveStateComponent(StateComponent *stateComponent)
        {
            if (const auto it = std::ranges::find(m_StateComponents, stateComponent); it != m_StateComponents.end())
            {
                m_StateComponents.erase(it);
            }
        }

        template<typename TStateComponent>
        TStateComponent *GetStateComponentByType() const
        {
            for (const auto &stateComponent: m_StateComponents)
            {
                auto sc = dynamic_cast<TStateComponent *>(stateComponent);
                if (sc != nullptr)
                {
                    return sc;
                }
            }

            return nullptr;
        }

        void SetFilePath(const std::filesystem::path &filePath)
        {
            m_FilePath = filePath;
        }

        template<typename TStateComponent>
        void LoadFromFile(TStateComponent *stateComponent)
        {
            if (stateComponent == nullptr)
            {
                return;
            }

            const std::string &key = stateComponent->GetSerializationKey();
            if (key.empty())
            {
                return;
            }

            std::ifstream f(m_FilePath);
            if (!f.good())
            {
                return;
            }

            try
            {
                nlohmann::json jsonData = nlohmann::json::parse(f);

                if (jsonData.contains(key))
                {
                    from_json(jsonData[key], *stateComponent);
                }
            }
            catch (const std::exception &e)
            {
            }

            f.close();
        }

        template<typename TStateComponent>
        void SaveToFile(TStateComponent *stateComponent)
        {
            if (stateComponent == nullptr)
            {
                return;
            }

            const std::string &key = stateComponent->GetSerializationKey();
            if (key.empty())
            {
                return;
            }

            std::ifstream f(m_FilePath);
            nlohmann::json jsonData;
            if (f.good())
            {
                try
                {
                    jsonData = nlohmann::json::parse(f);
                }
                catch (const std::exception &e)
                {
                }

                f.close();
            }

            nlohmann::json componentJson;
            to_json(componentJson, *stateComponent);
            jsonData[key] = componentJson;

            auto dirs = m_FilePath.parent_path();
            create_directories(dirs);

            std::ofstream outFile(m_FilePath);
            outFile << jsonData.dump(4);
            outFile.close();
        }
    };
}

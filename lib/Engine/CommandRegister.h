#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

#include "../CommandCore/Command.h"

class CommandRegister {
public:
    template <class NewCommand>
    void RegisterCommand(const std::string& name) {
        commands_[name] = std::make_unique<NewCommand>();
    }

    InterfaceCommand* GetCommand(const std::string& command) {
        auto it = commands_.find(command);

        if (it != commands_.end()) {
            return it->second.get();
        }

        return nullptr;
    }
private:
    std::unordered_map<std::string, std::unique_ptr<InterfaceCommand>> commands_;
};

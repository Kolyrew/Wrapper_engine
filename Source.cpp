#include <iostream>
#include <functional>
#include <sstream>
#include <unordered_map>

class Subject {
public:
    int f3(int arg1, int arg2) {
        return arg1 + arg2;
    }
};

template <typename T>
static std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <typename... Args>
class Wrapper {
public:
    using MethodType = std::function<int(Args...)>;

    Wrapper(MethodType method, std::unordered_map<std::string, int> defaultArgs)
        : method(method), defaultArgs(defaultArgs) {}

    int invoke(std::unordered_map<std::string, int>& args) {
        for (auto& pair : defaultArgs) {
            if (args.find(pair.first) == args.end()) {
                
                args[pair.first] = pair.second;
            }
        }
        return invoke_helper(args, std::index_sequence_for<Args...>());
    }

private:
    MethodType method;
    std::unordered_map<std::string, int> defaultArgs;

    template <std::size_t... Is>
    int invoke_helper(const std::unordered_map<std::string, int>& args, std::index_sequence<Is...>) {
        return method(args.at(get_arg_name<Is>())...);
    }

    template <std::size_t I>
    static std::string get_arg_name() {
        return "arg" + to_string(I + 1);
    }
};

class Engine {
public:
    void register_command(const std::string& command, Wrapper<int, int>& wrapper) {
        commands[command] = [&wrapper](std::unordered_map<std::string, int>& args) {
            return wrapper.invoke(args);
            };
    }

    int execute(const std::string& command, std::unordered_map<std::string, int>& args) {
        if (commands.find(command) != commands.end()) {
            return commands[command](args);
        }
        return 0;
    }

private:
    std::unordered_map<std::string, std::function<int(std::unordered_map<std::string, int>&)>> commands;
};

int main() {
    Subject subj;
    Wrapper<int, int> wrapper([&subj](int arg1, int arg2) { return subj.f3(arg1, arg2); }, { {"arg1", 0}, {"arg2", 0} });
    Engine engine;
    engine.register_command("command1", wrapper);
    std::unordered_map<std::string, int> args = { {"arg1", 4}, {"arg2", 5} };
    std::cout << engine.execute("command1", args) << std::endl;

    return 0;
}
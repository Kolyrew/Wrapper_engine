#include <iostream>
#include <functional>
#include <sstream>
#include <unordered_map>

class Subject {
public:
    int sum(int arg1, int arg2) {
        return arg1 + arg2;
    }
};

// Перемещаем to_string выше get_arg_name
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
                // Используем at() для бросания исключения при отсутствии ключа
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

int main() {
    Subject subj;
    Wrapper<int, int> wrapper([&subj](int arg1, int arg2) { return subj.sum(arg1, arg2); }, { {"arg1", 0}, {"arg2", 0} });

    return 0;
}
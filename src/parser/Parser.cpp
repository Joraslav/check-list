#include "Parser.hpp"

#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <cctype>
#include <charconv>

namespace parser
{
    enum class TypeCommand {ADD, LIST, CLEAR, DONE, REMOVE, EDIT, HELP};

    static TypeCommand FormatInEnum(const std::string& type)
    {
        std::unordered_map<std::string, TypeCommand> u_map = 
        {
            {"add", TypeCommand::ADD},
            {"list", TypeCommand::LIST},
            {"clear", TypeCommand::CLEAR},
            {"done", TypeCommand::DONE},
            {"remove", TypeCommand::REMOVE},
            {"edit", TypeCommand::EDIT},
            {"help", TypeCommand::HELP},
        };

        auto it = u_map.find(type);
        if (it != u_map.end()) 
            return it->second;
        else 
            throw std::invalid_argument("Error: unknown command");
    }

    static void ValidCommandWords(const TypeCommand& check_type, int count)
    {
        bool is_type_checked = (check_type == TypeCommand::ADD) || 
                    (check_type == TypeCommand::DONE) || 
                    (check_type == TypeCommand::REMOVE);

        const std::string message = "Error: invalid number of command arguments";
        if ((count != 3) && is_type_checked)
            throw std::invalid_argument(message);

        if (count != 4 && check_type == TypeCommand::EDIT)
            throw std::invalid_argument(message);

        if (count > 3 && check_type == TypeCommand::LIST)
            throw std::invalid_argument(message);
    }

    static int ConvertWordInNumber(const std::string& str)
    {
        int number = 0;
        const char* first = str.data();
        const char* last = first + str.size();
        while (first != last && (*first == ' ' || *first == '\t'))
            ++first;

        auto res = std::from_chars(str.data(), str.data() + str.size(), number);
        if (res.ec != std::errc() || res.ptr != str.data() + str.size())
            throw std::invalid_argument("Error: Extra chars");
        return number;
    }

    static std::string ConvertToLower(const std::string& str)
    {
        std::string convert_str = str;
        for (char& c : convert_str)
            c = std::tolower(c);
        
        return convert_str;
    }

    void Parser::Parse(const int& argc, const char** argv)
    {
        if (argc < 2)
            throw std::invalid_argument("Error: no command entered");
        
        std::string type_str = ConvertToLower(std::string(argv[1]));
        TypeCommand type = FormatInEnum(type_str);
        ValidCommandWords(type, argc);
        switch (type)
        {
            case TypeCommand::ADD:
            {    
                command.type_ = "add";
                for (int i = 2; i < argc; ++i)
                {
                    std::string tmp_str = ConvertToLower(std::string(argv[i]));
                    command.task_text_ += " " + tmp_str;
                }
                break;
            }
            case TypeCommand::LIST:
            {    
                command.type_ = "list";
                if (argc == 3)
                {    
                    std::string type_list = ConvertToLower(std::string(argv[2]));
                    if (type_list != "pending" && type_list != "completed")
                        throw std::invalid_argument("Error: unknown list options");
                    command.task_text_ = type_list;
                }
                break;
            }
            case TypeCommand::CLEAR:
            {    
                command.type_ = "clear";
                break;
            }
            case TypeCommand::DONE:
            {    
                command.type_ = "done";
                command.id_ = ConvertWordInNumber(argv[2]);
                break;
            }
            case TypeCommand::REMOVE:
            {    
                command.type_ = "remove";
                command.id_ = ConvertWordInNumber(argv[2]);
                break;
            }
            case TypeCommand::EDIT:
            {    
                command.id_ = ConvertWordInNumber(argv[2]);
                for (int i = 3; i < argc; ++i)
                {    
                    std::string tmp_str = ConvertToLower(std::string(argv[i]));
                    command.task_text_ += " " + tmp_str;
                }
                break;
            }
            case TypeCommand::HELP:
            {   
                command.type_ = "help";
                break;
            }
            default:
            {    
                throw std::runtime_error("Error: unknown command");
                break;
            }
        }   
    }
} // namespace parser

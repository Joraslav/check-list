#include "Parser.hpp"
#include <stdexcept>
#include <type_traits>

namespace parse
{
    enum class TypeCommand {ADD, LIST, CLEAR, DONE, REMOVE, EDIT, CONFIG, HELP};

    static TypeCommand FormatInEnum(const std::string type)
    {
        if (type == "add") 
            return TypeCommand::ADD;
        else if (type == "list")
            return TypeCommand::LIST;
        else if (type == "clear")
            return TypeCommand::CLEAR;
        else if (type == "done")
            return TypeCommand::DONE;
        else if (type == "remove")
            return TypeCommand::REMOVE;
        else if (type == "edit")
            return TypeCommand::EDIT;
        else if (type == "config")
            return TypeCommand::CONFIG;
        else if (type == "help")
            return TypeCommand::HELP;
        else 
        {
            const std::string message = "Error: unknow command";
            throw std::runtime_error(message);
        }
    }

    static void ValidCommandWords(const TypeCommand& check_type, int count)
    {
        bool is_type_checked = (check_type == TypeCommand::ADD) || 
                    (check_type == TypeCommand::DONE) || 
                    (check_type == TypeCommand::REMOVE) || 
                    (check_type == TypeCommand::EDIT) || 
                    (check_type == TypeCommand::CONFIG);

        if ((count < 3 || count > 3) && is_type_checked)
        {
            const std::string message = "Error: invalid number of command arguments";
            throw std::runtime_error(message);
        }
    }

    static int ConvertWordInNumber(const std::string& str)
    {
        size_t pos = 0;
        int number = std::stoi(str, &pos);
        if (pos != str.size())
            throw std::runtime_error("Error: Extract chars");
        return number;
    }

    void Parser::Parse(const int& argc, const char** argv)
    {
        if (argc < 2)
            throw std::runtime_error("Error: no command entered");
        
        TypeCommand type = FormatInEnum(std::string(argv[1]));
        ValidCommandWords(type, argc);
        switch (type)
        {
            case TypeCommand::ADD:
            {    
                command.type_ = "add";
                for (int i = 2; i < argc; ++i)
                    command.task_text_ += " " + std::string(argv[i]);
                break;
            }
            case TypeCommand::LIST:
            {    

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

                break;
            }
            case TypeCommand::CONFIG:
            {   

                break;
            }
            case TypeCommand::HELP:
            {   
                command.type_ = "help";
                break;
            }
            default:
                break;
        }   
    }
} // namespace parse

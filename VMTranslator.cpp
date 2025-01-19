//This code is a VM translator which translates VM code from nand to tetris course to Hack machine code........................................
//written by Ameer Ikbal.......................................................................................................................

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
namespace fs = std::filesystem;

enum command_type
{
    C_ARITHMETIC,C_PUSH,C_POP,C_LABEL,C_GOTO,C_IF,C_FUNCTION,C_RETURN,C_CALL,C_ERROR
};



//parser module=================================================================================================================================================================
//====================================================================================================================================================================================
class parser
{
private:
    std::ifstream filehandle;
    std::vector<std::string> commands;
    int curr_line_number = 0;  //stores the current line number
    std::string current_command[3]; //stores the current command
    command_type current_type;

public:
    explicit parser(const std::string filename)
    {
        filehandle.open(filename);
        if (!filehandle.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        read_string();
    }





    //cleans the string and removes comments and empty lines..................................................................................................................
    void clean_store_string(std::string li)
    {
        std::string no_comment_line;
        for (int i = 0;i < li.size(); i++)
        {
            if(li[i] == '/' && li[i+1] == '/')
            {
                break;
            }
            no_comment_line += li[i];
            
        }

        if(no_comment_line.size() > 0)
        {
            commands.push_back(no_comment_line);
        }
    }

    //read each line and filter it....................................................................................................................................................

    void read_string()
    {
        std::string line;
        while(filehandle.peek()!=EOF)
        {
            getline(filehandle,line);

            clean_store_string(line);

        }
    }








    //checking if there is more commands or not-----------------------------------------------------------------------------------------------------------------------------------------------

    bool has_more_commands()
    {
        if(curr_line_number == commands.size())
        {
            return false;
        }
        return true;

    }


    //split the current command into its part and store it in the current command array.    update when dealing with functions......................................................................................
    void split_and_store(std::string comm)
    {
        int sc = 0;
        for(char c : comm)
        {
            if(c == ' ')
            {
                sc += 1;
                continue;
            }
                
            current_command[sc].push_back(c);
        }
    }


    //this function advances to the next command and store it in an array_________________________________________________________________________________________________________

    void advance()
    {
        for(int i = 0; i < 3; i++) //reset the current command string to empty
             current_command[i] = "";
        std::string temp_comm;

        temp_comm = commands[curr_line_number];


        split_and_store(temp_comm);

        current_command_type();
        curr_line_number += 1;
    }

    //prints the content of current commands---------------------------------------------------------------------------------------------------------------------------------------
    void print_current_command()
    {
        for(std::string s : current_command)
        {
            std::cout << s << " ";
        }

        std::cout << std::endl;
    }
    

    //returns the command type for the current command selected by advance----------------------------------------------------

    void current_command_type()
    {
        if(current_command[0] =="add" || current_command[0] == "sub" || current_command[0] == "neg" || current_command[0] == "eq" || current_command[0] == "gt" || current_command[0] == "lt" || current_command[0] == "and" || current_command[0] == "or" || current_command[0] == "not")
        {
            current_type = command_type::C_ARITHMETIC;
        }


        else if(current_command[0] == "pop")
        {
            current_type = command_type::C_POP;
        }

        else if(current_command[0] == "push")
        {
            current_type = command_type::C_PUSH;
        }

        else if(current_command[0] == "label")
        {
            current_type = command_type::C_LABEL;
        }
        else if (current_command[0] == "goto")
        {
            current_type = command_type::C_GOTO;
        }
        else if (current_command[0] == "if-goto")
        {
            current_type = command_type::C_IF;
        }
        else if(current_command[0] == "function")
        {
            current_type = command_type::C_FUNCTION;
        }
        else if(current_command[0] == "call")
        {
            current_type = command_type::C_CALL;
        }

        else if(current_command[0]== "return")
        {
            current_type = command_type::C_RETURN;
        }
        else
        {
            current_type = command_type::C_ERROR;
        }

    }

    //returns the current command type

    command_type ret_commandtype()
    {
        return current_type;
    }


    //return argument 1 --- requires update when writing for function.
    std::string argument_1()
    {
        if(current_type == command_type::C_ARITHMETIC)
        {
            return current_command[0];
        }
        else 
        {
            return current_command[1];
        }
    }


    //return argument 2 -- requires updates for functions labels and goto
    std::string argument_2()
    {
        return current_command[2];
    }

};


// code writer class================================================================================================================================================================
// =====================================================================================================================================================================================

class code_writer
{
private:
    std::ofstream output_handle;
    std::string code;
    std::string static_name;
    int label_count = 0;


private:


    //These two functions contain the code to implement the push code================================================================================================================
    void push_code()
    {
            code.append("@0\n");
            code.append("A=M\n");
            code.append("M=D\n");
            code.append("D=A+1\n");
            code.append("@0\n");
            code.append("M=D\n");
    }
    void push_code_generator(std::string arg1,std::string arg2)
    {
        code.append(std::string("//push") + " " + arg1 + " " + arg2 + "\n");

        if(arg1 == "constant")
        {
            code.append("@"+arg2+"\n");
            code.append("D=A\n");
            push_code();
        }
        else if (arg1 == "local" || arg1 == "argument" || arg1 == "this" || arg1 == "that")
        {
            if(arg1 == "local")
            {
                code.append("@1\n");
            }
            else if(arg1 == "argument")
            {
                code.append("@2\n");
            }
            else if(arg1 == "this")
            {
                code.append("@3\n");
            }
            else if(arg1 == "that")
            {
                code.append("@4\n");
            }

            code.append("A=M\n");
            code.append("D=A\n");
            code.append(std::string("@") + arg2 + "\n");
            code.append("A=D+A\n");
            code.append("D=M\n");
            push_code();
        }


        else if(arg1 == "static")
        {
            code.append(std::string("@") + static_name + "." + arg2 + "\n");
            code.append("D=M\n");
            push_code();
        }

        else if(arg1 == "temp")
        {
            code.append("@5\n");
            code.append("D=A\n");
            code.append(std::string("@") + arg2 + "\n");
            code.append("A=D+A\n");
            code.append("D=M\n");
            push_code();

        }
        else if(arg1 == "pointer")
        {
            if(arg2 == "0")
            {
                code.append("@3\n");
                code.append("D=M\n");
                push_code();
            }
            else if(arg2 == "1")
            {
                code.append("@4\n");
                code.append("D=M\n");
                push_code();
            }
        }

    }


    //POP code gerating functions=====================================================================================================

    void pop_code_generator(std::string arg1,std::string arg2)
    {
        code.append(std::string("//pop") + " " + arg1 + " " + arg2 + "\n");

        //calculating the address for various memory sengment.............................................................................
        if (arg1 == "local" || arg1 == "argument" || arg1 == "this" || arg1 == "that")
        {
            if(arg1 == "local")
            {
                code.append("@1\n");
            }
            else if(arg1 == "argument")
            {
                code.append("@2\n");
            }
            else if(arg1 == "this")
            {
                code.append("@3\n");
            }
            else if(arg1 == "that")
            {
                code.append("@4\n");
            }

            code.append("D=M\n");
            code.append(std::string("@") + arg2 + "\n");
            code.append("D=D+A\n");
            code.append("@addr\n");
            code.append("M=D\n");
            

        }


        else if(arg1 == "static")
        {
            code.append(std::string("@") + static_name + "." + arg2 + "\n");
            code.append("D=A\n");
            code.append("@addr\n");
            code.append("M=D\n");
        }

        else if(arg1 == "temp")
        {
            code.append("@5\n");
            code.append("D=A\n");
            code.append(std::string("@") + arg2 + "\n");
            code.append("D=D+A\n");
            code.append("@addr\n");
            code.append("M=D\n");


        }
        else if(arg1 == "pointer")
        {
            if(arg2 == "0")
            {
                code.append("@3\n");
                code.append("D=A\n");
                code.append("@addr\n");
                code.append("M=D\n");
            }
            else if(arg2 == "1")
            {
                code.append("@4\n");
                code.append("D=A\n");
                code.append("@addr\n");
                code.append("M=D\n");
            }
        }
        //this code takes the value in the sp and store it in the memry space we calculated and stored in the addr.
        code.append("@0\n");
        code.append("A=M-1\n");
        code.append("D=M\n");
        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=D\n");
        code.append("@0\n");
        code.append("A=M\n");
        code.append("A=A-1\n");
        code.append("D=A\n");
        code.append("@0\n");
        code.append("M=D\n");
    }

    void add_code_generator()
    {
        code.append("//add\n");
        code.append("@0\n");
        code.append("A=M\n");
        code.append("A=A-1\n");
        code.append("D=M\n");
        code.append("A=A-1\n");
        code.append("M=D+M\n");
        code.append("A=A+1\n");
        code.append("D=A\n");
        code.append("@0\n");
        code.append("M=D\n");
    }

    void sub_code_generator()
    {
        code.append("//add\n");
        code.append("@0\n");
        code.append("A=M\n");
        code.append("A=A-1\n");
        code.append("D=M\n");
        code.append("A=A-1\n");
        code.append("M=M-D\n");
        code.append("A=A+1\n");
        code.append("D=A\n");
        code.append("@0\n");
        code.append("M=D\n");
    }

    void neg_code_generator()
    {
        code.append("//neg\n");
        code.append("@0\n");
        code.append("A=M-1\n");
        code.append("M=-M\n");
    }


    void eq_code_generator()
    {
        code.append("//eq\n");
        code.append("@0\n");
        code.append("A=M\n");
        code.append("D=A-1\n");
        code.append("D=D-1\n");
        code.append("@addr\n");
        code.append("M=D\n");

        code.append("@0\n");
        code.append("A=M\n");
        code.append("A=A-1\n");
        code.append("D=M\n");
        code.append("A=A-1\n");
        code.append("D=D-M\n");

        code.append(std::string("@label") + std::to_string(label_count) + "\n");
        code.append("D;JEQ\n");
        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=0\n");
        code.append(std::string("@next" + std::to_string(label_count) + "\n"));
        code.append("0;JMP\n");

        code.append(std::string("(label") + std::to_string(label_count) + ")\n");
        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=-1\n");

        code.append(std::string("(next" + std::to_string(label_count) + ")\n"));

        code.append("@addr\n");
        code.append("D=M+1\n");
        code.append("@0\n");
        code.append("M=D\n");

        label_count += 1;
    }

    void gt_code_generator()
    {
        code.append("//gt\n");
        code.append("@0\n");
        code.append("A=M\n");
        code.append("D=A-1\n");
        code.append("D=D-1\n");
        code.append("@addr\n");
        code.append("M=D\n");

        code.append("@0\n");
        code.append("A=M\n");
        code.append("A=A-1\n");
        code.append("D=M\n");
        code.append("A=A-1\n");
        code.append("D=M-D\n");

        code.append(std::string("@label") + std::to_string(label_count) + "\n");
        code.append("D;JGT\n");
        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=0\n");
        code.append(std::string("@next" + std::to_string(label_count) + "\n"));
        code.append("0;JMP\n");

        code.append(std::string("(label") + std::to_string(label_count) + ")\n");
        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=-1\n");

        code.append(std::string("(next" + std::to_string(label_count) + ")\n"));

        code.append("@addr\n");
        code.append("D=M+1\n");
        code.append("@0\n");
        code.append("M=D\n");

        label_count += 1;
    }

    void lt_code_generator()
    {
        code.append("//lt\n");
        code.append("@0\n");
        code.append("A=M\n");
        code.append("D=A-1\n");
        code.append("D=D-1\n");
        code.append("@addr\n");
        code.append("M=D\n");

        code.append("@0\n");
        code.append("A=M\n");
        code.append("A=A-1\n");
        code.append("D=M\n");
        code.append("A=A-1\n");
        code.append("D=M-D\n");

        code.append(std::string("@label") + std::to_string(label_count) + "\n");
        code.append("D;JLT\n");
        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=0\n");
        code.append(std::string("@next" + std::to_string(label_count) + "\n"));
        code.append("0;JMP\n");

        code.append(std::string("(label") + std::to_string(label_count) + ")\n");
        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=-1\n");

        code.append(std::string("(next" + std::to_string(label_count) + ")\n"));

        code.append("@addr\n");
        code.append("D=M+1\n");
        code.append("@0\n");
        code.append("M=D\n");

        label_count += 1;
    }

    void and_code_generator()
    {
        code.append("//and\n");
        code.append("@0\n");
        code.append("A=M-1\n");
        code.append("D=A-1\n");
        code.append("@addr\n");
        code.append("M=D\n");

        code.append("@0\n");
        code.append("A=M\n");
        code.append("A=A-1\n");
        code.append("D=M\n");
        code.append("A=A-1\n");
        code.append("D=D&M\n");

        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=D\n");

        code.append("@addr\n");
        code.append("D=M+1\n");
        code.append("@0\n");
        code.append("M=D\n");
    }

    void or_code_generator()
    {
        code.append("//or\n");
        code.append("@0\n");
        code.append("A=M-1\n");
        code.append("D=A-1\n");
        code.append("@addr\n");
        code.append("M=D\n");

        code.append("@0\n");
        code.append("A=M-1\n");
        code.append("D=M\n");
        code.append("A=A-1\n");
        code.append("D=D|M\n");

        code.append("@addr\n");
        code.append("A=M\n");
        code.append("M=D\n");

        code.append("@addr\n");
        code.append("D=M+1\n");
        code.append("@0\n");
        code.append("M=D\n");
    }

    void not_code_generator()
    {
        code.append("//not\n");
        code.append("@0\n");
        code.append("A=M-1\n");
        code.append("D=M\n");
        code.append("M=!D\n");
    }




public:
    explicit code_writer(const std::string filename)
    {
        output_handle.open(filename);
        for(int i = 0;i < filename.size();i++)
        {
            if(filename[i] == '.')
            {
                break;
            }
            static_name.push_back(filename[i]);
        }
        if (!output_handle.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    
    
    //This function take the commands and convert the command to suitable assembly code

    void code_generator_main(command_type type,std::string arg1 = "",std::string arg2 = "") //arg1 and arg2 are optional
    {
        code = "";
        if(type == command_type::C_ARITHMETIC)
        {
            if(arg1 == "add")
            {
                add_code_generator();
            }
            else if(arg1 == "sub")
            {
                sub_code_generator();
            }
            else if(arg1 == "neg")
            {
                neg_code_generator();
            }
            else if(arg1 == "eq")
            {
                eq_code_generator();
            }
            else if(arg1 == "gt")
            {
                gt_code_generator();
            }

            else if (arg1 == "lt")
            {
                lt_code_generator();
            }
            else if (arg1 == "and")
            {
                and_code_generator();
            }
            else if(arg1 == "or")
            {
                or_code_generator();
            }
            else if(arg1 == "not")
            {
                not_code_generator();
            }

        }

        else if(type == command_type::C_POP)
        {
            pop_code_generator(arg1,arg2);
        }

        else if(type == command_type::C_PUSH)
        {
            push_code_generator(arg1,arg2);
        }

        output_handle << code;
    }


    //prints the current code
    void print()
    {
        std::cout << code;
    }

};

std::string output_file_name(std::string if_name)
{
    std::string of_name;
    for(int i = 0; i < if_name.size(); i++)
    {
        if(if_name[i] != '.')
        {
            of_name.push_back(if_name[i]);
            continue;
        }
        break;
    }

    of_name += ".asm";
    return of_name;
}

// std::vector<std::string> get_file_names(std::string dir_name)
// {

// } 

bool check_dir_file(std::string name)
{
    for(int i = 0; i < name.size() ; i++)
    {
        if(name[i] == '.')
        {
            if(name[i+1] == 'v' && name[i+2] == 'm')
            {
                return false;
            }
        }
    }
    return true;
}


bool check_vm(std::string name)
{
    for(int i = 0; i<name.size();i++)
    {
        if(name[i] == '.')
        {
            if(name[i+1] == 'v' && name[i+2] == 'm' && (i+3) == name.size())
            {
                return true;
            }
        }
    }
    return false;
}


int main(int argc,char *argv[])
{

    std::vector<std::string> vm_file_name;
    bool dir_or_not;

    if(argc == 2)
    {
        if(check_dir_file(argv[1]))
        {
        for (const auto & entry : fs::directory_iterator(argv[1]))
            if(check_vm(entry.path()))
            {
                vm_file_name.push_back(entry.path());
                dir_or_not = true;
            }
        }
        else if(check_vm(argv[1]))
        {
            vm_file_name.push_back(argv[1]);
            dir_or_not  = false;
        }
    
        else 
        {
            std::runtime_error("Invalid file name");
        }
    }
    else
    {
        for(int i = 1; i < argc; i++)
        {
            if(check_vm(argv[i]))
            {
                vm_file_name.push_back(argv[i]);
                dir_or_not = false;
            }
            else 
            {
                std::runtime_error("Invalid file name");
            }
        }
    }

    if(dir_or_not)
    {
        std::string out_name = std::string(argv[1]) + "/" + argv[1] + ".asm";
        code_writer cw(out_name);
        for (int i = 0; i < vm_file_name.size(); i++)
        {
            parser p{vm_file_name[i]};
            while(p.has_more_commands())
            {
                p.advance();
                cw.code_generator_main(p.ret_commandtype(),p.argument_1(),p.argument_2());
            }
        }
    }
    else
    {
        for (int i = 0; i < vm_file_name.size(); i++)
        {
            parser p{vm_file_name[i]};
            std::string out_name = output_file_name(vm_file_name[i]);
            code_writer cw(out_name);
            while(p.has_more_commands())
            {
                p.advance();
                cw.code_generator_main(p.ret_commandtype(),p.argument_1(),p.argument_2());
            }
        }
    }
    


    
}

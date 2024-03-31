#ifndef BUG_H_
#define BUG_H_

#include <fstream>

#ifndef DEBUG
    //#define DEBUG
#endif

/*
    struct for debugging - this is gross but can be used pretty much like an ofstream,
                           except the debug messages are stripped while compiling if
                           DEBUG is not defined.
    example:
        Bug bug;
        bug.Open("./debug.txt");
        bug << _state << endl;
        bug << "testing" << 2.0 << '%' << endl;
        bug.Close();
*/
struct Bug
{
    std::ofstream file;

    Bug() = default;

    //opens the specified file
    void Open(const std::string &r_filename)
    {
        #ifdef DEBUG
            file.open(r_filename.c_str());
        #endif
    }

    //closes the ofstream
    void Close()
    {
        #ifdef DEBUG
            file.close();
        #endif
    }
};

//output function for endl
inline Bug& operator<<(Bug &bug, std::ostream& (*p_manipulator)(std::ostream&))
{
    #ifdef DEBUG
        bug.file << p_manipulator;
    #endif

    return bug;
};

//output function
template <class T>
inline Bug& operator<<(Bug &r_bug, const T &r_t)
{
    #ifdef DEBUG
        r_bug.file << r_t;
    #endif

    return r_bug;
};

#endif //BUG_H_

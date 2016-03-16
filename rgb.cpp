using namespace std;
class rgb{
public:
    int r;
    int g;
    int b;
    rgb(int rGiven, int gGiven, int bGiven){
        r = rGiven;
        g = gGiven;
        b = bGiven;
    }
    rgb(){
        r = -1;
        g = -1;
        b = -1;
    }
    bool isEqual(rgb RGB){
        return r == RGB.r && g == RGB.g && b == RGB.b;
    }
    bool isClose(rgb RGB){
        return abs(r - RGB.r) <= COLOR_TOLERENCE && 
            abs(g - RGB.g) <= COLOR_TOLERENCE && 
            abs(b - RGB.b) <= COLOR_TOLERENCE;
    }
    int sum(){
        return r + g + b;
    }
    // std::ostream & operator<<(std::ostream & Str, rgb const & v) { 
    //     // print something from v to str, e.g: Str << v.getX();
    //     string retString = "r: " + to_string(r) + " g: " + to_string(g) + " b: " + to_string(b);
    //     return  retString.c_str();
    // }
    operator std::string() const { return "r: " + to_string(r) + " g: " + to_string(g) + " b: " + to_string(b); }
};
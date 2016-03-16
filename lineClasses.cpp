using namespace std;
struct listColorRet{
public:
    rgb *rgbValues;
    int rgbValuesSize;
    int *occurenceCounter;
    listColorRet(){
    }
    listColorRet(rgb rgbArr[], int max, int counter[]){
        rgbValues = rgbArr;
        rgbValuesSize = max;
        occurenceCounter = new int[max];
        occurenceCounter = counter;
    }
};
struct myLine{
public:
    double m;
    int b;
    myLine(){
        m = 0;
        b = 0;
    }
    myLine(int _b, double _m){
        m = _m;
        b = _b;
    }
    void setLine(int _b, double _m){
        m = _m;
        b = _b;
    }
    operator std::string() const {
        return "m: " + to_string(m) + " b: " + to_string(b);
    }
};
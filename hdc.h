struct hdc{
    int init;
    int x;
    int y;
    int penIndex;
}; //Forward declaration

struct hdcArray{
    struct hdc hdcObjects[5]; //Holds 5 hdc objects
};

struct curHDC{
    int index;
};
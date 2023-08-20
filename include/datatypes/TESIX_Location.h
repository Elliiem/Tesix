#pragma once

struct TESIX_Location{
    public:
    TESIX_Location(){
        this->line = 0;
        this->col = 0;
    }
    TESIX_Location(int line, int col){
        this->line = line;
        this->col = col;
    }
    ~TESIX_Location(){}
    public:
    int line;
    int col;

    public:

    private:

    private:

};

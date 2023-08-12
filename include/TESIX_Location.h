struct TESIX_Location{
    public:
    TESIX_Location(){}
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

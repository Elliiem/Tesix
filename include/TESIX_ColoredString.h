#include <vector>
#include <string>
#include <cinttypes>


enum TESIX_Colors{
    TESIX_COLORS_NONE = 0,
    // TESIX_COLORS_NONE_ERR = 1,
    // TESIX_COLORS_NONE_HIGH = 2,
    TESIX_COLORS_PRIM_TYPE = 3,
    // TESIX_COLORS_PRIM_TYPE_ERR = 4,
    // TESIX_COLORS_PRIM_TYPE_HIGH = 5,
    TESIX_COLORS_TYPE = 6,
    // TESIX_COLORS_TYPE_ERR = 7,
    // TESIX_COLORS_TYPE_HIGH = 8,
    TESIX_COLORS_FUNC = 9,
    // TESIX_COLORS_FUNC_ERR = 10,
    // TESIX_COLORS_FUNC_HIGH = 11,
    TESIX_COLORS_VAR = 12,
    // TESIX_COLORS_VAR_ERR = 13,
    // TESIX_COLORS_VAR_HIGH = 14,
    TESIX_COLORS_NULL = 15
};


struct TESIX_Color{
    public:
    TESIX_Color(uint32_t base_col);
    TESIX_Color();
    ~TESIX_Color();
    public:

    public:
    TESIX_Color ClearMod();
    TESIX_Color ToError();
    TESIX_Color ToHighlight();
    
    bool operator==(TESIX_Color& other);

    uint32_t GetCurrent();
    uint32_t GetBase();

    private:
    uint32_t base;
    uint32_t cur;

    private:

};

struct TESIX_ColorIndexPair{
    public:
    TESIX_ColorIndexPair(TESIX_Color color, uint32_t index);
    TESIX_ColorIndexPair();
    ~TESIX_ColorIndexPair();
    public:
    TESIX_Color color;
    uint32_t index;

    public:

    private:

    private:
};

class TESIX_ColoredString{
    public:
    TESIX_ColoredString(std::string str);
    TESIX_ColoredString(std::vector<TESIX_ColorIndexPair> colors, std::string str);
    ~TESIX_ColoredString();
    public:

    public:
    // Returns the next substring and the color its in
    std::pair<std::string, uint32_t> NextColorSegment();

    // Set colors within the range between start and end to be highlighted or to have an error
    void SetHighlight(uint32_t start, uint32_t end);
    void SetError(uint32_t start, uint32_t end);
    void ClearMod(uint32_t start, uint32_t end);

    // For Constructing a colored string
    TESIX_ColoredString* AddColor(TESIX_ColorIndexPair color_index_pair);
    TESIX_ColoredString* InsertColor(TESIX_ColorIndexPair color_index_pair);

    void SetColors(std::vector<TESIX_ColorIndexPair> new_colors);
    void ClearColors();

    uint32_t LastColoredIndex();

    uint32_t ColorAt(uint32_t index);

    void Simplyfy();

    // tmp
    void PrintColors();
    
    private:
    std::vector<TESIX_ColorIndexPair> colors;
    
    uint32_t next_cur_index;

    std::string colored_string;

    private:
    uint32_t GetColorIndex(uint32_t index);
    std::pair<uint32_t, uint32_t> CreateInterval(uint32_t start, uint32_t end);

};
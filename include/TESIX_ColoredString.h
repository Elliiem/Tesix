#include <vector>
#include <string>
#include <cinttypes>

// COLORS
// NONE 0
// PRIMITIVE TYPES:
//    STD: 1
//    ERR: 2
//    HIGH: 3
// TYPES 4, 5, 6
// FUNCTIONS 7, 8, 9
// VARS 10, 11, 12


class TESIX_ColoredString{
    public:
    TESIX_ColoredString(std::string str);
    TESIX_ColoredString(std::vector<std::pair<uint32_t, uint32_t>> colors, std::string str);
    ~TESIX_ColoredString();
    public:
    // Returns the next substring and the color its in
    std::pair<std::string, uint32_t> NextColor();

    // Set colors within the range between start and end to be highlighted or to have an error
    void SetHighlight(uint32_t start, uint32_t end);
    void SetError(uint32_t start, uint32_t end);

    // For Constructing a colored string
    int32_t AddColor(uint32_t index, uint32_t color);

    uint32_t LastColored();

    public:

    private:
    uint32_t cur_index;
    std::vector<std::pair<uint32_t, uint32_t>> colors;
    // index color
    // 0, 0
    // 4, 10
    // ...


    std::string str;

    private:

};
#include "TESIX_Selection.h"

bool TESIX_Selection::IsMultiline(){
    return start.line != end.line;
}
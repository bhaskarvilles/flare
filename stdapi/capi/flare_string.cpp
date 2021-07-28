//
// Created by jobinrjohnson on 24/07/21.
//

#include <string>

using namespace std;

extern "C" {

struct FLARE_string_t {
    int8_t *str;
};

void printString(struct FLARE_string_t s) {
    printf("%s\n", ((std::string *) s.str)->c_str());
}

void FLARE_str_init(FLARE_string_t *s, int8_t *charSeq) {
    auto *str = new std::string();
    str->assign((const char *) charSeq);
    s->str = (int8_t *) str;
}

void FLARE_str_concat(struct FLARE_string_t s1, struct FLARE_string_t s2) {
    ((string *) s1.str)->append(*(string *) s2.str);
}

bool FLARE_str_is_equal(struct FLARE_string_t s1, struct FLARE_string_t s2) {
    return *((string *) s1.str) == *(string *) s2.str;
}

}
#include <vector>
#include <string>

using namespace std;

class MagicDictionary {
public:
    MagicDictionary() {
    }

    void buildDict(vector<string> dictionary) {
        dictionary_ = dictionary;
    }

    bool search(string searchWord) {
        for (auto&& c : searchWord) {
            for (size_t i = 0; i < 26; i++) {
                char tmp = c;
                if (c == i + 'a') {
                    continue;
                }
                c = i + 'a';
                for (auto&& dic : dictionary_) {
                    if (dic.size() != searchWord.size()) {
                        continue;
                    }
                    if (dic == searchWord) {
                        return true;
                    }
                }
                c = tmp;
            }
        }
        return false;
    }

private:
    vector<string> dictionary_;
};

/**
 * Your MagicDictionary object will be instantiated and called as such:
 * MagicDictionary* obj = new MagicDictionary();
 * obj->buildDict(dictionary);
 * bool param_2 = obj->search(searchWord);
 */
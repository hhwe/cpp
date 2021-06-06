#include "query.h"

using namespace std;

typedef shared_ptr<set<TextQuery::line_no>> lineType;

// TextQuery
TextQuery::TextQuery(ifstream &ifs) : file(new vector<string>)
{
    string text;
    while (getline(ifs, text))
    {
        file->push_back(text);
        int n = file->size() - 1;
        istringstream line(text);
        string word;
        while (line >> word)
        {
            word = cleanup_str(word);
            lineType &lines = wm[word];
            if (!lines)
            {
                lines.reset(new set<line_no>);
            }
            lines->insert(n);
        }
    }
};

string TextQuery::cleanup_str(const string &word)
{
    string ret;
    for (string::const_iterator it = word.begin(); it != word.end(); ++it)
    {
        if (ispunct(*it))
        {
            ret += tolower(*it);
        }
    }
    return ret;
}

QueryResult TextQuery::query(const string &sought) const
{
    static lineType nodata(new set<line_no>);
    auto loc = wm.find(cleanup_str(sought));

    if (loc == wm.end())
    {
        return QueryResult(sought, nodata, file);
    }
    else
    {
        return QueryResult(sought, loc->second, file);
    }
}

void TextQuery::display_map()
{
    for (auto iter = wm.begin(); iter != wm.end(); iter++)
    {
        cout << "word: " << iter->first << " {";
        lineType text_locs = iter->second;
        auto loc_iter = text_locs->begin(), loc_iter_end = text_locs->end();
        while (loc_iter != loc_iter_end)
        {
            cout << *loc_iter;
            if (++loc_iter != loc_iter_end)
            {
                cout << ", ";
            }
        }
        cout << "}\n";
    }
    cout << endl;
}

ostream &print(ostream &os, const QueryResult &qr)
{
    os << qr.sought << " occurs " << qr.lines->size() << " " << make_plural(qr.lines->size(), "time", "s") << endl;
    for (auto num = qr.lines->begin(); num != qr.lines->end(); ++num)
    {
        os << "\t(line " << *num + 1 << ") " << *(qr.file->begin() + *num) << endl;
    }
    return os;
}

// Test::TextQuery
void runQueries(ifstream &infile)
{
    TextQuery tq(infile);
    tq.display_map();
    while (true)
    {
        cout << "enter word to looc for, or q to quit: ";
        string s;
        if (!(cin >> s) || s == "q")
        {
            break;
        }
        print(cout, tq.query(s)) << endl;
    }
}

// returns the lines not in its operand's result set
QueryResult NotQuery::eval(const TextQuery &text) const
{
    // virtual call to eval through the Query operand
    QueryResult result = query.eval(text);

    // start out with an empty result set
    shared_ptr<set<line_no>> ret_lines(new set<line_no>);

    // we have to iterate through the lines on which our operand appears
    QueryResult::line_it beg = result.begin(), end = result.end();

    // for each line in the input file, if that line is not in result,
    // add that line number to ret_lines
    vector<string>::size_type sz = result.get_file()->size();
    for (size_t n = 0; n != sz; ++n)
    {
        // if we haven't processed all the lines in result
        // check whether this line is present
        if (beg == end || *beg != n)
            ret_lines->insert(n); // if not in result, add this line
        else if (beg != end)
            ++beg; // otherwise get the next line number in result if there is one
    }
    return QueryResult(rep(), ret_lines, result.get_file());
}

// returns the intersection of its operands' result sets
QueryResult AndQuery::eval(const TextQuery &text) const
{
    // virtual calls through the Query operands to get result sets for the operands
    QueryResult left = lhs.eval(text), right = rhs.eval(text);

    // set to hold the intersection of left and right
    shared_ptr<set<line_no>> ret_lines(new set<line_no>);

    // writes the intersection of two ranges to a destination iterator
    // destination iterator in this call adds elements to ret
    set_intersection(left.begin(), left.end(),
                     right.begin(), right.end(),
                     inserter(*ret_lines, ret_lines->begin()));
    return QueryResult(rep(), ret_lines, left.get_file());
}

// returns the union of its operands' result sets
QueryResult OrQuery::eval(const TextQuery &text) const
{
    // virtual calls through the Query members, lhs and rhs
    // the calls to eval return the QueryResult for each operand
    QueryResult right = rhs.eval(text), left = lhs.eval(text);

    // copy the line numbers from the left-hand operand into the result set
    shared_ptr<set<line_no>>
        ret_lines(new set<line_no>(left.begin(), left.end()));

    // insert lines from the right-hand operand
    ret_lines->insert(right.begin(), right.end());
    // return the new QueryResult representing the union of lhs and rhs
    return QueryResult(rep(), ret_lines, left.get_file());
}

// these functions are declared in Query.h
TextQuery get_file(int argc, char **argv)
{
    // get a file to read from which user will query words
    ifstream infile;
    if (argc == 2)
        infile.open(argv[1]);
    if (!infile)
    {
        throw runtime_error("No input file!");
    }

    return TextQuery(infile); // builds query map
}

bool get_word(string &s1)
{
    cout << "enter a word to search for, or q to quit: ";
    cin >> s1;
    if (!cin || s1 == "q")
        return false;
    else
        return true;
}

bool get_words(string &s1, string &s2)
{

    // iterate with the user: prompt for a word to find and print results
    cout << "enter two words to search for, or q to quit: ";
    cin >> s1;

    // stop if hit eof on input or a "q" is entered
    if (!cin || s1 == "q")
        return false;
    cin >> s2;
    return true;
}

// ============================ Unit Test Query ============================
int TextQueryTest(int argc, char **argv)
{
    cout << "<<<<<<<<<<<<<<<<<<<<<<<< QueryResultTest >>>>>>>>>>>>>>>>>>>>>>" << endl;
    // open the file from which user will query words
    ifstream infile;
    // open returns void, so we use the comma operator XREF(commaOp)
    // to check the state of infile after the open
    if (argc < 2 || !(infile.open(argv[1]), infile))
    {
        cerr << "No input file!" << endl;
        return EXIT_FAILURE;
    }
    runQueries(infile);
    return 0;
}

int AndQueryTest(int argc, char **argv)
{
    cout << "<<<<<<<<<<<<<<<<<<<<<<<< AndQueryTest >>>>>>>>>>>>>>>>>>>>>>" << endl;
    // gets file to read and builds map to support queries
    TextQuery file = get_file(argc, argv);

    do
    {
        string sought1, sought2;
        // stop if hit eof on input or a "q" is entered
        if (!get_words(sought1, sought2))
            break;

        // find all the occurrences of the requested string
        Query andq = Query(sought1) & Query(sought2);
        cout << "\nExecuting query for: " << andq << endl;
        QueryResult results = andq.eval(file);
        // report matches
        print(cout, results);

        results = Query(sought1).eval(file);
        cout << "\nExecuted query: " << Query(sought1) << endl;
        // report matches
        print(cout, results);

        results = Query(sought2).eval(file);
        cout << "\nExecuted query: " << Query(sought2) << endl;
        // report matches
        print(cout, results);
    } while (true);

    return 0;
}

int AndOrQueryTest(int argc, char **argv)
{
    cout << "<<<<<<<<<<<<<<<<<<<<<<<< AndOrQueryTest >>>>>>>>>>>>>>>>>>>>>>" << endl;
    // gets file to read and builds map to support queries
    TextQuery file = get_file(argc, argv);

    // iterate with the user: prompt for a word to find and print results
    while (true)
    {
        string sought1, sought2, sought3;
        if (!get_words(sought1, sought2))
            break;
        cout << "\nenter third word: ";
        cin >> sought3;
        // find all the occurrences of the requested string
        Query q = Query(sought1) & Query(sought2) | Query(sought3);
        cout << "\nExecuting Query for: " << q << endl;
        const QueryResult results = q.eval(file);
        // report matches
        print(cout, results);
    }
    return 0;
}

// program takes single argument specifying the file to query
int main(int argc, char **argv)
{
    TextQueryTest(argc, argv);
    AndQueryTest(argc, argv);
    AndOrQueryTest(argc, argv);
    return 0;
}
